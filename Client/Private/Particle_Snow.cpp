#include "stdafx.h"
#include "..\Public\Particle_Snow.h"

#include "GameInstance.h"


CParticle_Snow::CParticle_Snow(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
	: CGameObject{pDevice,pContext}
{}

CParticle_Snow::CParticle_Snow(const CParticle_Snow& Prototype)
	: CGameObject{Prototype}
{}

HRESULT CParticle_Snow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Snow::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if(FAILED(Add_Components()))
		return E_FAIL;


	return S_OK;
}

void CParticle_Snow::Priority_Update(_float fTimeDelta)
{}

void CParticle_Snow::Update(_float fTimeDelta)
{
	// m_pVIBufferCom->Drop(fTimeDelta);
}

void CParticle_Snow::Late_Update(_float fTimeDelta)
{
	if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND,this)))
		return;
}

HRESULT CParticle_Snow::Render()
{
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;
	if(FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if(FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CParticle_Snow::Add_Components()
{
	/* 멤버변수로 직접 참조를 하게되면 */
	  /* 1. 내가 내 컴포넌트를 이용하고자할 때 굳이 검색이 필요없이 특정 멤버변수로 바로 기능을 이용하면 된다. */
	  /* 2. 다른 객체가 내 컴포넌트를 검색하고자 할때 스위치케이스가 겁나 늘어나는 상황. */

	/* For.Com_Texture */
	if(FAILED(__super::Add_Component(LEVEL_YARD,TEXT("Prototype_Component_Texture_Snow"),
		TEXT("Com_Texture"),reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if(FAILED(__super::Add_Component(LEVEL_YARD,TEXT("Prototype_Component_Shader_VtxParticlePoint"),
		TEXT("Com_Shader"),reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if(FAILED(__super::Add_Component(LEVEL_YARD,TEXT("Prototype_Component_VIBuffer_Particle_Snow"),
		TEXT("Com_VIBuffer"),reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Snow::Bind_ShaderResources()
{
	if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom,"g_WorldMatrix")))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if(FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom,"g_Texture",0)))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//    return E_FAIL;

	return S_OK;
}

CParticle_Snow* CParticle_Snow::Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
{
	CParticle_Snow* pInstance = new CParticle_Snow(pDevice,pContext);

	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CParticle_Snow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticle_Snow::Clone(void* pArg)
{
	CParticle_Snow* pInstance = new CParticle_Snow(*this);

	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CParticle_Snow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Snow::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}