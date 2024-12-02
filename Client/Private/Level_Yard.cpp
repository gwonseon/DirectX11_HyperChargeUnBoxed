#include "stdafx.h"
#include "..\Public\Level_Yard.h"
#include "GameInstance.h"

#include "Level_Loading.h"
#include "Yard_Round.h"

#include "CrossLine.h"
#include "NumberUI.h"


#include "Environment.h"
#include "Weapon.h"
#include "Coin.h"


#include "Trap_Bricks.h"
#include "Terrain.h"
#include "Sky.h"

#include "Monster.h"
#include "Tank.h"
#include "Helicopter.h"
#include "Alien.h"
#include "Pony.h"
#include <Coin_Item.h>
#include <Hp_Item.h>
#include <UI_3D.h>
#include <Collector.h>
#include "Effect_Explosion.h"
#include <Grass_Instancing.h>


CLevel_Yard::CLevel_Yard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Yard::Initialize()
{

	ShowCursor(false);
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_PlayerBuild(TEXT("Layer_PlayerBuild"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Trap(TEXT("Layer_Trap"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MissileTruck(TEXT("Layer_MissileTruck"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_WeaponITem(TEXT("Layer_WeaponItem"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_ITem(TEXT("Layer_Item"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Icon(TEXT("Layer_UI_Icon"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	// 가장 마지막에 그려야한다.
	if (FAILED(Ready_Layer_Damaged(TEXT("Layer_UI_Damaged"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	m_pGameInstance->Set_Reset();
	Load_Map();
	pTrap = m_pGameInstance->Find_Layer(LEVEL_YARD, TEXT("Layer_Trap"));

#pragma region 라운드
	// 라운드( 각 라운드 마다 데이터가 다르기 때문에 각각 따로 라운드를 생성해서 관리해줌)
	m_pRound[0] = CYard_Round::Create(m_pDevice, m_pContext, 1);
	m_pRound[1] = CYard_Round::Create(m_pDevice, m_pContext, 2);
	m_pRound[2] = CYard_Round::Create(m_pDevice, m_pContext, 3);
	for (int i = 0; i < 3; i++)
	{
		m_pRound[i]->Set_Player(m_pPlayer);
		m_pRound[i]->Set_TrapLayer(pTrap);
		m_pRound[i]->Set_BrainPos(m_pBrain->Get_BrainPos());
		m_pRound[i]->Set_Player_BrainCore(m_pBrain);
		m_pRound[i]->Set_BrainCoreWorld_matrix(m_pBrain->Get_Transform()->Get_WorldMatrixPtr());
		m_pRound[i]->Set_PlayerWorld_matrix(m_pPlayer->Get_Transform()->Get_WorldMatrixPtr());
		m_pRound[i]->Set_Camera(m_pCamera);
	}
#pragma endregion 라운드

	m_pPlayer->Set_RoundStart(&m_bRoundStart);
	m_pReloading = m_pPlayer->Get_Reloading();
	pPlayerLayer = m_pGameInstance->Find_Layer(LEVEL_YARD, TEXT("Layer_Player"));
	pCoin = m_pGameInstance->Find_Layer(LEVEL_YARD, TEXT("Layer_Coin"));
	pCircleUI = m_pGameInstance->Find_Layer(LEVEL_YARD, TEXT("Layer_CircleUI"));
	pItem = m_pGameInstance->Find_Layer(LEVEL_YARD, TEXT("Layer_Item"));
	m_pGameInstance->Set_CurrentLevel(LEVEL_YARD);


	m_pGameInstance->StopAll();
	m_pGameInstance->PlayBGM(L"YardBackGround.wav", 0.1f);


	
    return S_OK;
}

void CLevel_Yard::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	Text_Update(fTimeDelta);
	Build_Check(); // 트랩 설치관련 
	Interaction();
	RoundMgr_And_MonsterSpawn(fTimeDelta);

#pragma region Collision
	if (pTrap_Shield == nullptr)
		pTrap_Shield = m_pGameInstance->Find_Layer(LEVEL_YARD, TEXT("Layer_Trap_Shield"));
	if (pBuild == nullptr)
		pBuild = m_pGameInstance->Find_Layer(LEVEL_YARD, TEXT("Layer_PlayerBuild"));
	if (pExplosion == nullptr)
		pExplosion = m_pGameInstance->Find_Layer(LEVEL_YARD, TEXT("Layer_Explosion"));
	if(pExplosion_Player == nullptr)
		pExplosion_Player = m_pGameInstance->Find_Layer(LEVEL_YARD, TEXT("Layer_Explosion_Player"));
	// 앞이 당하는 애
	m_pGameInstance->Collision_Layer(pPlayerLayer, pNearMonsterLayer, TEXT("Com_Collider_AABB"), TEXT("Com_Collider_Sphere"), CPlayer::TPS_PART_BODY);		 // 근접 공격 몬스터랑 플레이어
	m_pGameInstance->Collision_Layer(pNearMonsterLayer, pPlayerLayer, TEXT("Com_Collider_Sphere"), TEXT("Com_Collider_Sphere"), 0, CPlayer::TPS_PART_KATANA); // 칼이랑 몬스터
	m_pGameInstance->Collision_Layer(pFarMonsterLayer, pPlayerLayer, TEXT("Com_Collider_Sphere"), TEXT("Com_Collider_Sphere"), 0, CPlayer::TPS_PART_KATANA); // 칼이랑 몬스터
	m_pGameInstance->Collision_Layer_Coin(pCoin, pPlayerLayer, TEXT("Com_Collider_Sphere"), TEXT("Com_Collider_AABB"), 0, CPlayer::TPS_PART_BODY);
	m_pGameInstance->Collision_Trap(pTrap_Shield, pMonsterBullet, TEXT("Com_Collider_AABB"), TEXT("Com_Collider_Sphere"));
	m_pGameInstance->Collision_Trap(pTrap_Shield, pNearMonsterLayer, TEXT("Com_Collider_AABB"), TEXT("Com_Collider_Sphere"));
	m_pGameInstance->Collision_Explosion(pExplosion, pBuild, TEXT("Com_Collider_Sphere"), TEXT("Com_Collider_AABB"), 4);
	m_pGameInstance->Collision_Explosion(pExplosion, pTrap_Shield, TEXT("Com_Collider_Sphere"), TEXT("Com_Collider_AABB"), 4);
	m_pGameInstance->Collision_Explosion(pExplosion, pPlayerLayer, TEXT("Com_Collider_Sphere"), TEXT("Com_Collider_AABB"), 4, 0, CPlayer::TPS_PART_BODY);
	

	// 밀어내기
	// m_pGameInstance->Anti_OverLapping(pFarMonsterLayer, pNearMonsterLayer, TEXT("Com_Collider_Sphere"), TEXT("Com_Collider_Sphere"), 0, 0);
	m_pGameInstance->Anti_OverLapping(pNearMonsterLayer, pFarMonsterLayer, TEXT("Com_Collider_Sphere"), TEXT("Com_Collider_Sphere"), 0, 0);
	m_pGameInstance->Anti_OverLapping_SameLayer(pNearMonsterLayer, TEXT("Com_Collider_Sphere"), 0);
	m_pGameInstance->Anti_OverLapping_SameLayer(pFarMonsterLayer, TEXT("Com_Collider_Sphere"), 0);

#pragma endregion Collision	
#pragma region 총알충돌검사
	_uint WeaponState = *m_pPlayer->Get_WeaponState();
	if (WeaponState == CPlayer::WEAPON_LOCKETLAUNCHER)
	{
		pExplosion_Player; // 폭발과 충돌확인, 포탄 충돌시 폭발로 변경
	}
	else
	{
		_float3 fMousePos = m_pGameInstance->Get_MousePos_NDC(g_hWnd, g_iWinSizeX, g_iWinSizeY);
		XMMATRIX invProj = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_PROJ);
		XMMATRIX invView = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW);
		XMVECTOR RayPos, RayDir;
		m_pGameInstance->Get_MouseRayDirection(fMousePos, invProj, invView, &RayPos, &RayDir);
		RayDir = XMVector3Normalize(RayDir);

		// 레이 값이 쓰레기 값인 경우 검사 패스~
		if (!XMVector3IsInfinite(RayPos) && !XMVector3IsNaN(RayPos) &&
			!XMVector3IsInfinite(RayDir) && !XMVector3IsNaN(RayDir))
		{
				_bool* bShot = m_pPlayer->Get_ShotStart();
				
				m_pGameInstance->Collision_Bullet(pNearMonsterLayer, TEXT("Com_Collider_Sphere"), RayDir, RayPos, bShot, m_pPlayer->Get_Attack()); // 총과 근거리 몬스터
				m_pGameInstance->Collision_Bullet(pFarMonsterLayer, TEXT("Com_Collider_Sphere"), RayDir, RayPos, bShot, m_pPlayer->Get_Attack());  // 총과 장거리 몬스터
		}
	}
#pragma endregion 총알충돌검사


}

HRESULT CLevel_Yard::Render()
{
	__super::Render();
	Text_Render();
	
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Yard레벨입니다."));
#endif
	if (m_pGameInstance->Get_DIKeyState_Down(DIK_ESCAPE))
	{
		m_pGameInstance->Free_Light();
		(m_pGameInstance->Open_Level(LEVEL_YARD, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOGO)));
		ShowCursor(true);
	}
	return S_OK;
}

void CLevel_Yard::Interaction()
{
	m_pGameInstance->CircleGauge_Interaction( pItem, pCircleUI);

	_vector vPlayerPos = m_pPlayer->Get_Position();
	_float3 fPlayerPos{}, fWeaponPos{};
	XMStoreFloat3(&fPlayerPos, vPlayerPos);
	for (int i = 0; i < 2; i++)
	{
		_vector vWeaponPos = m_pWeaponItem[i]->Get_Position();
		XMStoreFloat3(&fWeaponPos, vWeaponPos);
		if (((fPlayerPos.x - fWeaponPos.x) * (fPlayerPos.x - fWeaponPos.x) + (fPlayerPos.y - fWeaponPos.y) * (fPlayerPos.y - fWeaponPos.y) + (fPlayerPos.z - fWeaponPos.z) * (fPlayerPos.z - fWeaponPos.z)) <= 80.f)
		{
			m_pWeaponItem[i]->Set_Interation(true);
			if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_E))
				m_pWeaponItem[i]->Set_Charging(true); // 아이템에서 차징중임을 알려줌
			else
				m_pWeaponItem[i]->Set_Charging(false);

			// 플레이어에게 장착된 장비가 무엇인지 알려줌
			_bool bEquip{};
			_uint iEuquipNum{};
			m_pWeaponItem[i]->Set_WeaponItem_Equip(bEquip, iEuquipNum);
			if (bEquip == true)
				m_pPlayer->Set_EquipNumber(iEuquipNum);
		}
		else
		{
			m_pWeaponItem[i]->Set_Charging(false);
			m_pWeaponItem[i]->Set_Interation(false);
		}
	}

	if (*m_pReloading == true)
	{
		m_pGuage->Set_Charging(true);
	}
	else
	{
		_int iCheck = 0;
		for (int i = 0; i < 2; i++)
		{
			if (m_pWeaponItem[i]->Get_Charging() == true)
				iCheck++;
		}
		if (iCheck > 0)
			m_pGuage->Set_Charging(true);
		else
			m_pGuage->Set_Charging(false);
	}
}

void CLevel_Yard::Text_Render()
{
	if(m_bVictory == false)
	{
		if (*m_pPlayer->Get_BuildMode() == true)
			m_pGameInstance->Render_Text(TEXT("GumiFont"), TEXT("건설 모드 건너뛰기"), _float2(g_iWinSizeX * 0.45f, g_iWinSizeY * 0.785f), XMVectorSet(1.f, 1.f, 1.f, 0.5f), 0.6f);
	}

	if (m_iDrawNumber == 99)
		return;

	if ((m_eTextState & STATE_HALF_HP) == 0 && m_iDrawNumber == STATE_HALF_HP)
	{
		m_pGameInstance->Render_Text(TEXT("GumiFont"), TEXT("서둘러, 하이퍼코어가 거의 파괴되었어."), _float2(g_iWinSizeX * 0.38f, g_iWinSizeY * 0.05f), XMVectorSet(1.f, 1.f, 1.f, 0.5f), 0.5f);
		m_pGameInstance->Render_Text(TEXT("GumiFont"), TEXT("어서 빨리 적을 무찌르게"), _float2(g_iWinSizeX * 0.4f, g_iWinSizeY * 0.08f), XMVectorSet(1.f, 1.f, 1.f, 0.5f), 0.5);
	}
	if ((m_eTextState & STATE_HALF_ENERGY) == 0 && m_iDrawNumber == STATE_HALF_ENERGY)
	{
		m_pGameInstance->Render_Text(TEXT("GumiFont"), TEXT("파워 노드가 계속 작동할 수 있게 주의해,"), _float2(g_iWinSizeX * 0.4f, g_iWinSizeY * 0.05f), XMVectorSet(1.f, 1.f, 1.f, 0.5f), 0.5f);
		m_pGameInstance->Render_Text(TEXT("GumiFont"), TEXT("얼마 안 남았어!."), _float2(g_iWinSizeX * 0.4f, g_iWinSizeY * 0.08f), XMVectorSet(1.f, 1.f, 1.f, 0.5f), 0.5);
		
	}
	if ((m_eTextState & STATE_WARNING) == 0 && m_iDrawNumber == STATE_WARNING)
	{
		m_pGameInstance->Render_Text(TEXT("GumiFont"), TEXT("웨이브가 곧 끝날거야"), _float2(g_iWinSizeX * 0.38f, g_iWinSizeY * 0.05f), XMVectorSet(1.f, 1.f, 1.f, 0.5f), 0.6f);
		m_pGameInstance->Render_Text(TEXT("GumiFont"), TEXT("놈들에게 잊지 못할 기억을 선사해 주자고"), _float2(g_iWinSizeX * 0.4f, g_iWinSizeY * 0.08f), XMVectorSet(1.f, 1.f, 1.f, 0.5f), 0.6f);
	}
	if ((m_eTextState & STATE_MISSILE_WARNING) == 0 && m_iDrawNumber == STATE_MISSILE_WARNING)
	{
		m_pGameInstance->Render_Text(TEXT("GumiFont"), TEXT("표적 시스템을 재보장하는 것만이"), _float2(g_iWinSizeX * 0.4f, g_iWinSizeY * 0.05f), XMVectorSet(1.f, 1.f, 1.f, 0.5f), 0.6f);
		m_pGameInstance->Render_Text(TEXT("GumiFont"), TEXT("놈들을 막는 유일한 방법이야."), _float2(g_iWinSizeX * 0.4f, g_iWinSizeY * 0.08f), XMVectorSet(1.f, 1.f, 1.f, 0.5f), 0.6f);
	}
	if ((m_eTextState & STATE_GOOD) == 0 && m_iDrawNumber == STATE_GOOD)
	{
		m_pGameInstance->Render_Text(TEXT("GumiFont"), TEXT("아주 잘했어!"), _float2(g_iWinSizeX * 0.4f, g_iWinSizeY * 0.05f), XMVectorSet(1.f, 1.f, 1.f, 0.5f), 0.6f);
	}
	if ((m_eTextState & STATE_PROVOKE) == 0 && m_iDrawNumber == STATE_PROVOKE)
	{
		m_pGameInstance->Render_Text(TEXT("GumiFont"), TEXT("겁쟁이들 같으니!"), _float2(g_iWinSizeX * 0.4f, g_iWinSizeY * 0.05f), XMVectorSet(1.f, 1.f, 1.f, 0.5f), 0.6f);
		m_pGameInstance->Render_Text(TEXT("GumiFont"), TEXT("놈들이 포니를 보냈어. 조심해!"), _float2(g_iWinSizeX * 0.4f, g_iWinSizeY * 0.08f), XMVectorSet(1.f, 1.f, 1.f, 0.5f), 0.6f);
	}
	
}

void CLevel_Yard::Text_Update(_float fTimeDelta)
{
	// HP경고
	if (m_pBrain->Get_Hp() <= 50.f && (m_eTextState & STATE_HALF_HP) == 0 && m_iCurrentRound != MISSILEROUND)
	{
		m_iDrawNumber = STATE_HALF_HP;
		Conversation_Draw(true);
		m_fConversation_Draw_Timer += fTimeDelta;
		// 3초 지나면 끄기
		if(m_fConversation_Draw_Timer >= 3.f)
		{
			Conversation_Draw(false);
			m_eTextState |= STATE_HALF_HP;
			m_fConversation_Draw_Timer = 0.f;
		}

	}
	// 에너지 코어 경고
	else if (m_pBrain->Get_Energy() <= 30.f && (m_eTextState & STATE_HALF_ENERGY) == 0 && m_iCurrentRound != MISSILEROUND)
	{
		Conversation_Draw(true);
		m_iDrawNumber = STATE_HALF_ENERGY;
		m_fConversation_Draw_Timer += fTimeDelta;
		// 3초 지나면 끄기
		if (m_fConversation_Draw_Timer >= 3.f)
		{
			Conversation_Draw(false);
			m_eTextState |= STATE_HALF_ENERGY;
			m_fConversation_Draw_Timer = 0.f;
		}
	}
	// 미사일 성공
	else if (m_pMissile_Truck->Get_knockdown() == true && (m_eTextState & STATE_GOOD) == 0)
	{
		m_fTimerMissile += fTimeDelta;
		if(m_fTimerMissile >= 1.f)
		{
			m_iDrawNumber = STATE_GOOD;
			Conversation_Draw(true);
			m_fConversation_Draw_Timer += fTimeDelta;
			// 3초 지나면 끄기
			if (m_fConversation_Draw_Timer >= 3.f)
			{
				Conversation_Draw(false);
				m_eTextState |= STATE_GOOD;
				m_fConversation_Draw_Timer = 0.f;
			}
		}
	}
	// 미사일 경고, 미사일 라운드 시작전 쉬는 시간의 끝나기전?
	else if (*m_pPlayer->Get_BuildMode() == true 
		&& m_pRound[MISSILEROUND -1]->IsRound_End() == true
		&& m_pRound[MISSILEROUND]->IsRound_End() == false
		&& m_iCurrentRound == 0 
		&& (m_eTextState & STATE_MISSILE_WARNING) == 0)
	{
		m_iDrawNumber = STATE_MISSILE_WARNING;
		Conversation_Draw(true);
		m_fConversation_Draw_Timer += fTimeDelta;
		// 3초 지나면 끄기
		if (m_fConversation_Draw_Timer >= 5.f)
		{
			Conversation_Draw(false);
			m_eTextState |= STATE_MISSILE_WARNING;
			m_fConversation_Draw_Timer = 0.f;
		}
	}
	//// 칭찬
	//else if (m_pBrain->Get_Hp() <= 50.f && (m_eTextState & STATE_GOOD) == 0)
	//{
	//	m_iDrawNumber = STATE_GOOD;
	//	Conversation_Draw(true);
	//	m_fConversation_Draw_Timer += fTimeDelta;
	//	// 3초 지나면 끄기
	//	if (m_fConversation_Draw_Timer >= 3.f)
	//	{
	//		Conversation_Draw(false);
	//		m_eTextState |= STATE_GOOD;
	//		m_fConversation_Draw_Timer = 0.f;
	//	}
	//}
	// 적들 약올리기
	else if (m_iCurrentRound == 2 && (m_eTextState & STATE_PROVOKE) == 0)
	{
		m_iDrawNumber = STATE_PROVOKE;
		Conversation_Draw(true);
		m_fConversation_Draw_Timer += fTimeDelta;
		// 3초 지나면 끄기
		if (m_fConversation_Draw_Timer >= 3.f)
		{
			Conversation_Draw(false);
			m_eTextState |= STATE_PROVOKE;
			m_fConversation_Draw_Timer = 0.f;
		}
	}


	//if (m_iCurrentRound == 1) // 조건 바꾸자, 라운드 바뀔 때 상태 초기화 하는 로직
	//{
	//	m_eTextState &= ~STATE_HALF_HP;

	//}
}

void CLevel_Yard::Conversation_Draw(_bool bDraw)
{
	if (bDraw == false)
		m_iDrawNumber = 99;
	m_pConversationBox->Set_Draw(bDraw);
	m_pCharacter->Set_Draw(bDraw);
}


HRESULT CLevel_Yard::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CUI_3D::UIOBJ_DESC pObjDesc{};
	pObjDesc.eUIType = CUI_3D::UI_NUCLEAR;
	pObjDesc.fScale = _float3{ 2.5f,2.5f,2.5f };
	pObjDesc.m_eLevel = LEVEL_YARD;
	pObjDesc.pCamera = m_pCamera;
	pObjDesc.pPlayer = m_pPlayer;
	pObjDesc.m_pMissile_Truck = m_pMissile_Truck;
	m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_3DUI"), &pObjDesc);


	CInGameUI::INGAMEUI_DESC	Missile_Timer_Desc{};
	Missile_Timer_Desc.eLevel = LEVEL_YARD;
	Missile_Timer_Desc.eUITag = CInGameUI::UI_MISSILE_TIMER;
	Missile_Timer_Desc.fX = g_iWinSizeX * 0.5f;
	Missile_Timer_Desc.fY = g_iWinSizeY * 0.1f;
	Missile_Timer_Desc.fDepth = 0.1f;
	Missile_Timer_Desc.fSizeX = 400.f;
	Missile_Timer_Desc.fSizeY = 100.f;
	Missile_Timer_Desc.fTimer = m_pMissile_Truck->Get_HP_Ptr();
	Missile_Timer_Desc.iRound = &m_iCurrentRound;
	m_pMissile_Timer = static_cast<CInGameUI*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &Missile_Timer_Desc));



	CUI_CircleGuage::CIRCLEGAUGE_DESC pCircleDesc{};
	pCircleDesc.eLevel = LEVEL_YARD;
	pCircleDesc.fSizeX = 200.f;
	pCircleDesc.fSizeY = 200.f;
	pCircleDesc.iData = 0;
	pCircleDesc.fX = g_iWinSizeX * 0.5f;
	pCircleDesc.fY = g_iWinSizeY * 0.5f;
	pCircleDesc.fDepth = 0.1f;
	pCircleDesc.pPlayer = m_pPlayer;
	pCircleDesc.vecMarks = &m_vecTrapMark;
	pCircleDesc.pEnergy_Machine = m_pEnergyMachine;
	pCircleDesc.pEnergyMachine_Cap = m_pEnergyMachine_Cap;
	CGameObject* pGuage = m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, TEXT("Layer_CircleUI"), TEXT("Prototype_GameObject_Circle_UI"), &pCircleDesc);
	m_pGuage = static_cast<CUI_CircleGuage*>(pGuage);

	CInGameUI::INGAMEUI_DESC	DescCenterIcon{};
	DescCenterIcon.eLevel = LEVEL_YARD;
	DescCenterIcon.eUITag = CInGameUI::UI_CENTERICON;
	DescCenterIcon.fSizeX = 40.f;
	DescCenterIcon.fSizeY = 40.f;
	DescCenterIcon.iData = 0;
	DescCenterIcon.fX = g_iWinSizeX * 0.5f;
	DescCenterIcon.fY = g_iWinSizeY * 0.5f;
	DescCenterIcon.fDepth = 0.1f;
	DescCenterIcon.pPlayer = m_pPlayer;
	DescCenterIcon.pCircle = m_pGuage;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &DescCenterIcon)))
		return E_FAIL;

	// 총알 아이콘
	CInGameUI::INGAMEUI_DESC	DescBulletIcon{};
	DescBulletIcon.eLevel = LEVEL_YARD;
	DescBulletIcon.eUITag = CInGameUI::UI_BULLET;
	DescBulletIcon.fSizeX = 30.f;
	DescBulletIcon.fSizeY = 30.f;
	DescBulletIcon.iData = 0;
	DescBulletIcon.fX = g_iWinSizeX - 140.f;
	DescBulletIcon.fY = 610.f;
	DescBulletIcon.fDepth = 0.1f;
	DescBulletIcon.pPlayer = m_pPlayer;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &DescBulletIcon)))
		return E_FAIL;

	// 현재 총알 개수
	CNumberUI::NUMBERUI_DESC pBullet{};
	pBullet.fDepth = 0.1f;
	pBullet.eLevel = LEVEL_YARD;
	pBullet.fX = g_iWinSizeX - 100.f;
	pBullet.fY = 610.f;
	pBullet.fSizeX = 26.f;
	pBullet.fSizeY = 26.f;
	pBullet.eTypeUsage = CNumberUI::TYPE_BULLET;
	pBullet.eDigit = CNumberUI::ONE_DIGIT; // 일의 자리수
	pBullet.iData = 0;
	pBullet.pPlayer = m_pPlayer;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UINumber"), &pBullet)))
		return E_FAIL;

	pBullet.eDigit = CNumberUI::TEN_DIGIT; // 십의 자리수
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UINumber"), &pBullet)))
		return E_FAIL;

	// 총 탄알 수
	pBullet.fSizeX = 15.f;
	pBullet.fSizeY = 15.f;
	pBullet.fX = g_iWinSizeX - 40.f;
	pBullet.fY = 620.f;
	pBullet.eTypeUsage = CNumberUI::TYPE_FULLBULLET;
	pBullet.eDigit = CNumberUI::ONE_DIGIT; // 일의 자리수
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UINumber"), &pBullet)))
		return E_FAIL;

	pBullet.eDigit = CNumberUI::TEN_DIGIT; // 십의 자리수
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UINumber"), &pBullet)))
		return E_FAIL;


	CInGameUI::INGAMEUI_DESC	DescSlice{};
	DescSlice.eLevel = LEVEL_YARD;
	DescSlice.eUITag = CInGameUI::UI_SLICE;
	DescSlice.fSizeX = 20.f;
	DescSlice.fSizeY = 20.f;
	DescSlice.iData = 0;
	DescSlice.fX = g_iWinSizeX - 60.f;
	DescSlice.fY = 615.f;
	DescSlice.fDepth = 0.1f;
	DescSlice.pPlayer = m_pPlayer;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &DescSlice)))
		return E_FAIL;

	// 크로스라인
	CCrossLine::UIOBJECT_DESC			Desc{};
	Desc.fX = g_iWinSizeX * 0.5f;
	Desc.fY = g_iWinSizeY * 0.5f;
	Desc.fSizeX = 20.f;
	Desc.fSizeY = 20.f;
	Desc.iData = 10;
	Desc.fDepth = 0.1f;
	Desc.eLevel = LEVEL_YARD;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_CrossLine"), &Desc)))
		return E_FAIL;




	CInGameUI::INGAMEUI_DESC	pDesc9{};
	pDesc9.eLevel = LEVEL_YARD;
	pDesc9.eUITag = CInGameUI::UI_CONVERSATIONBOX_BACKGROUND;
	pDesc9.fSizeX = 180.f;
	pDesc9.fSizeY = 90.f;
	pDesc9.iData = 0;
	pDesc9.fX = 125.f;
	pDesc9.fY = 100.f;
	pDesc9.fDepth = 0.5f;
	pDesc9.iIndex = 1;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc9)))
		return E_FAIL;


	CInGameUI::INGAMEUI_DESC	pDesc5{};
	pDesc5.eLevel = LEVEL_YARD;
	pDesc5.eUITag = CInGameUI::UI_BATTERY;
	pDesc5.fSizeX = 20.f;
	pDesc5.fSizeY = 20.f;
	pDesc5.iData = 0;
	pDesc5.fX = 95.f;
	pDesc5.fY = 120.f;
	pDesc5.fDepth = 0.1f;
	m_pBatteryUI = static_cast<CInGameUI*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc5));


	CInGameUI::INGAMEUI_DESC	pDesc8{};
	pDesc8.eLevel = LEVEL_YARD;
	pDesc8.eUITag = CInGameUI::UI_BATTERY_GAGE;
	pDesc8.fSizeX = 80.f;
	pDesc8.fSizeY = 10.f;
	pDesc8.iData = 0;
	pDesc8.fX = 150;
	pDesc8.fY = 120;
	pDesc8.fDepth = 0.2f;
	pDesc8.iIndex = 3;
	m_pBatteryGaugeUI = static_cast<CInGameUI*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc8));


	pDesc8.fDepth = 0.1f;
	pDesc8.iIndex = 3;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc8)))
		return E_FAIL;


	CInGameUI::INGAMEUI_DESC	pDesc10{};
	pDesc10.eLevel = LEVEL_YARD;
	pDesc10.eUITag = CInGameUI::UI_MACHINE_HP;
	pDesc10.fSizeX = 100.f;
	pDesc10.fSizeY = 10.f;
	pDesc10.iData = 0;
	pDesc10.fX = 140;
	pDesc10.fY = 100;
	pDesc10.fBrainHP = m_pBrain->Get_BrainHp();
	pDesc10.fDepth = 0.2f;
	pDesc10.iIndex = 4;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc10)))
		return E_FAIL;

	pDesc10.fDepth = 0.1f;
	pDesc10.iIndex = 1;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc10)))
		return E_FAIL;


	CInGameUI::INGAMEUI_DESC	pDesc11{};
	pDesc11.eLevel = LEVEL_YARD;
	pDesc11.eUITag = CInGameUI::UI_MACHINE_ENERGY;
	pDesc11.fSizeX = 100.f;
	pDesc11.fSizeY = 10.f;
	pDesc11.iData = 0;
	pDesc11.fX = 140;
	pDesc11.fY = 80;
	pDesc11.fDepth = 0.3f;
	pDesc11.iIndex = 4;
	pDesc11.fBrainEnergy = m_pBrain->Get_BrainEnergy();
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc11)))
		return E_FAIL;

	pDesc11.fDepth = 0.2f;
	pDesc11.iIndex = 0;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc11)))
		return E_FAIL;


	// 플레이어 정보 뒷 배경
	CInGameUI::INGAMEUI_DESC	pDesc22{};
	pDesc22.eLevel = LEVEL_YARD;
	pDesc22.eUITag = CInGameUI::UI_CONVERSATIONBOX_BACKGROUND;
	pDesc22.fSizeX = 160.f;
	pDesc22.fSizeY = 90.f;
	pDesc22.iData = 0;
	pDesc22.fX = g_iWinSizeX - 95.f;
	pDesc22.fY = g_iWinSizeY - 45.f;
	pDesc22.fDepth = 0.4f;
	pDesc22.iIndex = 1;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc22)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc13{};
	pDesc13.eLevel = LEVEL_YARD;
	pDesc13.eUITag = CInGameUI::UI_PLAYER_HP;
	pDesc13.fSizeX = 120.f;
	pDesc13.fSizeY = 20.f;
	pDesc13.iData = 0;
	pDesc13.fX = g_iWinSizeX - 90.f;
	pDesc13.fY = g_iWinSizeY - 45.f;
	pDesc13.fPlayerHP = m_pPlayer->Get_PlayerHP();
	pDesc13.fDepth = 0.3f;
	pDesc13.iIndex = 4;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc13)))
		return E_FAIL;

	pDesc13.fDepth = 0.2f;
	pDesc13.iIndex = 1;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc13)))
		return E_FAIL;


	CInGameUI::INGAMEUI_DESC	pDesc14{};
	pDesc14.eLevel = LEVEL_YARD;
	pDesc14.eUITag = CInGameUI::UI_PLAYER_ENERGY;
	pDesc14.fSizeX = 120.f;
	pDesc14.fSizeY = 20.f;
	pDesc14.iData = 0;
	pDesc14.fX = g_iWinSizeX - 90.f;
	pDesc14.fY = g_iWinSizeY - 70.f;
	pDesc14.fDepth = 0.3f;
	pDesc14.iIndex = 4;
	pDesc14.fPlayerEnergy = m_pPlayer->Get_PlayerEnergy();
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc14)))
		return E_FAIL;

	pDesc14.fDepth = 0.2f;
	pDesc14.iIndex = 0;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc14)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc15{};
	pDesc15.eLevel = LEVEL_YARD;
	pDesc15.eUITag = CInGameUI::UI_ENERGY_ICON;
	pDesc15.fSizeX = 23.f;
	pDesc15.fSizeY = 23.f;
	pDesc15.iData = 0;
	pDesc15.fX = g_iWinSizeX - 160.f;
	pDesc15.fY = g_iWinSizeY - 70.f;
	pDesc15.fDepth = 0.2f;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc15)))
		return E_FAIL;


	CInGameUI::INGAMEUI_DESC	pDesc16{};
	pDesc16.eLevel = LEVEL_YARD;
	pDesc16.eUITag = CInGameUI::UI_HP_ICON;
	pDesc16.fSizeX = 15.f;
	pDesc16.fSizeY = 15.f;
	pDesc16.iData = 0;
	pDesc16.fX = g_iWinSizeX - 160.f;
	pDesc16.fY = g_iWinSizeY - 45.f;
	pDesc16.fDepth = 0.2f;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc16)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc17{};
	pDesc17.eLevel = LEVEL_YARD;
	pDesc17.eUITag = CInGameUI::UI_CREDIT_ICON;
	pDesc17.fSizeX = 18.f;
	pDesc17.fSizeY = 18.f;
	pDesc17.iData = 0;
	pDesc17.fX = g_iWinSizeX - 160.f;
	pDesc17.fY = g_iWinSizeY - 22.f;
	pDesc17.fDepth = 0.2f;
	pDesc17.pPlayer = m_pPlayer;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc17)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDescMidCenter_Sign{};
	pDescMidCenter_Sign.eLevel = LEVEL_YARD;
	pDescMidCenter_Sign.eUITag = CInGameUI::UI_BUILDMODE_CONVERSATIONBOX;
	pDescMidCenter_Sign.fSizeX = 240.f;
	pDescMidCenter_Sign.fSizeY = 30.f;
	pDescMidCenter_Sign.iData = 0;
	pDescMidCenter_Sign.fX = g_iWinSizeX * 0.5 + 20.f;
	pDescMidCenter_Sign.fY = g_iWinSizeY * 0.8f;
	pDescMidCenter_Sign.fDepth = 0.4f;
	pDescMidCenter_Sign.iIndex = 1;
	pDescMidCenter_Sign.pPlayer = m_pPlayer;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDescMidCenter_Sign)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pBuild_F_Desc{};
	pBuild_F_Desc.eLevel = LEVEL_YARD;
	pBuild_F_Desc.eUITag = CInGameUI::UI_BUILDMODE_F;
	pBuild_F_Desc.fSizeX = 25.f;
	pBuild_F_Desc.fSizeY = 25.f;
	pBuild_F_Desc.iData = 0;
	pBuild_F_Desc.fX = g_iWinSizeX * 0.5f - 80.f;
	pBuild_F_Desc.fY = g_iWinSizeY * 0.8f;
	pBuild_F_Desc.fDepth = 0.1f;
	pBuild_F_Desc.pPlayer = m_pPlayer;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pBuild_F_Desc)))
		return E_FAIL;


