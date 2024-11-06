#include "stdafx.h"
#include "..\Public\Particle_Explosion.h"

#include "GameInstance.h"

CParticle_Explosion::CParticle_Explosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CParticle_Explosion::CParticle_Explosion(const CParticle_Explosion& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CParticle_Explosion::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticle_Explosion::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;
    return S_OK;
}

void CParticle_Explosion::Priority_Update(_float fTimeDelta)
{
}

void CParticle_Explosion::Update(_float fTimeDelta)
{
    m_pVIBufferCom->Spread(fTimeDelta);
}

void CParticle_Explosion::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
}

HRESULT CParticle_Explosion::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Explosion::Add_Components()
{
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_YARD, TEXT("Prototype_Component_Texture_Snow"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_YARD, TEXT("Prototype_Component_Shader_VtxParticleRect"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_YARD, TEXT("Prototype_Component_VIBuffer_Particle_Explosion"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Explosion::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    return S_OK;
}

CParticle_Explosion* CParticle_Explosion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_Explosion* pInstance = new CParticle_Explosion(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CParticle_Explosion");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Explosion::Clone(void* pArg)
{
    CParticle_Explosion* pInstance = new CParticle_Explosion(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CParticle_Explosion");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Explosion::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
