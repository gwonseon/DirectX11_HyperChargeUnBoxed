#include "stdafx.h"
#include "..\Public\Energy_Lader.h"

#include "GameInstance.h"

CEnergy_Lader::CEnergy_Lader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPlayer_Build{ pDevice, pContext }
{
}

CEnergy_Lader::CEnergy_Lader(const CEnergy_Lader& Prototype)
    : CPlayer_Build{ Prototype }
{
}

HRESULT CEnergy_Lader::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEnergy_Lader::Initialize(void* pArg)
{
    ENERGYLADER_DESC* pDesc = static_cast<ENERGYLADER_DESC*>(pArg);
    m_eLevel = pDesc->eID;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    _float4x4 matSecondPreTransform{};
    XMStoreFloat4x4(&matSecondPreTransform, XMMatrixTranslation(-0.05f, 0.f, 0.f));
    m_pModelCom->Set_SecondPreTransform(matSecondPreTransform);

    return S_OK;
}

void CEnergy_Lader::Priority_Update(_float fTimeDelta)
{
    m_pTransformCom->Turn(0.f, 1.f, 0.f, fTimeDelta);
}

void CEnergy_Lader::Update(_float fTimeDelta)
{
}

void CEnergy_Lader::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
}

HRESULT CEnergy_Lader::Render()
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

HRESULT CEnergy_Lader::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxItem"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    const _wstring Model_Component = TEXT("Prototype_Component_Model_Environment");
    const _wstring Model_Component_Result = Model_Component + to_wstring(210);
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEnergy_Lader::Bind_ShaderResources()
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


    return S_OK;
}

CEnergy_Lader* CEnergy_Lader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEnergy_Lader* pInstance = new CEnergy_Lader(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CEnergy_Lader");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CEnergy_Lader::Clone(void* pArg)
{
    CEnergy_Lader* pInstance = new CEnergy_Lader(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CEnergy_Lader");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CEnergy_Lader::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}