#pragma region 대화상자
	CInGameUI::INGAMEUI_DESC	pDesc7{};
	pDesc7.eLevel = LEVEL_YARD;
	pDesc7.eUITag = CInGameUI::UI_CONVERSATIONBOX;
	pDesc7.fSizeX = 450.f;
	pDesc7.fSizeY = 90.f;
	pDesc7.iData = 0;
	pDesc7.fX = g_iWinSizeX * 0.55f;
	pDesc7.fY = 65.f;
	pDesc7.fDepth = 0.2f;
	pDesc7.iIndex = 0;	
	pDesc7.pPlayer = m_pPlayer;
	m_pConversationBox = static_cast<CInGameUI*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc7));
	
	CInGameUI::INGAMEUI_DESC	pDesc6{};
	pDesc6.eLevel = LEVEL_YARD;
	pDesc6.eUITag = CInGameUI::UI_CHARACTER;
	pDesc6.fSizeX = 100.f;
	pDesc6.fSizeY = 100.f;
	pDesc6.iData = 0;
	pDesc6.fX = g_iWinSizeX * 0.7f;
	pDesc6.fY = 65.f;
	pDesc6.fDepth = 0.1f;
	pDesc6.pPlayer = m_pPlayer;
	m_pCharacter = static_cast<CInGameUI*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc6));
		

