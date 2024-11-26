#include "stdafx.h"
#include "..\Public\Yard_Round.h"
#include "Tank.h"
#include "Helicopter.h"
#include "Alien.h"
#include "Pony.h"
#include "Blimp.h"
#include "RifleMan.h"


CYard_Round::CYard_Round(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel_Yard{ pDevice, pContext }
{
}

HRESULT CYard_Round::Initialize(_uint iRound)
{
	// 여기서 벡터에 라운드에 따라서 몬스터 담아서 생성하면 될것도 같은데
	m_iMyRound = iRound;
	MONSTER_CREATE_FOR_YARD_DESC pDesc{};
	_ulong		dwByte = { 0 };
	_wstring strLast = TEXT(".dat");
	_wstring strPath = TEXT("../Bin/Data/Yard_Monster");
	_wstring Path_Result = strPath + to_wstring(iRound) + strLast;
	HANDLE		hFile = CreateFile(Path_Result.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Yard_Monster File Failed", L"Error", MB_OK);
		return E_FAIL;
	}

	while (ReadFile(hFile, &pDesc.eModelIndex, sizeof(_uint), &dwByte, nullptr) && dwByte > 0)
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

void CYard_Round::Update(_float fTimeDelta)
{
	// 지금 라운드와 이 객체의 라운드가 일치할 때 생성이 된다.
	if (m_iMyRound == m_iCurrentRound)
	{
		// 필드에 남은 몬스터가 5마리 밑이고 만들 수 있는 몬스터가 더 있을 때
		if (m_iCurrent_RemainMonster < 5 && m_iMonsterCount > 0 && fRound_Time > 0.f)
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
		if (fRound_Time <= 0.f)
		{
			for (int i = 0; i < 3; i++)
			{
				MonsterCreate(fTimeDelta);
				m_iMonsterCount--;
			}
		}
	}

	// 현재 라운드의 시간만 증가됨
	fRound_Time += fTimeDelta;
}

void CYard_Round::MonsterCreate(_float fTimeDelta)
{
	CTank::TANK_DESC Tank_Desc{};
	CHelicopter::HELICOPTER_DESC Helicopter_Desc{};
	CAlien::ALIEN_DESC Alien_Desc{};
	CPony::PONY_DESC Pony_Desc{};
	CRifleMan::RIFLEMAN_DESC pRifleMan{};
	CBlimp::BLIMP_DESC pBlimp{};

	ANIMMODEL_INDEX eModel = m_vecMonsterCreate.front().eModelIndex;
	switch (eModel)
	{
	case Client::ANIM_HELICOPTER:
	{
		Helicopter_Desc.vecTargetPos = vecBrainPos;
		Helicopter_Desc.eID = LEVEL_YARD;
		Helicopter_Desc.fPosition = m_vecMonsterCreate.front().fPos;
		Helicopter_Desc.pBuild = m_pBrain;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, TEXT("Layer_Monster_Attack_Far"), TEXT("Prototype_GameObject_Helicopter"), &Helicopter_Desc);
		m_vecMonsterCreate.erase(m_vecMonsterCreate.begin());
		break;
	}
	case Client::ANIM_TANK:
	{
		Tank_Desc.vecTargetPos = vecBrainPos;
		Tank_Desc.m_pBuild = m_pBrain;
		Tank_Desc.iBraincore_CellNumber = 563;
		Tank_Desc.eID = LEVEL_YARD;
		Tank_Desc.fPosition = m_vecMonsterCreate.front().fPos;
		Tank_Desc.iCell_Idx = m_vecMonsterCreate.front().iCell_Idx;
		Tank_Desc.pTrapLayer = m_pTrapLeyer;
		Tank_Desc.pPlayer = m_pPlayer;
		Tank_Desc.pCamera = m_pCamera;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, TEXT("Layer_Monster_Attack_Far"), TEXT("Prototype_GameObject_Tank"), &Tank_Desc);
		m_vecMonsterCreate.erase(m_vecMonsterCreate.begin());
		break;
	}
	case Client::ANIM_ALIEN:
	{
		Alien_Desc.eID = LEVEL_YARD;
		Alien_Desc.fPosition = m_vecMonsterCreate.front().fPos;
		Alien_Desc.vecTargetPos = vecBrainPos;
		Alien_Desc.matBrainCoreWorld = matBrainCoreWorld;
		Alien_Desc.matPlayerWorld = matPlayerWorld;
		Alien_Desc.iCell_Idx = m_vecMonsterCreate.front().iCell_Idx;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, TEXT("Layer_Monster_Attack_Near"), TEXT("Prototype_GameObject_Alien"), &Alien_Desc);
		m_vecMonsterCreate.erase(m_vecMonsterCreate.begin());
		break;
	}
	case Client::ANIM_PONY:
	{
		Pony_Desc.eID = LEVEL_YARD;
		Pony_Desc.iBraincore_CellNumber = 563;
		Pony_Desc.fPosition = m_vecMonsterCreate.front().fPos;
		Pony_Desc.vecTargetPos = vecBrainPos;
		Pony_Desc.matBrainCoreWorld = matBrainCoreWorld;
		Pony_Desc.matPlayerWorld = matPlayerWorld;
		Pony_Desc.pPlayer = m_pPlayer;
		Pony_Desc.iCell_Idx = m_vecMonsterCreate.front().iCell_Idx;
		Pony_Desc.pTrapLayer = m_pTrapLeyer;
		Pony_Desc.m_pBuild = m_pBrain;

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, TEXT("Layer_Monster_Attack_Near"), TEXT("Prototype_GameObject_Pony"), &Pony_Desc);
		m_vecMonsterCreate.erase(m_vecMonsterCreate.begin());
		break;
	}
	case Client::ANIM_RIFLEMAN:
	{
		pRifleMan.fPosition = m_vecMonsterCreate.front().fPos;
		pRifleMan.iBraincore_CellNumber = 563;
		pRifleMan.vecTargetPos = vecBrainPos;
		pRifleMan.pTrapLayer = m_pTrapLeyer;
		pRifleMan.iCell_Idx = m_vecMonsterCreate.front().iCell_Idx;
		pRifleMan.pPlayer = m_pPlayer;
		pRifleMan.eID = LEVEL_YARD;
		pRifleMan.matPlayerWorld = matPlayerWorld;
		pRifleMan.matBrainCoreWorld = matBrainCoreWorld;
		pRifleMan.m_pBuild = m_pBrain;
		pRifleMan.pCamera = m_pCamera;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, TEXT("Layer_Monster_Attack_Far"), TEXT("Prototype_GameObject_RifleMan"), &pRifleMan);
		m_vecMonsterCreate.erase(m_vecMonsterCreate.begin());

		break;
	}

	case Client::ANIM_BLIMP:
	{
		pBlimp.fPosition = m_vecMonsterCreate.front().fPos;
		pBlimp.vecTargetPos = vecBrainPos;
		pBlimp.pTrapLayer = m_pTrapLeyer;
		pBlimp.iCell_Idx = m_vecMonsterCreate.front().iCell_Idx;
		pBlimp.pPlayer = m_pPlayer;
		pBlimp.eID = LEVEL_YARD;
		pBlimp.matPlayerWorld = matPlayerWorld;
		pBlimp.matBrainCoreWorld = matBrainCoreWorld;
		pBlimp.pBuild = m_pBrain;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_YARD, TEXT("Layer_Monster_Attack_Far"), TEXT("Prototype_GameObject_Blimp"), &pBlimp);
		m_vecMonsterCreate.erase(m_vecMonsterCreate.begin());

		break;
	}
	default:
		break;
	}
}

CYard_Round* CYard_Round::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iRound)
{
	CYard_Round* pInstance = new CYard_Round(pDevice, pContext);
	if (FAILED(pInstance->Initialize(iRound)))
	{
		MSG_BOX("Failed to Created : CYard_Round");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CYard_Round::Free()
{
	__super::Free();
	
}
