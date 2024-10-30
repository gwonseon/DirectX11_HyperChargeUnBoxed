#include "stdafx.h"
#include "..\Public\Broken_Bricks.h"

#include "GameInstance.h"

CBroken_Bricks::CBroken_Bricks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPlayer_Build{ pDevice, pContext }
{
}

CBroken_Bricks::CBroken_Bricks(const CBroken_Bricks& Prototype)
    : CPlayer_Build{ Prototype }
{
}

HRESULT CBroken_Bricks::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBroken_Bricks::Initialize(void* pArg)
{
    PLAYER_BUILD_DESC* pDesc = static_cast<PLAYER_BUILD_DESC*>(pArg);
    m_eLevel= pDesc->eID;
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CBroken_Bricks::Priority_Update(_float fTimeDelta)
{

    _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    if (m_fDestroy_Pos.y >= 5.f)
        m_bDestroy_Pos_Mgr = true;
    if (m_fDestroy_Pos.y > 0.f)
    {
        if (m_bDestroy_Pos_Mgr == false)
            m_fDestroy_Pos.y += fTimeDelta * 6.f;
        else
            m_fDestroy_Pos.y -= fTimeDelta * 6.f;
    }
    else
        m_fDestroy_Pos.y = 0;
    vPos=   XMVectorSetY(vPos, m_fDestroy_Pos.y);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

}

void CBroken_Bricks::Update(_float fTimeDelta)
{
    m_fDelete_Time += fTimeDelta;
    if (m_fDelete_Time > 40.f)
    {
        m_bDead = true;
    }
}

void CBroken_Bricks::Late_Update(_float fTimeDelta)
{
    if(m_bDead == false)
    {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
            return;
    }
}

HRESULT CBroken_Bricks::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;
    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
            return E_FAIL;
  
        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;
        

        m_pModelCom->Render(i);
    }
    return S_OK;
}

HRESULT CBroken_Bricks::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTrap"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    const _wstring Model_Component = TEXT("Prototype_Component_Model_Trap");
    const _wstring Model_Component_Result = Model_Component + to_wstring(12);
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBroken_Bricks::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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

CBroken_Bricks* CBroken_Bricks::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBroken_Bricks* pInstance = new CBroken_Bricks(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBroken_Bricks");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CBroken_Bricks::Clone(void* pArg)
{
    CBroken_Bricks* pInstance = new CBroken_Bricks(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBroken_Bricks");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBroken_Bricks::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