#pragma endregion 대화상자

	CInGameUI::INGAMEUI_DESC	Ending{};
	Ending.eLevel = LEVEL_YARD;
	Ending.eUITag = CInGameUI::UI_VICTORY;
	Ending.fSizeX = g_iWinSizeX;
	Ending.fSizeY = g_iWinSizeY;
	Ending.fX = g_iWinSizeX * 0.5f;
	Ending.fY = g_iWinSizeY * 0.5f;
	Ending.fDepth = 0.f;
	CGameObject* pEnd = m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"), &Ending);
	m_pEnding = static_cast<CInGameUI*>(pEnd);

	// 코인 UI( 숫자 )
	CNumberUI::NUMBERUI_DESC pCoin{};
	pCoin.fDepth = 0.1f;
	pCoin.eLevel = LEVEL_YARD;
	pCoin.fX = g_iWinSizeX - 140.f;
	pCoin.fY = g_iWinSizeY - 22.f;
	pCoin.fSizeX = 13.f;
	pCoin.fSizeY = 13.f;
	pCoin.eTypeUsage = CNumberUI::TYPE_COIN;
	pCoin.eDigit = CNumberUI::ONE_DIGIT; // 첫 번째 자리수
	pCoin.iData = 0;
	pCoin.pPlayer = m_pPlayer;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UINumber"), &pCoin)))
		return E_FAIL;

	pCoin.eDigit = CNumberUI::TEN_DIGIT; // 십의 자리
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UINumber"), &pCoin)))
		return E_FAIL;

	pCoin.eDigit = CNumberUI::HUNDREDS_DIGIT; // 백의 자리
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UINumber"), &pCoin)))
		return E_FAIL;

	pCoin.eDigit = CNumberUI::THOUSANDS_DIGIT; // 천의 자리
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UINumber"), &pCoin)))
		return E_FAIL;

	pCoin.eDigit = CNumberUI::TENS_OF_THOUSANDS_DIGIT; // 만의 자리
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UINumber"), &pCoin)))
		return E_FAIL;

	pCoin.eDigit = CNumberUI::HUNDREDS_OF_THOUSANDS_DIGIT; // 십만의 자리
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UINumber"), &pCoin)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Yard::Ready_Layer_Terrain(const _tchar* pLayerTag)
{
	CTerrain::TERRAIN_DESC pDesc{};
	pDesc.eID = LEVEL_YARD;
	pDesc.pPlayer = m_pPlayer;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_Terrain"), &pDesc)))
		return E_FAIL;

	CSky::SKY_DESC pSky{};
	pSky.m_eLevel = LEVEL_YARD;
	pSky.pCamPos = m_pCamera->Get_Camera_Pos();
	pSky.pCamera = m_pCamera;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_Sky"),&pSky)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Yard::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC			Desc{};

	Desc.vEye = _float4(418.755f, 1.5f, 245.710f, 1.f);
	Desc.fPosition = _float3(418.755f, 1.5f, 245.710f);
	Desc.vAt = _float4(0.f, 0.f, 1.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNearZ = 0.1f;
	Desc.fFar = 900.f;
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	Desc.fSpeedPerSec = 20.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fMouseSensor = 0.1f;
	Desc.eLevel = LEVEL_YARD;
	Desc.matPlayerWorld = m_pPlayer->Get_Transform()->Get_WorldMatrixPtr();
	Desc.m_vecTPS_CamPos = m_pPlayer->Get_TPSPosptr();
	Desc.m_vecFPS_CamPos = m_pPlayer->Get_FPSPosptr();
	Desc.iViewState = m_pPlayer->Get_ViewState();
	Desc.m_vecWeaponPos = m_pPlayer->Get_WeaponPos();
	Desc.m_vecWeaponDir = m_pPlayer->Get_WeaponDir();
	Desc.bShotNow = m_pPlayer->Get_ShotNow();
	Desc.bShotStart = m_pPlayer->Get_ShotStart();
	Desc.iWeaponState = m_pPlayer->Get_WeaponState();
	Desc.iUpperMotion = m_pPlayer->Get_UpperMotion();
	m_pCamera = static_cast<CCamera_Free*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &Desc));
	m_pPlayer->Set_CameraAt(m_pCamera->Get_Camera_At());
	m_pPlayer->Set_CameraPos(m_pCamera->Get_Camera_Pos());
	return S_OK;
}

