#include "stdafx.h"
#include "..\Public\Effect_Explosion_Tank.h"

#include "GameInstance.h"

CEffect_Explosion_Tank::CEffect_Explosion_Tank(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CEffect_Explosion_Tank::CEffect_Explosion_Tank(const CEffect_Explosion_Tank& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Explosion_Tank::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Explosion_Tank::Initialize(void* pArg)
{
	EFFECT_Tank_Explosion_DESC* pDesc = static_cast<EFFECT_Tank_Explosion_DESC*>(pArg);
	m_eLevel = pDesc->eLevel;
	pDesc->fSpeedPerSec = 5.f;
	pDesc->fRotationPerSec = XMConvertToRadians(90.0f);
	m_eType = pDesc->eType;
	m_fScale = pDesc->fScale;
	m_vecPosition = XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f);
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
	m_vecPosition = XMVectorSetY(m_vecPosition, XMVectorGetY(m_vecPosition) + 2.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);

	switch (m_eType)
	{
	case Client::CEffect_Explosion_Tank::EXPLOSION_TANK:
		m_fMaxFrame = 7.f;
		m_iTextureNum = 0;
		break;
	case Client::CEffect_Explosion_Tank::EXPLOSION_MISSILE:
		m_fMaxFrame = 6.f;
		m_iTextureNum = 2;
		break;
	case Client::CEffect_Explosion_Tank::EXPLOSION_END:
		break;
	default:
		break;
	}
	

	return S_OK;

}

void CEffect_Explosion_Tank::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CEffect_Explosion_Tank::Update(_float fTimeDelta)
{
	__super::Compute_Depth();

	if (m_fFrame.x < m_fMaxFrame)
	{
		m_fFrame.x += 1.f;
	}
	else
	{
		m_fFrame.y += 1.f;
		m_fFrame.x = 0.f;
	}
	if (m_fFrame.y > m_fMaxFrame)
	{
		m_bDead = true;
	}

}

void CEffect_Explosion_Tank::Late_Update(_float fTimeDelta)
{

	_float4 CamPos = *m_pGameInstance->Get_CamPosition();
	m_pTransformCom->LookAt(XMVectorSet(CamPos.x, CamPos.y, CamPos.z,1.f));
	switch (m_eType)
	{
	case Client::CEffect_Explosion_Tank::EXPLOSION_TANK:
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
			return;
		break;
	case Client::CEffect_Explosion_Tank::EXPLOSION_MISSILE:
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
			return;
		break;
	case Client::CEffect_Explosion_Tank::EXPLOSION_END:
		break;
	default:
		break;
	}

}

HRESULT CEffect_Explosion_Tank::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Explosion_Tank::Add_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_Tank_Explosion"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Explosion_Tank::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", static_cast<_uint>(m_iTextureNum))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShaderCom, "g_DepthTexture", TEXT("Target_Depth"))))
		return E_FAIL;

	_float fFar = m_pGameInstance->Get_CameraFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Index", &m_fFrame, sizeof(_float2))))
		return E_FAIL;
	_float2		ImageEa = { m_fMaxFrame + 1.f,m_fMaxFrame + 1.f};
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ImageEA", &ImageEa, sizeof(_float2))))
		return E_FAIL;




	return S_OK;
}

CEffect_Explosion_Tank* CEffect_Explosion_Tank::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Explosion_Tank* pInstance = new CEffect_Explosion_Tank(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Explosion_Tank");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Explosion_Tank::Clone(void* pArg)
{
	CEffect_Explosion_Tank* pInstance = new CEffect_Explosion_Tank(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CEffect_Explosion_Tank");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Explosion_Tank::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

}
