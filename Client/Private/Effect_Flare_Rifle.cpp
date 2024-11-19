#include "stdafx.h"
#include "..\Public\Effect_Flare_Rifle.h"

#include "GameInstance.h"


CEffect_Flare_Rifle::CEffect_Flare_Rifle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlendObject{ pDevice, pContext }
{
}

CEffect_Flare_Rifle::CEffect_Flare_Rifle(const CEffect_Flare_Rifle& Prototype)
	: CBlendObject{ Prototype }
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
	m_vecPosition = XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f);
	m_fScale = pDesc->fScale;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);
	return S_OK;
}

void CEffect_Flare_Rifle::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Flare_Rifle::Update(_float fTimeDelta)
{
}

void CEffect_Flare_Rifle::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
		return;
}

HRESULT CEffect_Flare_Rifle::Render()
{

	return S_OK;
}

HRESULT CEffect_Flare_Rifle::Add_Components()
{
	return S_OK;
}

HRESULT CEffect_Flare_Rifle::Bind_ShaderResources()
{
	return S_OK;
}

CEffect_Flare_Rifle* CEffect_Flare_Rifle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CEffect_Flare_Rifle::Clone(void* pArg)
{
	return nullptr;
}

void CEffect_Flare_Rifle::Free()
{
}