HRESULT CLevel_Yard::Ready_Lights()
{
	LIGHT_DESC	LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(-0.5f, -1.f, -0.5f, 0.f);
	LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Yard::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CPlayer::PLAYER_DESC Desc{};
	Desc.vCameraAt = m_pCamera->Get_Camera_At();
	Desc.vCameraPos = m_pCamera->Get_Camera_Pos();
	Desc.iRound = &m_iCurrentRound;
	Desc.m_eLevelID = LEVEL_YARD;
	Desc.iCellIdx = 598;
	Desc.fPosition = _float3(645.424f, 0.f, 559.107f);

	CGameObject* pPlayer = m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_Player"), &Desc);
	m_pPlayer = static_cast<CPlayer*>(pPlayer);

	return S_OK;
}

HRESULT CLevel_Yard::Ready_Layer_WeaponITem(const _tchar* pLayerTag)
{
	CWeapon_Item::WEAPONITEM_DESC Desc{};
	Desc.eID = LEVEL_YARD;
	Desc.iModelIndex = 5;
	Desc.fScale = { 30.f,30.f,30.f };
	Desc.fPosition = { 469.82f, 3.f, 442.094f };
	CGameObject* m_pItem = m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_WeaponItem"), &Desc);
	m_pWeaponItem[0] = static_cast<CWeapon_Item*>(m_pItem);

	Desc.fScale = { 10.f,10.f,10.f };
	Desc.iModelIndex = 7;
	Desc.fPosition = { 433.203f, 2.f, 457.242f };
	m_pItem = m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_WeaponItem"), &Desc);
	m_pWeaponItem[1] = static_cast<CWeapon_Item*>(m_pItem);


	CBattery::BATTERY_DESC pBattery;
	pBattery.eID = LEVEL_YARD;
	pBattery.fScale = { 7.f, 7.f, 7.f };
	pBattery.fPosition = { 400.f,2.f, 450.f };
	pBattery.pGauge = m_pGuage;
	pBattery.pPlayer = m_pPlayer;
	pBattery.pEnergy_Machine = m_pEnergyMachine;
	pBattery.pBrain = m_pBrain;
	pBattery.pInGameUI = m_pBatteryUI;
	pBattery.pInGameUI_Gauge = m_pBatteryGaugeUI;
	m_pBattery = static_cast<CBattery*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_Battery"), &pBattery));


	return S_OK;
}

