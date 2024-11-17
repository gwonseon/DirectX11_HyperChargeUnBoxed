#include "stdafx.h"
#include "..\Public\Coin_Item.h"

#include "GameInstance.h"


CCoin_Item::CCoin_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CCoin_Item::CCoin_Item(const CCoin_Item& Prototype)
    : CGameObject{ Prototype }
    , m_vecItemPos{Prototype.m_vecItemPos }
{
}

HRESULT CCoin_Item::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CCoin_Item::Initialize(void* pArg)
{
    COINITEM_DESC* pDesc = static_cast<COINITEM_DESC*>(pArg);
    m_pPlayer = pDesc->pPlayer;
    m_pGuage = pDesc->pGuage;

    m_fScale = pDesc->fScale;
    m_iModelIndex = pDesc->iModelIndex;
    m_eLevel = pDesc->eID;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f));
    m_pTransformCom->Rotation(XMConvertToRadians(90.f), 0.f, 0.f);
    // 중점위치 변경 ( 회전 위치를 바꿔줌)
    _float4x4 matSecondPreTransform{};
    XMStoreFloat4x4(&matSecondPreTransform, XMMatrixTranslation(0.f, 0.f, -0.15f));
    m_pModelCom->Set_SecondPreTransform(matSecondPreTransform);
    m_fCharging_Time = 0.f;
    switch (m_iModelIndex)
    {
    case 47:
    {
        m_iCoin = 100;
        break;
    }
    case 48:
    {
        m_iCoin = 80;
        break;
    }
    case 49:
    {
        m_iCoin = 50;
        break;
    }
    default:
        break;
    }
    



    return S_OK;
}

void CCoin_Item::Priority_Update(_float fTimeDelta)
{    
    // 회전
    _vector Axis = { 0.f, 1.f, 0.f };
    m_pTransformCom->Turn(Axis, fTimeDelta * 0.3f);
}

void CCoin_Item::Update(_float fTimeDelta)
{
    if (m_bDead)
        return;

    _vector vecPlayerPos = m_pPlayer->Get_Position();
    m_vecItemPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    // 플레이어와 아이템 거리가 가까워졌을 때
    if (m_pTransformCom->Cal_Distance_vec(vecPlayerPos, m_vecItemPos) <= 50.f)
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
        m_bInteraction = false;
        m_pTransformCom->Set_Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
        m_fCharging_Time = 0.f;
    }


    if (m_fCharging_Time >= 1.f)
    {
        m_bInteraction = false; 
        m_pPlayer->Set_PickUp_Coin(m_iCoin);
        m_bDead = true;
    }

}

void CCoin_Item::Late_Update(_float fTimeDelta)
{
    if (m_bDead == false)
    {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
            return;
    }

}

HRESULT CCoin_Item::Render()
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

HRESULT CCoin_Item::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxItem"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    const _wstring Model_Component = TEXT("Prototype_Component_Model_Environment");
    const _wstring Model_Component_Result = Model_Component + to_wstring(m_iModelIndex + ENVIRONMENT_EA);
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CCoin_Item::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_SecondMatrix", m_pModelCom->Get_SecondPreTransform())))
        return E_FAIL;
    auto SecondPreTrans = *m_pModelCom->Get_SecondPreTransform();
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _float fFar = m_pGameInstance->Get_CameraFar();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
        return E_FAIL;
    /*if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
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

CCoin_Item* CCoin_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCoin_Item* pInstance = new CCoin_Item(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCoin_Item");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CCoin_Item::Clone(void* pArg)
{
    CCoin_Item* pInstance = new CCoin_Item(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CCoin_Item");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCoin_Item::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
