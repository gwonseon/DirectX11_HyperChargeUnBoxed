#include "stdafx.h"
#include "Player2.h"
#include "GameInstance.h"
#include "Player_Define.h"
#include "PartObject.h"
#include "Player2_Body.h"

namespace {
	constexpr _float kMouseSens = 0.1f; // 감도
	constexpr _float kMaxHp = 100.f;
	constexpr _float kAttack = 20.f;
}

CPlayer2::CPlayer2(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
	: CContainerObject{pDevice,pContext}
{}

CPlayer2::CPlayer2(const CPlayer2 & Prototype)
	: CContainerObject{Prototype}
{}

HRESULT CPlayer2::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer2::Initialize(void * pArg)
{
	CContainerObject::CONTAINEROBJECT_DESC		Desc{};
	Desc.iNumPartObjects = PART_END;
	Desc.fSpeedPerSec = 25.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.f);

	PLAYER_DESC* pPlayer = static_cast<PLAYER_DESC*>(pArg);
	m_eLevelID = pPlayer->m_eLevelID;
	m_iCellidx = pPlayer->iCellIdx;

	m_vecCameraAt = pPlayer->vCameraAt;
	m_vecCameraPos = pPlayer->vCameraPos;
	m_iRound = pPlayer->iRound;

	if(FAILED(__super::Initialize(&Desc)))
		return E_FAIL;
	if(FAILED(Add_Components()))
		return E_FAIL;
	if(FAILED(Add_PartObjects()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,XMVectorSet(pPlayer->fPosition.x,pPlayer->fPosition.y,pPlayer->fPosition.z,1.f));
	m_pTransformCom->Set_Scaling(1.5f,1.5f,1.5f);


	m_fHp = kMaxHp;
	m_fEnergy = kMaxHp;
	m_fAttack = kAttack;
	m_iCoin = 0;
	m_bDontDestroy = true;	 // 파괴되지 않는다.
	m_bAffected = true; // 폭발의 영향을 받겠다.
	
	m_pState = PLAYER_STATE_DESC::Idle();
	m_pState->Enter(this);

    return S_OK;
}

void CPlayer2::Priority_Update(_float fTimeDelta)
{
	Auto_Heal(fTimeDelta);
	Build_Update(fTimeDelta);


	__super::Priority_Update(fTimeDelta);
}

void CPlayer2::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	if(m_pState) m_pState->Update(this, fTimeDelta); // 상태 패턴 업데이트

}

void CPlayer2::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	#ifdef _DEBUG 
	if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND,this)))
		return;
	#endif
}
HRESULT CPlayer2::Render()
{
	m_pNavigationCom->Render();
    return S_OK;
}
HRESULT CPlayer2::Add_Components()
{
	CNavigation::NAVIGATION_DESC		Desc{};
	Desc.iCurrentCellIndex = m_iCellidx;
	switch(m_eLevelID)
	{
	case Client::LEVEL_GAMEPLAY:
	{
		if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Navigation"),
			TEXT("Com_Playerigation"),reinterpret_cast<CComponent**>(&m_pNavigationCom),&Desc)))
			return E_FAIL;
		break;
	}
	case Client::LEVEL_YARD:
	{
		if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Navigation_Yard"),
			TEXT("Com_Playerigation"),reinterpret_cast<CComponent**>(&m_pNavigationCom),&Desc)))
			return E_FAIL;
		break;
	}
	default:
	break;
	}
	return S_OK;
}
HRESULT CPlayer2::Add_PartObjects()
{
	CPlayer2_Body::PLAYER2_BODY_DESC BodyDesc{};
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyDesc.fSpeedPerSec = 0.f;
	BodyDesc.fRotationPerSec = 0.f;
	BodyDesc.m_iViewState = &m_iViewState;
	BodyDesc.m_bAttackState = &m_bAttackState;
	BodyDesc.eLevelID = m_eLevelID;
	if(FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Player2_BODY"),PART_PLAYER_BODY,&BodyDesc)))
		return E_FAIL;
	

    return S_OK;
}

void CPlayer2::ChangeState(unique_ptr<CPlayer_State> nextState)
{
	if(!nextState) return;
	if(m_pState) m_pState->Exit(this);
	m_pState = move(nextState);
	m_pState->Enter(this);
}

void CPlayer2::Auto_Heal(_float fTimeDelta)
{
	if(m_fHpTimer >= 1.f)
	{
		// 1초마다 피가 꽉찼는데 쉴드에너지가 만땅 아니면 쉴드 에너지 충전
		m_fHpTimer = 0.f;
		if(m_fHp == kMaxHp)
			if(m_fEnergy < kMaxHp)
				m_fEnergy += 1.f;
		else
			m_fHp += 1.f;	// 피가 부족하면 피 충전
	}
	m_fHpTimer += fTimeDelta;
}
void CPlayer2::Build_Update(_float fTimeDelta)
{
	if(m_bBuildMode == true && m_bBuild_Able == true)
	{
		if(m_pGameInstance->Get_DIKeyState_Pressing(DIK_E))
			m_bBuild_Gauging = true;
		else
			m_bBuild_Gauging = false;
	} else
		m_bBuild_Gauging = false;

	// 빌드모드 건너뛰기
	if(m_pGameInstance->Get_DIKeyState_Down(DIK_F) && *m_bRoundStart == false)
	{
		if(m_bBuildMode == false)
			m_bBuildMode = true;
		else
			m_bBuildMode = false;
	}
}
CPlayer2 * CPlayer2::Create(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
{
	CPlayer2* pInstance = new CPlayer2(pDevice,pContext);
	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer2");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CGameObject * CPlayer2::Clone(void * pArg)
{
	CPlayer2* pInstance = new CPlayer2(*this);
	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer2");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CPlayer2::Free()
{
	__super::Free();
	Safe_Release(m_pNavigationCom);
}
