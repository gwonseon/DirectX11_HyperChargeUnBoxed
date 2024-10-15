#include "stdafx.h"
#include "..\Public\Player_Build.h"

#include "GameInstance.h"

CPlayer_Build::CPlayer_Build(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPlayer_Build::CPlayer_Build(const CPlayer_Build& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CPlayer_Build::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Build::Initialize(void* pArg)
{
	PLAYER_BUILD_DESC* pDesc = static_cast<PLAYER_BUILD_DESC*>(pArg);
	

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f));
	m_pTransformCom->Set_Scaling(pDesc->fScale.x, pDesc->fScale.y, pDesc->fScale.z);
	return S_OK;
}

void CPlayer_Build::Priority_Update(_float fTimeDelta)
{
}

void CPlayer_Build::Update(_float fTimeDelta)
{
}

void CPlayer_Build::Late_Update(_float fTimeDelta)
{
}

HRESULT CPlayer_Build::Render()
{
	return S_OK;
}

void CPlayer_Build::Free()
{
	__super::Free();
}
