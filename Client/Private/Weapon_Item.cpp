#include "stdafx.h"
#include "..\Public\Weapon_Item.h"

#include "GameInstance.h"
#include <InGameUI.h>


CWeapon_Item::CWeapon_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CWeapon_Item::CWeapon_Item(const CWeapon_Item& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CWeapon_Item::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon_Item::Initialize(void* pArg)
{
    WEAPONITEM_DESC* pDesc = static_cast<WEAPONITEM_DESC*>(pArg);
    m_eLevel = pDesc->eID;
    m_iModelIndex = pDesc->iModelIndex;
    pDesc->fRotationPerSec = 5.f;
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f));
    m_pTransformCom->Set_Scaling(pDesc->fScale.x, pDesc->fScale.y, pDesc->fScale.z);
    m_vecPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
 
    _float4x4 matSecondPreTransform{};
    XMStoreFloat4x4(&matSecondPreTransform, XMMatrixTranslation(0.f, 0.f, -0.15f));
    m_pModelCom->Set_SecondPreTransform(matSecondPreTransform);
    if (m_iModelIndex == 7) // Ä«Å¸³ª
    {
    
        Rotation = { XMConvertToRadians(90.5), XMConvertToRadians(47), XMConvertToRadians(32) };
        m_pTransformCom->Rotation(Rotation.x, Rotation.y, Rotation.z);
    }
    return S_OK;
}

void CWeapon_Item::Priority_Update(_float fTimeDelta)
{
    m_vecPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

}

void CWeapon_Item::Update(_float fTimeDelta)
{
    if (m_bDead)
    {
        return;
    }
   
   


    _vector Axis = { 0.f, 1.f, 0.f };
    
    if (m_bInteration == true)
    {
        m_pTransformCom->Set_Scaling(7.f, 7.f, 7.f);
        m_pTransformCom->Turn(Axis, fTimeDelta * 0.3f);
        if (m_bCharging == true)
        {
            m_fCharging += fTimeDelta;
        }
        else
            m_fCharging = 0.f;
        if (m_fCharging >= 1.f)
        {
            m_bEquip = true;
            m_bDead = true;
        }
    }
    else
    {
        m_fCharging = 0.f;
        m_pTransformCom->Set_Scaling(4.f, 4.f, 4.f);
    }

}

void CWeapon_Item::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
}

HRESULT CWeapon_Item::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }


    return S_OK;
}

HRESULT CWeapon_Item::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxItem"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
 
   const _wstring Model_Component = TEXT("Prototype_Component_Model_Weapon");
   const _wstring Model_Component_Result = Model_Component + to_wstring(m_iModelIndex +1);
    /* For.Com_Model */
    ;
    if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeapon_Item::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_SecondMatrix", m_pModelCom->Get_SecondPreTransform())))
        return E_FAIL;

    auto aa = *m_pModelCom->Get_SecondPreTransform();
    
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CWeapon_Item* CWeapon_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWeapon_Item* pInstance = new CWeapon_Item(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CWeapon_Item");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWeapon_Item::Clone(void* pArg)
{
    CWeapon_Item* pInstance = new CWeapon_Item(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CWeapon_Item");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeapon_Item::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);


}