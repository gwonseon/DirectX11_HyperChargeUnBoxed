#include "stdafx.h"
#include "..\Public\Bulb.h"

#include "GameInstance.h"


CBulb::CBulb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CBulb::CBulb(const CBulb& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CBulb::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBulb::Initialize(void* pArg)
{
    BULB_DESC* pDesc = static_cast<BULB_DESC*>(pArg);
    m_eLevel = pDesc->eID;
    m_eBulbType = pDesc->eBulbType;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Add_Components()))
        return E_FAIL;

    if (m_eBulbType == BULB_SPOT)
    {
        m_pTransformCom->Rotation(-0.107432f, 0.602907f, 0.23939f);
    }
    m_pTransformCom->Set_Scaling(pDesc->fScale.x + 2.f, pDesc->fScale.y + 2.f, pDesc->fScale.z + 2.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f));

    return S_OK;
}

void CBulb::Priority_Update(_float fTimeDelta)
{

}

void CBulb::Update(_float fTimeDelta)
{
}

void CBulb::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONLIGHT, this)))
        return;
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
        return;

}

HRESULT CBulb::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
            return E_FAIL;

        switch (m_eBulbType)
        {
        case Client::CBulb::BULB_CEIL:
            if (FAILED(m_pShaderCom->Begin(15)))
                return E_FAIL;
            break;
        case Client::CBulb::BULB_SPOT:
            if (FAILED(m_pShaderCom->Begin(16)))
                return E_FAIL;
            break;
        case Client::CBulb::BULB_END:
            break;
        default:
            break;
        }
    
        m_pModelCom->Render(i);

    }

    return S_OK;

}

HRESULT CBulb::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    _uint iModelNum{};
    switch (m_eBulbType)
    {
    case Client::CBulb::BULB_CEIL:
        iModelNum = 157;
        break;
    case Client::CBulb::BULB_SPOT:
        iModelNum = 158;
        break;
    case Client::CBulb::BULB_END:
        break;
    default:
        break;
    }
    const _wstring Model_Component = TEXT("Prototype_Component_Model_Environment");

    const _wstring Model_Component_Result = Model_Component + to_wstring(iModelNum + ENVIRONMENT_EA);
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBulb::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _float fFar = m_pGameInstance->Get_CameraFar();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
        return E_FAIL;

    return S_OK;
}

CBulb* CBulb::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBulb* pInstance = new CBulb(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBulb");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CBulb::Clone(void* pArg)
{
    CBulb* pInstance = new CBulb(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBulb");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBulb::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

}