HRESULT CLevel_Yard::Ready_Layer_ITem(const _tchar* pLayerTag)
{
	CCollector::COLLECTOR_DESC pCollectItem{};
	pCollectItem.eID = LEVEL_YARD;
	pCollectItem.fScale = { 8.f,8.f ,8.f };
	pCollectItem.iModelIndex = 154;
	pCollectItem.fPosition = { 439.75f, 3.1f, 576.119f };
	pCollectItem.pPlayer = m_pPlayer;
	(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_Collect_Item"), &pCollectItem));

	          
	// 코인 아이템
	CCoin_Item::COINITEM_DESC pCoinItem{};
	pCoinItem.eID = LEVEL_YARD;
	pCoinItem.pPlayer = m_pPlayer;
	pCoinItem.fScale = { 6.f, 6.f,6.f };
	pCoinItem.fPosition = { 571.985f, 3.f, 237.756f };
	pCoinItem.iModelIndex = 47;
	pCoinItem.pGuage = m_pGuage;
	(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_CoinItem"), &pCoinItem));
	
	pCoinItem.fPosition = { 554.751f, 3.f, 219.951f };
	pCoinItem.iModelIndex = 47;
	(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_CoinItem"), &pCoinItem));

	pCoinItem.fPosition = { 540.489f, 3.f, 614.398f };
	pCoinItem.iModelIndex = 48;
	(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_CoinItem"), &pCoinItem));

	pCoinItem.fPosition = { 305.877f, 3.f, 608.576f };
	pCoinItem.iModelIndex = 48;
	(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_CoinItem"), &pCoinItem));

	pCoinItem.fPosition = { 293.493f, 3.f, 304.906f };
	pCoinItem.iModelIndex = 49;
	(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_CoinItem"), &pCoinItem));

	// 힐 아이템
	CHp_Item::HPITEM_DESC pHpItem{};
	pHpItem.eID = LEVEL_YARD;
	pHpItem.pPlayer = m_pPlayer;
	pHpItem.fScale = { 8.f, 8.f,8.f };
	pHpItem.fPosition = { 644.512f, 3.f, 565.156f };
	pHpItem.pGuage = m_pGuage;
	(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_HpItem"), &pHpItem));

	return S_OK;
}

HRESULT CLevel_Yard::Ready_Layer_PlayerBuild(const _tchar* pLayerTag)
{
	// 브레인 코어
	CBrainCore::BRAIN_CORE_DESC pDesc{};
	pDesc.eID = LEVEL_YARD;
	pDesc.fPosition = _float3(490.f, 0.1f, 505.f);
	pDesc.fScale = { 4.f,4.f,4.f };
	pDesc.iModelComponentIndex = 205;
	m_pBrain = static_cast<CBrainCore*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_BrainCore"), &pDesc));
	// 563
	// 에너지 머신
	CEnergy_Machine::ENERGYMACHINE_DESC pEnergyMachine{};
	pEnergyMachine.eID = LEVEL_YARD;
	pEnergyMachine.fScale = { 5.f,5.f,5.f };
	pEnergyMachine.fPosition = _float3{ 477.267f, 0.1f,532.115f };
	pEnergyMachine.pPlayer = m_pPlayer;

	m_pEnergyMachine = static_cast<CEnergy_Machine*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_EnergyMachine"), &pEnergyMachine));


	// 에너지 머신 캡
	CEnergy_Cap::ENERGYCAP_DESC pEnergyCap{};
	pEnergyCap.eID = LEVEL_YARD;
	pEnergyCap.fScale = { 5.f,5.f,5.f };
	pEnergyCap.fPosition = _float3{ 476.075f, 5.82203f,532.203f };
	m_pEnergyMachine_Cap = static_cast<CEnergy_Cap*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_EnergyCap"), &pEnergyCap));

	return S_OK;
}

HRESULT CLevel_Yard::Ready_Layer_Icon(const _tchar* pLayerTag)
{
	CInGameUI::INGAMEUI_DESC	pDesc{};
	pDesc.eLevel = LEVEL_YARD;
	pDesc.eUITag = CInGameUI::UI_F;
	pDesc.fSizeX = 30.f;
	pDesc.fSizeY = 30.f;
	pDesc.iData = 0;
	pDesc.fX = g_iWinSizeX - 75.f;
	pDesc.fY = 490.f;
	pDesc.fDepth = 0.1f;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc1{};
	pDesc1.eLevel = LEVEL_YARD;
	pDesc1.eUITag = CInGameUI::UI_V;
	pDesc1.fSizeX = 30.f;
	pDesc1.fSizeY = 30.f;
	pDesc1.iData = 0;
	pDesc1.fX = g_iWinSizeX - 75.f;
	pDesc1.fY = 530.f;
	pDesc1.fDepth = 0.1f;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc1)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc2{};
	pDesc2.eLevel = LEVEL_YARD;
	pDesc2.eUITag = CInGameUI::UI_SPACE;
	pDesc2.fSizeX = 70.f;
	pDesc2.fSizeY = 30.f;
	pDesc2.iData = 0;
	pDesc2.fX = g_iWinSizeX - 55.f;
	pDesc2.fY = 450.f;
	pDesc2.fDepth = 0.1f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc2)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc19{};
	pDesc19.eLevel = LEVEL_YARD;
	pDesc19.eUITag = CInGameUI::UI_JUMP_ICON;
	pDesc19.fSizeX = 28.f;
	pDesc19.fSizeY = 32.f;
	pDesc19.iData = 0;
	pDesc19.fX = g_iWinSizeX - 110.f;
	pDesc19.fY = 450.f;
	pDesc19.fDepth = 0.1f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc19)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc3{};
	pDesc3.eLevel = LEVEL_YARD;
	pDesc3.eUITag = CInGameUI::UI_SHIFT;
	pDesc3.fSizeX = 70.f;
	pDesc3.fSizeY = 30.f;
	pDesc3.iData = 0;
	pDesc3.fX = g_iWinSizeX - 55.f;
	pDesc3.fY = 410.f;
	pDesc3.fDepth = 0.1f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc3)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc18{};
	pDesc18.eLevel = LEVEL_YARD;
	pDesc18.eUITag = CInGameUI::UI_RUN_ICON;
	pDesc18.fSizeX = 30.f;
	pDesc18.fSizeY = 30.f;
	pDesc18.iData = 0;
	pDesc18.fX = g_iWinSizeX - 110.f;
	pDesc18.fY = 410.f;
	pDesc18.fDepth = 0.1f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc18)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc20{};
	pDesc20.eLevel = LEVEL_YARD;
	pDesc20.eUITag = CInGameUI::UI_MODECHANGE_ICON;
	pDesc20.fSizeX = 30.f;
	pDesc20.fSizeY = 30.f;
	pDesc20.iData = 0;
	pDesc20.fX = g_iWinSizeX - 110.f;
	pDesc20.fY = 490.f;
	pDesc20.fDepth = 0.1f;
	pDesc20.iIndex = 0;
	pDesc20.pPlayer = m_pPlayer;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc20)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc21{};
	pDesc21.eLevel = LEVEL_YARD;
	pDesc21.eUITag = CInGameUI::UI_PUNCH_ICON;
	pDesc21.fSizeX = 30.f;
	pDesc21.fSizeY = 30.f;
	pDesc21.iData = 0;
	pDesc21.fX = g_iWinSizeX - 110.f;
	pDesc21.fY = 530.f;
	pDesc21.fDepth = 0.1f;
	pDesc21.iIndex = 0;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc21)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc23{};
	pDesc23.eLevel = LEVEL_YARD;
	pDesc23.eUITag = CInGameUI::UI_C;
	pDesc23.fSizeX = 30.f;
	pDesc23.fSizeY = 30.f;
	pDesc23.iData = 0;
	pDesc23.fX = g_iWinSizeX - 75.f;
	pDesc23.fY = 570.f;
	pDesc23.fDepth = 0.1f;
	pDesc23.iIndex = 0;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc23)))
		return E_FAIL;


	CInGameUI::INGAMEUI_DESC	pDesc24{};
	pDesc24.eLevel = LEVEL_YARD;
	pDesc24.eUITag = CInGameUI::UI_VIEWCHANGE_ICON;
	pDesc24.fSizeX = 30.f;
	pDesc24.fSizeY = 30.f;
	pDesc24.iData = 0;
	pDesc24.fX = g_iWinSizeX - 110.f;
	pDesc24.fY = 570.f;
	pDesc24.fDepth = 0.1f;
	pDesc24.iIndex = 0;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc24)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Yard::Ready_Layer_Trap(const _tchar* pLayerTag)
{
	CTrap_Marks::TRAP_MARKS_DESC Mark_Desc{};
	Mark_Desc.eID = LEVEL_YARD;
	Mark_Desc.fScale = { 4.f,4.f,4.f };
	Mark_Desc.pPlayer = m_pPlayer;


#pragma region 레고트랩
	Mark_Desc.eType = CTrap_Marks::BRICKS_TRAP;

	Mark_Desc.fPosition = _float3(502.f, 0.11f, 505.f);
	m_vecTrapMark.push_back(static_cast<CTrap_Marks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_TrapMarks"), &Mark_Desc)));

	Mark_Desc.fPosition = _float3(478.f, 0.11f, 505.f);
	m_vecTrapMark.push_back(static_cast<CTrap_Marks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_TrapMarks"), &Mark_Desc)));

	Mark_Desc.fPosition = _float3(490.f, 0.11f, 493.f);
	m_vecTrapMark.push_back(static_cast<CTrap_Marks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_TrapMarks"), &Mark_Desc)));

	Mark_Desc.fPosition = _float3(490.f, 0.11f, 517.f);
	m_vecTrapMark.push_back(static_cast<CTrap_Marks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_TrapMarks"), &Mark_Desc)));

#pragma endregion 레고트랩

#pragma region 탱크트랩
	Mark_Desc.eType = CTrap_Marks::TANK_TRAP;

	Mark_Desc.fPosition = _float3(620.293f, 0.f, 526.778f);
	m_vecTrapMark.push_back(static_cast<CTrap_Marks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_TrapMarks"), &Mark_Desc)));

	Mark_Desc.fPosition = _float3(575.161f, 0.f, 598.337f);
	m_vecTrapMark.push_back(static_cast<CTrap_Marks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_TrapMarks"), &Mark_Desc)));

	Mark_Desc.fPosition = _float3(586.718f, 0.f, 518.17f);
	m_vecTrapMark.push_back(static_cast<CTrap_Marks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_TrapMarks"), &Mark_Desc)));

	Mark_Desc.fPosition = _float3(496.737f, 0.f, 594.82f);
	m_vecTrapMark.push_back(static_cast<CTrap_Marks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_TrapMarks"), &Mark_Desc)));

	Mark_Desc.fPosition = _float3(399.722f, 0.f, 576.963f);
	m_vecTrapMark.push_back(static_cast<CTrap_Marks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_TrapMarks"), &Mark_Desc)));

	Mark_Desc.fPosition = _float3(399.895f, 0.f, 428.506f);
	m_vecTrapMark.push_back(static_cast<CTrap_Marks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_TrapMarks"), &Mark_Desc)));

	Mark_Desc.fPosition = _float3(453.112f, 0.f, 374.234f);
	m_vecTrapMark.push_back(static_cast<CTrap_Marks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_TrapMarks"), &Mark_Desc)));

	Mark_Desc.fPosition = _float3(550.279f, 0.f, 405.683f);
	m_vecTrapMark.push_back(static_cast<CTrap_Marks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_TrapMarks"), &Mark_Desc)));

	Mark_Desc.fPosition = _float3(600.133f, 0.f, 439.33f);
	m_vecTrapMark.push_back(static_cast<CTrap_Marks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_TrapMarks"), &Mark_Desc)));

	Mark_Desc.fPosition = _float3(482.854f, 0.f, 379.527f);
	m_vecTrapMark.push_back(static_cast<CTrap_Marks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_TrapMarks"), &Mark_Desc)));

	Mark_Desc.fPosition = _float3(434.03f, 0.f, 400.054f);
	m_vecTrapMark.push_back(static_cast<CTrap_Marks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_TrapMarks"), &Mark_Desc)));

	Mark_Desc.fPosition = _float3(370.759f, 0.f, 484.017f);
	m_vecTrapMark.push_back(static_cast<CTrap_Marks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_TrapMarks"), &Mark_Desc)));

	Mark_Desc.fPosition = _float3(356.141f, 0.f, 516.032f);
	m_vecTrapMark.push_back(static_cast<CTrap_Marks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_TrapMarks"), &Mark_Desc)));


#pragma endregion 탱크트랩






	return S_OK;
}

HRESULT CLevel_Yard::Ready_Layer_Damaged(const _tchar* pLayerTag)
{
	CInGameUI::INGAMEUI_DESC	pDesc12{};
	pDesc12.eLevel = LEVEL_YARD;
	pDesc12.eUITag = CInGameUI::UI_DAMAGED;
	pDesc12.fSizeX = g_iWinSizeX;
	pDesc12.fSizeY = g_iWinSizeY;
	pDesc12.iData = 0;
	pDesc12.fX = g_iWinSizeX * 0.5f;
	pDesc12.fY = g_iWinSizeY * 0.5f;
	pDesc12.fDepth = 0.f;
	pDesc12.iIndex = 0;
	pDesc12.pPlayer = m_pPlayer;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc12)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc4{};
	pDesc4.eLevel = LEVEL_YARD;
	pDesc4.eUITag = CInGameUI::UI_DEAD;
	pDesc4.fSizeX = 100.f;
	pDesc4.fSizeY = 100.f;
	pDesc4.iData = 0;
	pDesc4.fX = g_iWinSizeX * 0.5f;
	pDesc4.fY = g_iWinSizeY * 0.3f;
	pDesc4.fDepth = 0.1f;
	pDesc4.pPlayer = m_pPlayer;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc4)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Yard::Ready_Layer_MissileTruck(const _tchar* pLayerTag)
{
	CMissile_Truck::MISSILETRUCK_DESC MissileTruckDesc{};
	MissileTruckDesc.iRound = &m_iCurrentRound;
	MissileTruckDesc.m_eLevelID = LEVEL_YARD;
	MissileTruckDesc.fPosition = _float3(301.378f, 0.f, 528.018f);
	MissileTruckDesc.fScale = { 8.f,8.f ,8.f };
	MissileTruckDesc.pPlayer = m_pPlayer;
	m_pMissile_Truck = static_cast<CMissile_Truck*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, pLayerTag, TEXT("Prototype_GameObject_MissileTruck"), &MissileTruckDesc));


	return S_OK;
}

HRESULT CLevel_Yard::Ready_Layer_Effect(const _tchar* pLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag,
	//	TEXT("Prototype_GameObject_Particle_Snow"))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag,
	//	TEXT("Prototype_GameObject_Particle_Explosion"))))
	//	return E_FAIL;

	//CEffect_Explosion::EFFECT_EXPLOSION_DESC EffectDesc{};
	//EffectDesc.fScale = { 10.f,10.f,10.f };
	//EffectDesc.pCamera = m_pCamera;
	//for (size_t i = 0; i < 20; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag,
	//		TEXT("Prototype_GameObject_Effect_Explosion"), &EffectDesc)))
	//		return E_FAIL;
	//}

	CGrass_Instancing::INSTANCIN_MESH_DESC pGrass{};
	pGrass.eLevel = LEVEL_YARD;

	pGrass.iType = 108;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag,
		TEXT("Prototype_GameObject_Grass"),&pGrass)))
		return E_FAIL;

	pGrass.iType = 109;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag,
		TEXT("Prototype_GameObject_Grass"), &pGrass)))
		return E_FAIL;

	pGrass.iType = 110;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag,
		TEXT("Prototype_GameObject_Grass"), &pGrass)))
		return E_FAIL;
	
	pGrass.iType = 111;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, pLayerTag,
		TEXT("Prototype_GameObject_Grass"), &pGrass)))
		return E_FAIL;
	return S_OK;
}

void CLevel_Yard::Load_Map()
{
	HANDLE hFile = CreateFile(L"../Bin/Data/Environment_Yard.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Environment_Yard File Failed", L"Error", MB_OK);
		return;
	}
	DWORD dwByte = 0;
	LEVELID iLevel;
	_uint iImGuiMode{};
	_int  iModelIndex{};
	_float3 fPos{}, fCollisionBoxScale{}, fScale{};
	_vector	vRight{}, vUp{}, vLook{}, vecCollisionPos{};


	while (ReadFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr) && dwByte > 0)
	{

		ReadFile(hFile, &iModelIndex, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &fScale, sizeof(_float3), &dwByte, nullptr);

		ReadFile(hFile, &fCollisionBoxScale, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &iImGuiMode, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &vecCollisionPos, sizeof(_vector), &dwByte, nullptr);

		ReadFile(hFile, &vRight, sizeof(_vector), &dwByte, nullptr);
		ReadFile(hFile, &vUp, sizeof(_vector), &dwByte, nullptr);
		ReadFile(hFile, &vLook, sizeof(_vector), &dwByte, nullptr);

		CEnvironment::ENVIRONMENT_DESC			Desc{};
		Desc.eID = LEVEL_YARD;
		Desc.fPosition = fPos;
		Desc.iModelComponentIndex = iModelIndex;
		Desc.fScale = fScale;
		Desc.pPlayer = m_pPlayer;
		//cout << fScale.x << "     " << fScale.y << "            " << fScale.z << endl;
		CGameObject* pGameObj = (m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, TEXT("Layer_Environment"),
			TEXT("Prototype_GameObject_Environment_ImGui"), &Desc));
		if (pGameObj != nullptr)
		{
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Scale(0.f, fScale.x, fScale.y, fScale.z);
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Rotaion(vRight, vUp, vLook);
		}
	}
	CloseHandle(hFile);

	hFile = CreateFile(L"../Bin/Data/Build_Yard.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Build_Yard File Failed", L"Error", MB_OK);
		return;
	}

	while (ReadFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr) && dwByte > 0)
	{

		ReadFile(hFile, &iModelIndex, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &fScale, sizeof(_float3), &dwByte, nullptr);

		ReadFile(hFile, &fCollisionBoxScale, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &iImGuiMode, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &vecCollisionPos, sizeof(_vector), &dwByte, nullptr);

		ReadFile(hFile, &vRight, sizeof(_vector), &dwByte, nullptr);
		ReadFile(hFile, &vUp, sizeof(_vector), &dwByte, nullptr);
		ReadFile(hFile, &vLook, sizeof(_vector), &dwByte, nullptr);

		CEnvironment::ENVIRONMENT_DESC			Desc{};
		Desc.eID = LEVEL_YARD;
		Desc.fPosition = fPos;
		Desc.iModelComponentIndex = iModelIndex;
		Desc.fScale = fScale;
		Desc.pPlayer = m_pPlayer;
		//cout << fScale.x << "     " << fScale.y << "            " << fScale.z << endl;
		CGameObject* pGameObj = (m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, TEXT("Layer_Environment"),
			TEXT("Prototype_GameObject_Environment_ImGui"), &Desc));
		if (pGameObj != nullptr)
		{
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Scale(0.f, fScale.x, fScale.y, fScale.z);
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Rotaion(vRight, vUp, vLook);
		}
	}

	CloseHandle(hFile);

	hFile = CreateFile(L"../Bin/Data/Coin_Yard.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Coin_Yard File Failed", L"Error", MB_OK);
		return;
	}

	while (ReadFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr) && dwByte > 0)
	{
		ReadFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &fScale, sizeof(_float3), &dwByte, nullptr);

		fScale = { 15.f, 15.f, 15.f };
		fPos.y = 3.f;
		CCoin::COIN_DESC			Desc{};
		Desc.eID = LEVEL_YARD;
		Desc.fPosition = fPos;
		Desc.fScale = fScale;
		//cout << fScale.x << "     " << fScale.y << "            " << fScale.z << endl;
		CGameObject* pGameObj = (m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, TEXT("Layer_Coin"),
			TEXT("Prototype_GameObject_Coin"), &Desc));
		if (pGameObj != nullptr)
		{
			dynamic_cast<CCoin*>(pGameObj)->Set_Scale(0.f, fScale.x, fScale.y, fScale.z);
			dynamic_cast<CCoin*>(pGameObj)->MovePos(fPos.x, fPos.y, fPos.z);
		}
	}

	CloseHandle(hFile);
}

