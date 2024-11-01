#include "stdafx.h"
#include "..\Public\GamePlay_Round.h"
#include "Tank.h"
#include "Helicopter.h"
#include "Alien.h"
#include "Pony.h"

CGamePlay_Round::CGamePlay_Round(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel_GamePlay{ pDevice, pContext }
{
}

HRESULT CGamePlay_Round::Initialize( _uint iRound)
{
	// 여기서 벡터에 라운드에 따라서 몬스터 담아서 생성하면 될것도 같은데
	m_iMyRound = iRound;
	MONSTER_CREATE_DESC pDesc{};
	_ulong		dwByte = { 0 };
	_wstring strLast = TEXT(".dat");
	_wstring strPath = TEXT("../Bin/Data/Gameplay_Monster");
	_wstring Path_Result = strPath + to_wstring(iRound) + strLast;
	HANDLE		hFile = CreateFile(Path_Result.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Monster File Failed", L"Error", MB_OK);
		return E_FAIL;
	}

	while(ReadFile(hFile, &pDesc.eModelIndex, sizeof(_uint), &dwByte, nullptr) && dwByte > 0)
	{
		ReadFile(hFile, &pDesc.fPos, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &pDesc.iCell_Idx, sizeof(_uint), &dwByte, nullptr);
		m_vecMonsterCreate.push_back(pDesc);
		m_iMonsterCount++;
	}
	
	fRound_Time = 0.f;
	fCreate_Time = 0.f;
	return S_OK;
}

void CGamePlay_Round::Update(_float fTimeDelta)
{
	// 지금 라운드와 이 객체의 라운드가 일치할 때 생성이 된다.
	if (m_iMyRound == m_iCurrentRound)
	{
		// 필드에 남은 몬스터가 2마리 밑이고 만들 수 있는 몬스터가 더 있을 때
		if (m_iCurrent_RemainMonster < 3 && m_iMonsterCount > 0 && fRound_Time > 0.f)
		{
			if (fCreate_Time >= 2.f) // 2초마다 1마리씩 생성
			{
				MonsterCreate(fTimeDelta);
				m_iMonsterCount--;
				fCreate_Time = 0.f;
			}
			fCreate_Time += fTimeDelta;
		}
		else
			fCreate_Time = 0.f;


		// 처음 시작할 때 3마리 생성
 		if(fRound_Time <= 0.f)
		{
			for(int i = 0; i < 3; i++)
			{
			
				MonsterCreate(fTimeDelta);
				m_iMonsterCount--;
			}
		}
		
	}


	// 현재 라운드의 시간만 증가됨
	fRound_Time += fTimeDelta;
	m_iCurrent_RemainMonster; // 지금 남은 몬스터
	m_iMonsterCount; // 이번 라운드에서 생성할 몬스터 수
}

void CGamePlay_Round::MonsterCreate(_float fTimeDelta)
{
	CTank::TANK_DESC Tank_Desc{};
	CHelicopter::HELICOPTER_DESC Helicopter_Desc{};
	CAlien::ALIEN_DESC Alien_Desc{};
	CPony::PONY_DESC Pony_Desc{};

	ANIMMODEL_INDEX eModel = m_vecMonsterCreate.front().eModelIndex;
	switch (eModel)
	{
	case Client::ANIM_HELICOPTER:
	{
		Helicopter_Desc.vecTargetPos = vecBrainPos;
		Helicopter_Desc.eID = LEVEL_GAMEPLAY;
		Helicopter_Desc.fPosition = m_vecMonsterCreate.front().fPos;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster_Attack_Far"), TEXT("Prototype_GameObject_Helicopter"), &Helicopter_Desc);
		m_vecMonsterCreate.erase(m_vecMonsterCreate.begin());
		break;
	}
	case Client::ANIM_TANK:
	{
		Tank_Desc.vecTargetPos = vecBrainPos;
		Tank_Desc.m_pBuild = m_pBrain;
		Tank_Desc.eID = LEVEL_GAMEPLAY;
		Tank_Desc.fPosition = m_vecMonsterCreate.front().fPos;
		Tank_Desc.iCell_Idx = m_vecMonsterCreate.front().iCell_Idx;
		Tank_Desc.pTrapLayer = m_pTrapLeyer;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster_Attack_Far"), TEXT("Prototype_GameObject_Tank"), &Tank_Desc);
		m_vecMonsterCreate.erase(m_vecMonsterCreate.begin());
		break;
	}
	case Client::ANIM_ALIEN:
	{
		Alien_Desc.eID = LEVEL_GAMEPLAY;
		Alien_Desc.fPosition = m_vecMonsterCreate.front().fPos;
		Alien_Desc.vecTargetPos = vecBrainPos;
		Alien_Desc.matBrainCoreWorld = matBrainCoreWorld;
		Alien_Desc.matPlayerWorld = matPlayerWorld;
		Alien_Desc.iCell_Idx = m_vecMonsterCreate.front().iCell_Idx;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster_Attack_Near"), TEXT("Prototype_GameObject_Alien"), &Alien_Desc);
		m_vecMonsterCreate.erase(m_vecMonsterCreate.begin());
		break;
	}
	case Client::ANIM_PONY:
	{
		Pony_Desc.eID = LEVEL_GAMEPLAY;
		Pony_Desc.fPosition = m_vecMonsterCreate.front().fPos;
		Pony_Desc.vecTargetPos = vecBrainPos;
		Pony_Desc.matBrainCoreWorld = matBrainCoreWorld;
		Pony_Desc.matPlayerWorld = matPlayerWorld;
		Pony_Desc.pPlayer = m_pPlayer;
		Pony_Desc.iCell_Idx = m_vecMonsterCreate.front().iCell_Idx;
		Pony_Desc.pTrapLayer = m_pTrapLeyer;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster_Attack_Near"), TEXT("Prototype_GameObject_Pony"), &Pony_Desc);
		m_vecMonsterCreate.erase(m_vecMonsterCreate.begin());
		break;
	}
	default:
		break;
	}
}

CGamePlay_Round* CGamePlay_Round::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iRound)
{
	CGamePlay_Round* pInstance = new CGamePlay_Round(pDevice, pContext);
	if (FAILED(pInstance->Initialize(iRound)))
	{
		MSG_BOX("Failed to Created : CGamePlay_Round");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGamePlay_Round::Free()
{
	__super::Free();
}
