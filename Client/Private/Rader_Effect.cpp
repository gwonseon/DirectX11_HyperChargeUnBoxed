#include "stdafx.h"
#include "..\Public\Rader_Effect.h"

#include "GameInstance.h"

CRader_Effect::CRader_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPlayer_Build{ pDevice, pContext }
{
}

CRader_Effect::CRader_Effect(const CRader_Effect& Prototype)
    : CPlayer_Build{ Prototype }
{
}

HRESULT CRader_Effect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRader_Effect::Initialize(void* pArg)
{
    RADER_DESC* pDesc = static_cast<RADER_DESC*>(pArg);
    m_eLevel = pDesc->eID;


    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;
    if (FAILED(Add_Components()))
        return E_FAIL;


    return S_OK;
}

void CRader_Effect::Priority_Update(_float fTimeDelta)
{
    if(m_bWork == true)
    {
        m_pTransformCom->Turn(0.f, 1.f, 0.f, fTimeDelta);
        m_fUV += fTimeDelta * 2.f;
    }
}

void CRader_Effect::Update(_float fTimeDelta)
{
}

void CRader_Effect::Late_Update(_float fTimeDelta)
{
   if (m_bWork == true)
   {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
               return;
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
            return;
    }
}

HRESULT CRader_Effect::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(10)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CRader_Effect::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    const _wstring Model_Component = TEXT("Prototype_Component_Model_Effect");
    const _wstring Model_Component_Result = Model_Component + to_wstring(2);
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;


    return S_OK;
}

HRESULT CRader_Effect::Bind_ShaderResources()
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
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fTex_Move", &m_fUV, sizeof(float))))
        return E_FAIL;
    
    return S_OK;
}

CRader_Effect* CRader_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRader_Effect* pInstance = new CRader_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CRader_Effect");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CRader_Effect::Clone(void* pArg)
{
    CRader_Effect* pInstance = new CRader_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CRader_Effect");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CRader_Effect::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
