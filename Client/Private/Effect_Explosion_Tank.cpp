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

	CamPos = *m_pGameInstance->Get_CamPosition();
	_vector vCamPos{};
	_vector vLookDir{};
	_vector vFinalPos{};
	_vector vRight{};
	switch (m_eType)
	{
	case Client::CEffect_Explosion_Tank::EXPLOSION_TANK:
		m_fMaxFrame.x = m_fMaxFrame.y = 7.f;
		m_iTextureNum = 0;
		vFinalPos = m_vecPosition;
		vFinalPos = XMVectorSetY(vFinalPos, XMVectorGetY(vFinalPos) + 2.f);
		break;
	case Client::CEffect_Explosion_Tank::EXPLOSION_MISSILE:
		m_fMaxFrame.x = m_fMaxFrame.y = 6.f;
		m_iTextureNum = 2;
		vFinalPos = m_vecPosition;
		break;
	case Client::CEffect_Explosion_Tank::EXPLOSION_MISSILE2:
		m_fMaxFrame.x = m_fMaxFrame.y = 3.f;
		m_iTextureNum = 3;
		vCamPos = XMVectorSet(CamPos.x, CamPos.y, CamPos.z, 1.f);
		vLookDir = vCamPos - m_vecPosition;
		vLookDir = XMVector3Normalize(vLookDir);
		m_vecPosition += vLookDir * 0.8f;
		 vRight =  m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		m_vecPosition += vRight * 1.f;
		vFinalPos = m_vecPosition;
		break;
	case Client::CEffect_Explosion_Tank::EXPLOSION_MISSILE3:
		m_fMaxFrame.x = m_fMaxFrame.y = 3.f;
		m_iTextureNum = 3;
		 vCamPos = XMVectorSet(CamPos.x , CamPos.y, CamPos.z, 1.f);
		 vLookDir = vCamPos - m_vecPosition;
		vLookDir = XMVector3Normalize(vLookDir);
		m_vecPosition += vLookDir * 1.3f;
		 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		m_vecPosition -= vRight * 0.6f;
		vFinalPos = m_vecPosition;
		break;
	case Client::CEffect_Explosion_Tank::EXPLOSION_TANK_DEAD:
		m_fMaxFrame.x = m_fMaxFrame.y = 6.f;
		m_iTextureNum = 2;
		vFinalPos = m_vecPosition;
		break;

	case Client::CEffect_Explosion_Tank::EXPLOSION_HELICOPTER_FIRE:
		m_fMaxFrame.x = 4.f;
		m_fMaxFrame.y = 9.f;
		m_iTextureNum = 7;
		vFinalPos = m_vecPosition;
		break;


		

	default:
		break;
	}
	
	m_pTransformCom->Set_Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vFinalPos);
	return S_OK;

}

void CEffect_Explosion_Tank::Priority_Update(_float fTimeDelta)
{

}

void CEffect_Explosion_Tank::Update(_float fTimeDelta)
{
	__super::Compute_Depth();
	

	if (m_fFrame.x < m_fMaxFrame.x)
	{
		m_fFrame.x += 1.f;
	}
	else
	{
		m_fFrame.y += 1.f;
		m_fFrame.x = 0.f;
	}
	if (m_fFrame.y > m_fMaxFrame.y)
	{

		m_bDead = true;
	}
	
}

void CEffect_Explosion_Tank::Late_Update(_float fTimeDelta)
{

	CamPos = *m_pGameInstance->Get_CamPosition();
	m_pTransformCom->LookAt(XMVectorSet(CamPos.x, CamPos.y, CamPos.z,1.f));
	switch (m_eType)
	{
	case Client::CEffect_Explosion_Tank::EXPLOSION_TANK:
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
			return;
		break;

	case Client::CEffect_Explosion_Tank::EXPLOSION_MISSILE:
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
			return;
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
			return;
		break;

	case Client::CEffect_Explosion_Tank::EXPLOSION_MISSILE2:
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
			return;
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
			return;
		break;

	case Client::CEffect_Explosion_Tank::EXPLOSION_MISSILE3:
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
			return;
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
			return;
		break;

	case Client::CEffect_Explosion_Tank::EXPLOSION_TANK_DEAD:
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
			return;
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
			return;
		break;

	default:
		break;
	}

}

HRESULT CEffect_Explosion_Tank::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (m_eType == EXPLOSION_TANK || m_eType == EXPLOSION_MISSILE || m_eType == EXPLOSION_TANK_DEAD)
	{
		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;
	}

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
	_float2		ImageEa = { m_fMaxFrame.x + 1.f,m_fMaxFrame.y + 1.f};
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
