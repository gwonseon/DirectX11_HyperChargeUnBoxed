
#include "stdafx.h"
#include "..\Public\TruckShooter.h"

#include "GameInstance.h"
#include "Missile_Truck.h"


CTruckShooter::CTruckShooter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTruckShooter::CTruckShooter(const CTruckShooter& Prototype)
    : CGameObject{ Prototype }
{
}


HRESULT CTruckShooter::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTruckShooter::Initialize(void* pArg)
{
    TRUCKSHOOTER_DESC* pDesc = static_cast<TRUCKSHOOTER_DESC*>(pArg);
    m_eLevelID = pDesc->m_eLevelID;
    m_iRound = pDesc->iRound;
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Add_Components()))
        return E_FAIL;


    fRotX = 1.58f;
    m_pTransformCom->Rotation(0.f, XMConvertToRadians(m_fRotY), 0.f);
    m_pTransformCom->Set_Scaling(pDesc->fScale.x, pDesc->fScale.y, pDesc->fScale.z);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->fPosition.x , pDesc->fPosition.y, pDesc->fPosition.z , 1.f));

    _float4x4 matSecondPreTransform{};
    XMStoreFloat4x4(&matSecondPreTransform, XMMatrixTranslation(-3.9f, -0.3f, -0.f));
    m_pModelCom[0]->Set_SecondPreTransform(matSecondPreTransform);
    m_pModelCom[1]->Set_SecondPreTransform(matSecondPreTransform);



    return S_OK;
}

void CTruckShooter::Priority_Update(_float fTimeDelta)
{
    m_vecPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    if (m_bKnockdown == true)
        return;
    if (m_bReady_Shot == false)
        m_iModelIdx = 0;
    else
        m_iModelIdx = 1;


    if (m_bStart_Shoot == true)
    {
        if (fRotX >= 0.f)
        {
            m_vecPosition = XMVectorSetY(m_vecPosition, XMVectorGetY(m_vecPosition) - 0.01f);
            m_vecPosition = XMVectorSetZ(m_vecPosition, XMVectorGetZ(m_vecPosition) - 0.01f);
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);
            fRotX -= 0.01f;
            m_bReady_Shot = false;
        }
        else
            m_bReady_Shot = true;
    }
    if (m_bStart_Shoot == false)
    {
        m_bReady_Shot = false;
        if (fRotX < 1.58f)
        {
        m_vecPosition = XMVectorSetY(m_vecPosition, XMVectorGetY(m_vecPosition) + 0.01f);
        m_vecPosition = XMVectorSetZ(m_vecPosition, XMVectorGetZ(m_vecPosition) + 0.01f);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);
        fRotX += 0.01f;
        }

    }
    m_pTransformCom->Rotation(0.f, XMConvertToRadians(m_fRotY), fRotX);


    if (*m_iRound == 1)
    {
        m_bStart_Shoot = true;
    }
    else
    {
        m_bStart_Shoot = false;
    }

}

void CTruckShooter::Update(_float fTimeDelta)
{
    if (m_bKnockdown == true)
    {
        if (XMVectorGetY(m_vecPosition) < 5.f)
        {
            m_vecPosition = XMVectorSetY(m_vecPosition, XMVectorGetY(m_vecPosition) + (fTimeDelta * 10.f));
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);
            
        }
        if (fRotZ >= -90.f)
            fRotZ -= fTimeDelta * 30.f;
        m_pTransformCom->Rotation(XMConvertToRadians(fRotZ), XMConvertToRadians(m_fRotY), fRotX);
        return;
    }

}

void CTruckShooter::Late_Update(_float fTimeDelta)
{
    if (m_bDead == false)
    {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
            return;
    }
}

HRESULT CTruckShooter::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom[m_iModelIdx]->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom[m_iModelIdx]->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
            return E_FAIL;


        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom[m_iModelIdx]->Render(i);
    }

    return S_OK;
}

HRESULT CTruckShooter::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxItem"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevelID, TEXT("Prototype_Component_Model_Trap15"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom[0]))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevelID, TEXT("Prototype_Component_Model_Trap14"),
        TEXT("Com_Model2"), reinterpret_cast<CComponent**>(&m_pModelCom[1]))))
        return E_FAIL;
    return S_OK;
}

HRESULT CTruckShooter::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

   if (FAILED(m_pShaderCom->Bind_Matrix("g_SecondMatrix", m_pModelCom[0]->Get_SecondPreTransform())))
       return E_FAIL;

   if (FAILED(m_pShaderCom->Bind_Matrix("g_SecondMatrix", m_pModelCom[1]->Get_SecondPreTransform())))
       return E_FAIL;

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

CTruckShooter* CTruckShooter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTruckShooter* pInstance = new CTruckShooter(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTruckShooter");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTruckShooter::Clone(void* pArg)
{
    CTruckShooter* pInstance = new CTruckShooter(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CTruckShooter");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTruckShooter::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom[0]);
    Safe_Release(m_pModelCom[1]);
    Safe_Release(m_pShaderCom);
}
