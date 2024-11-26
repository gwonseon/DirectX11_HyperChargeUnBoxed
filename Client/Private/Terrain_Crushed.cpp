#include "stdafx.h"
#include "..\Public\Terrain_Crushed.h"

#include "GameInstance.h"


CTerrain_Crushed::CTerrain_Crushed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTerrain_Crushed::CTerrain_Crushed(const CTerrain_Crushed& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CTerrain_Crushed::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTerrain_Crushed::Initialize(void* pArg)
{
    TERRAIN_CRUSHED_DESC* pDesc = static_cast<TERRAIN_CRUSHED_DESC*>(pArg);

    m_eLevel = pDesc->eLevel;
    m_pPlayer = pDesc->pPlayer;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pModelCom->Set_Animation(0, false);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f));
    m_pTransformCom->Set_Scaling(pDesc->fScale.x, pDesc->fScale.y, pDesc->fScale.z);

    m_fAnimSpeed = 3.f;
    return S_OK;
}

void CTerrain_Crushed::Priority_Update(_float fTimeDelta)
{


}

void CTerrain_Crushed::Update(_float fTimeDelta)
{
    if (m_bDead == true)
        return;
    _bool m_bAnimState = m_pModelCom->Play_Animation(fTimeDelta * m_fAnimSpeed, false);
    if (m_bAnimState == true)
        m_bDead = true;

    if (m_fAnimSpeed >= 1.f)
        m_fAnimSpeed -= fTimeDelta * 5.f;
    else
        m_fAnimSpeed = 1.f;
}

void CTerrain_Crushed::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;

}

HRESULT CTerrain_Crushed::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;
        m_pModelCom->Render(i);
    }
    return S_OK;

}


HRESULT CTerrain_Crushed::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    /* For.Com_Model */
    const _wstring Model_Component = TEXT("Prototype_Component_Model_Anim");
    const _wstring Model_Component_Result = Model_Component + to_wstring(12);
    if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTerrain_Crushed::Bind_ShaderResources()
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

CTerrain_Crushed* CTerrain_Crushed::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTerrain_Crushed* pInstance = new CTerrain_Crushed(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTerrain_Crushed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CTerrain_Crushed::Clone(void* pArg)
{
    CTerrain_Crushed* pInstance = new CTerrain_Crushed(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CTerrain_Crushed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CTerrain_Crushed::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