void CLevel_Yard::Build_Check()
{
	_bool bBuildCheck = false;
	for (auto& pBuild : m_vecTrapMark)
	{
		if (pBuild->Get_BuildAble() == true)
		{
			m_pPlayer->Set_Build_Able(true);
			bBuildCheck = true;
		}
		if (bBuildCheck == false)
		{
			m_pPlayer->Set_Build_Able(false);
		}
	}
}

void CLevel_Yard::RoundMgr_And_MonsterSpawn(_float fTimeDelta)
{
	if (m_iPreviousRound == 3 && m_iCurrentRound == 0)
	{
		m_bVictory = true;
		m_pEnding->Set_RoundEnd(true);
	}
	m_iPreviousRound = m_iCurrentRound;
	// 라운드 업데이트, 0은 쉬는 시간, 1 2 3 이 라운드 
	m_pGameInstance->Update_Round(fTimeDelta, m_iCurrentRound, *m_pPlayer->Get_BuildMode(), pNearMonsterLayer, pFarMonsterLayer, m_bRoundStart, m_fSkipTimer);
	if (m_iPreviousRound != m_iCurrentRound && m_iCurrentRound == 0)
	{
		// 빌드 모드 시작 (쉬는 시간 시작)
		for (auto pMark : m_vecTrapMark)
		{
			// 만든 레고 브릭이 부숴졌을 때 다시 만들 수 있게 값 초기화
			if (pMark->Get_Bricks_KnockDown() == true)
			{
				pMark->Set_ReBuild();
			}
		}
	}
	if (m_bRoundStart == true && m_iCurrentRound < 4 && m_iCurrentRound != 0)
	{		// 배열은 0부터 시작이라 1 빼줌 
		m_pRound[m_iCurrentRound - 1]->Set_CurrentRound(m_iCurrentRound);
		m_pRound[m_iCurrentRound - 1]->Update(fTimeDelta);
		//if (m_bOnce == false) // 한 번만 찾으면 된다
		//{
		//	// 몬스터 레이어 찾기 ( Initialize에서는 아직 몬스터 생성이 안되었기 때문에 여기서 찾아야한다.)
		//	pNearMonsterLayer = m_pGameInstance->Find_Layer(LEVEL_YARD, TEXT("Layer_Monster_Attack_Near"));
		//	pFarMonsterLayer = m_pGameInstance->Find_Layer(LEVEL_YARD, TEXT("Layer_Monster_Attack_Far"));
		//	m_bOnce = true;
		//}
		if(pNearMonsterLayer == nullptr)
			pNearMonsterLayer = m_pGameInstance->Find_Layer(LEVEL_YARD, TEXT("Layer_Monster_Attack_Near"));
		if (pFarMonsterLayer == nullptr)
			pFarMonsterLayer = m_pGameInstance->Find_Layer(LEVEL_YARD, TEXT("Layer_Monster_Attack_Far"));

		if (pMonsterBullet == nullptr)
			pMonsterBullet = m_pGameInstance->Find_Layer(LEVEL_YARD, TEXT("MonsterBullet_Layer"));

		if (pNearMonsterLayer != nullptr && pFarMonsterLayer != nullptr)
		{
			// 남은 몬스터 수를 보내줌
			m_pRound[m_iCurrentRound - 1]->Set_RemainMonster_Count(pNearMonsterLayer->Get_GameObjectList_Size() + pFarMonsterLayer->Get_GameObjectList_Size());
		}
		else
			m_bOnce = false;
	}
	
}

CLevel_Yard* CLevel_Yard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Yard* pInstance = new CLevel_Yard(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Yard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Yard::Free()
{
	__super::Free();
	ShowCursor(true);
	Safe_Release(m_pRound[0]);
	Safe_Release(m_pRound[1]);
	Safe_Release(m_pRound[2]);
	
}
