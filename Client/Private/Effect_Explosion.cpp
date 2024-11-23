#include "stdafx.h"
#include "..\Public\Effect_Explosion.h"

#include "GameInstance.h"

CEffect_Explosion::CEffect_Explosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CEffect_Explosion::CEffect_Explosion(const CEffect_Explosion& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Explosion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Explosion::Initialize(void* pArg)
{
	EFFECT_EXPLOSION_DESC* pDesc = static_cast<EFFECT_EXPLOSION_DESC*>(pArg);
	pDesc->fSpeedPerSec = 5.f;
	pDesc->fRotationPerSec = XMConvertToRadians(90.0f);
	m_pCamera = pDesc->pCamera;
	if (FAILED(__super::Initialize(&pDesc)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;
	m_pTransformCom->Set_Scaling(10.f, 10.f, 10.f );
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(
		m_pGameInstance->Compute_Random(450, 500), 10.f, m_pGameInstance->Compute_Random(550, 600.f), 1.f));

	return S_OK;
}

void CEffect_Explosion::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
	_vector vCam = *m_pCamera->Get_Camera_Pos();
	m_pTransformCom->LookAt(vCam);

}

void CEffect_Explosion::Update(_float fTimeDelta)
{
	__super::Compute_Depth();

	m_fFrame += 90.f * fTimeDelta;

	if (m_fFrame >= 90.f)
		m_fFrame = 0.f;
}

void CEffect_Explosion::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
		return;
}

HRESULT CEffect_Explosion::Render()
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

HRESULT CEffect_Explosion::Add_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_YARD, TEXT("Prototype_Component_Texture_Explosion"),
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

HRESULT CEffect_Explosion::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", static_cast<_uint>(m_fFrame))))
		return E_FAIL;
	_float fFar=	m_pGameInstance->Get_CameraFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShaderCom, "g_DepthTexture", TEXT("Target_Depth"))))
		return E_FAIL;
	return S_OK;
}

CEffect_Explosion* CEffect_Explosion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Explosion* pInstance = new CEffect_Explosion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Explosion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Explosion::Clone(void* pArg)
{
	CEffect_Explosion* pInstance = new CEffect_Explosion(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CEffect_Explosion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Explosion::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom); 
}
