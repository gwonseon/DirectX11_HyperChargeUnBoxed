#include "stdafx.h"
#include "..\Public\Effect_Flare_Rifle.h"

#include "GameInstance.h"


CEffect_Flare_Rifle::CEffect_Flare_Rifle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CEffect_Flare_Rifle::CEffect_Flare_Rifle(const CEffect_Flare_Rifle& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Flare_Rifle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Flare_Rifle::Initialize(void* pArg)
{
	EFFECT_RIFLE_FLARE_DESC* pDesc = static_cast<EFFECT_RIFLE_FLARE_DESC*>(pArg);
	pDesc->fSpeedPerSec = 5.f;
	pDesc->fRotationPerSec = XMConvertToRadians(90.0f);
	m_pCamera = pDesc->pCamera;
	m_vecCamPos = pDesc->vecCamPos;
	m_vecWeaponPos = pDesc->vecWeaponPos;
	m_eLevel = pDesc->eLevel;
	m_fScale = pDesc->fScale;
	m_eType = pDesc->eType;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	switch (m_eType)
	{
	case Client::CEffect_Flare_Rifle::FLARE_PLAYER:
		break;
	case Client::CEffect_Flare_Rifle::FLARE_RIFLEMAN:
		break;
	case Client::CEffect_Flare_Rifle::FLARE_HELICOPTER:
		break;
	case Client::CEffect_Flare_Rifle::FLARE_TANK:
		m_vecTargetPos = pDesc->vecTargetPos;
		break;
	default:
		break;
	}
	m_pTransformCom->Set_Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *m_vecWeaponPos);
	return S_OK;
}

void CEffect_Flare_Rifle::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CEffect_Flare_Rifle::Update(_float fTimeDelta)
{
	__super::Compute_Depth();
	m_fLifeTime += fTimeDelta;
	if (m_fLifeTime >= 0.1f)
		m_bDead = true;
	
	if(m_fFrame.x < 6)
	{
		m_fFrame.x += 1;
	}
	else
	{
		m_fFrame.y += 1;
		m_fFrame.x = 0;
	}
	if (m_fFrame.y > 6)
	{
		m_bDead = true;
	}


}

void CEffect_Flare_Rifle::Late_Update(_float fTimeDelta)
{
	if (m_pCamera != nullptr)
	{
		_vector vCam = *m_pCamera->Get_Camera_Pos();
		m_pTransformCom->LookAt(vCam);
	}
	else if(m_eType == FLARE_PLAYER)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, *m_vecWeaponPos);
		m_pTransformCom->LookAt(*m_vecCamPos);
	}
	else if (m_eType == FLARE_TANK)
	{
		_vector vPos = *m_vecWeaponPos;
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 5.f);
		_vector vDir = *m_vecTargetPos - vPos;
		vPos += XMVector3Normalize(vDir) * 10.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_pTransformCom->LookAt(*m_vecCamPos);
	}
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
		return;
}

HRESULT CEffect_Flare_Rifle::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Flare_Rifle::Add_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_Flare_DDS"),
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

HRESULT CEffect_Flare_Rifle::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", static_cast<_uint>(0))))
		return E_FAIL;
	_float fFar = m_pGameInstance->Get_CameraFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Index", &m_fFrame, sizeof(_float2))))
		return E_FAIL;
	_float2		ImageEa = { 5.f,5.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ImageEA", &ImageEa, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShaderCom, "g_DepthTexture", TEXT("Target_Depth"))))
		return E_FAIL;


	return S_OK;
}

CEffect_Flare_Rifle* CEffect_Flare_Rifle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Flare_Rifle* pInstance = new CEffect_Flare_Rifle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Flare_Rifle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Flare_Rifle::Clone(void* pArg)
{
	CEffect_Flare_Rifle* pInstance = new CEffect_Flare_Rifle(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CEffect_Flare_Rifle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Flare_Rifle::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
