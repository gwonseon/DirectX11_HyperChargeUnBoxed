#include "stdafx.h"
#include "..\Public\Energy_Cap.h"

#include "GameInstance.h"

CEnergy_Cap::CEnergy_Cap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPlayer_Build{ pDevice, pContext }
{
}

CEnergy_Cap::CEnergy_Cap(const CEnergy_Cap& Prototype)
    : CPlayer_Build{ Prototype }
{
}

HRESULT CEnergy_Cap::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CEnergy_Cap::Initialize(void* pArg)
{
    ENERGYCAP_DESC* pDesc = static_cast<ENERGYCAP_DESC*>(pArg);
    m_eLevel = pDesc->eID;
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;
   
    fPos = { pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z};
    return S_OK;
}

void CEnergy_Cap::Priority_Update(_float fTimeDelta)
{
}

void CEnergy_Cap::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_NUMPAD1))
    {
        fPos.x += fTimeDelta;
    }
    if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_NUMPAD2))
    {
        fPos.y += fTimeDelta;
    }
    if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_NUMPAD3))
    {
        fPos.z += fTimeDelta;
    }

    if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_NUMPAD4))
    {
        fPos.x -= fTimeDelta;
    }
    if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_NUMPAD5))
    {
        fPos.y -= fTimeDelta;
    }
    if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_NUMPAD6))
    {
        fPos.z -= fTimeDelta;
    }
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f));
}

void CEnergy_Cap::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
    if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_O))
    {
        // 468.957 13.1806 540.677
        cout << fPos.x << "    " << fPos.y << "     " << fPos.z << endl;
    }

}

HRESULT CEnergy_Cap::Render()
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

HRESULT CEnergy_Cap::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxItem"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    const _wstring Model_Component = TEXT("Prototype_Component_Model_Environment");
    const _wstring Model_Component_Result = Model_Component + to_wstring(208);
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEnergy_Cap::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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

CEnergy_Cap* CEnergy_Cap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEnergy_Cap* pInstance = new CEnergy_Cap(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CEnergy_Cap");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CEnergy_Cap::Clone(void* pArg)
{
    CEnergy_Cap* pInstance = new CEnergy_Cap(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CEnergy_Cap");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CEnergy_Cap::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

}
