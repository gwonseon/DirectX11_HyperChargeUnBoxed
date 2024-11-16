#include "stdafx.h"
#include "..\Public\Hp_Item.h"

#include "GameInstance.h"

CHp_Item::CHp_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CHp_Item::CHp_Item(const CHp_Item& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CHp_Item::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHp_Item::Initialize(void* pArg)
{
    HPITEM_DESC* pDesc = static_cast<HPITEM_DESC*>(pArg);
    m_pPlayer = pDesc->pPlayer;
    m_pGuage = pDesc->pGuage;

    m_fScale = pDesc->fScale;
    m_eLevel = pDesc->eID;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f));


    return S_OK;
}

void CHp_Item::Priority_Update(_float fTimeDelta)
{
    // 회전
    _vector Axis = { 0.f, 1.f, 0.f };
    m_pTransformCom->Turn(Axis, fTimeDelta * 0.3f);
}

void CHp_Item::Update(_float fTimeDelta)
{
    if (m_bDead)
        return;

    _vector vecPlayerPos = m_pPlayer->Get_Position();
    m_vecItemPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    // 플레이어와 아이템 거리가 가까워졌을 때
    if (m_pTransformCom->Cal_Distance_vec(vecPlayerPos, m_vecItemPos) <= 80.f)
    {
        // 사이즈 커지기
        m_pTransformCom->Set_Scaling(m_fScale.x + 2.f, m_fScale.y + 2.f, m_fScale.z + 2.f);

        if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_E))
        {
            m_bInteraction = true;

            m_fCharging_Time += fTimeDelta;
        }
        else
        {
            m_bInteraction = false;
            m_fCharging_Time = 0.f;
        }

    }
    else
    {
        m_pTransformCom->Set_Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
        m_fCharging_Time = 0.f;
    }


    if (m_fCharging_Time >= 1.f)
    {
        m_pPlayer->Set_FullHeal();
        m_bDead = true;
    }

}

void CHp_Item::Late_Update(_float fTimeDelta)
{
    if (m_bDead == false)
    {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
            return;
    }
}

HRESULT CHp_Item::Render()
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

HRESULT CHp_Item::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxItem"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    const _wstring Model_Component = TEXT("Prototype_Component_Model_Environment");
    const _wstring Model_Component_Result = Model_Component + to_wstring(50 + ENVIRONMENT_EA);
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;
    return S_OK;
}

HRESULT CHp_Item::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
   /* if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
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
        return E_FAIL;*/
    return S_OK;
}

CHp_Item* CHp_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHp_Item* pInstance = new CHp_Item(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CHp_Item");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CHp_Item::Clone(void* pArg)
{
    CHp_Item* pInstance = new CHp_Item(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CHp_Item");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHp_Item::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
