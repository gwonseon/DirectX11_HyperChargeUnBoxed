#include "stdafx.h"
#include "..\Public\Monster.h"

#include "GameInstance.h"


CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMonster::CMonster(const CMonster& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}


HRESULT CMonster::Initialize(void* pArg)
{
	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f));
	m_pTransformCom->Set_Scaling(pDesc->fScale.x, pDesc->fScale.y, pDesc->fScale.z);
	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{

	if (m_fHp <= 0.f)
	{
		m_bDead = true;
	}

}

void CMonster::Update(_float fTimeDelta)
{


}

void CMonster::Late_Update(_float fTimeDelta)
{
	if (m_bDead == false)
	{
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
			return;
	}
}

HRESULT CMonster::Render()
{
	return S_OK;
}



void CMonster::Free()
{
	__super::Free();


}
