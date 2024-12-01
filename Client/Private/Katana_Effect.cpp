#include "stdafx.h"
#include "..\Public\Katana_Effect.h"

#include "GameInstance.h"

CKatana_Effect::CKatana_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CKatana_Effect::CKatana_Effect(const CKatana_Effect& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CKatana_Effect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CKatana_Effect::Initialize(void* pArg)
{
    EFFECT_KATANA_DESC* pDesc = static_cast<EFFECT_KATANA_DESC*>(pArg);
    m_eLevel = pDesc->eLevel;
    m_iEffectNumber = pDesc->iEffectNumber;
    m_bKatanaState = pDesc->bKatanaState;
    m_pParentState = pDesc->pParentState;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;



    return S_OK;
}

void CKatana_Effect::Priority_Update(_float fTimeDelta)
{
}

void CKatana_Effect::Update(_float fTimeDelta)
{
}

void CKatana_Effect::Late_Update(_float fTimeDelta)
{
    if (*m_pParentState & 0x00010000 || *m_pParentState & 0x00020000)
    {
        m_fBlend_Value -= fTimeDelta * 1.5f;
        if (*m_bKatanaState == true)
        {
            if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
                return;
            if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
                return;
        }
    }
}

HRESULT CKatana_Effect::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(11)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }
    

    return S_OK;
}

HRESULT CKatana_Effect::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Model_Weapon8"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CKatana_Effect::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fBlend_Value, sizeof(float))))
        return E_FAIL;
    _float fFar = m_pGameInstance->Get_CameraFar();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
        return E_FAIL;
}

CKatana_Effect* CKatana_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CKatana_Effect* pInstance = new CKatana_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CKatana_Effect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CKatana_Effect::Clone(void* pArg)
{
    CKatana_Effect* pInstance = new CKatana_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CKatana_Effect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKatana_Effect::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
