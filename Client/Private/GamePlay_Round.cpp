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

HRESULT CGamePlay_Round::Initialize()
{
	// 여기서 벡터에 라운드에 따라서 몬스터 담아서 생성하면 될것도 같은데
	MONSTER_CREATE_DESC pDesc{};
	pDesc.fPos = _float3(519.786f, 0.f, 329.124f);
	pDesc.iCell_Idx = 323;
	pDesc.eModelIndex = ANIM_TANK;
	m_vecMonsterCreate.push_back(pDesc);

	
	pDesc.fPos = _float3(380.755f, 5.f, 300.710f);
	pDesc.eModelIndex = ANIM_HELICOPTER;
	m_vecMonsterCreate.push_back(pDesc);

	
	pDesc.fPos = _float3(400.f, 3.f, 300.710f);
	pDesc.eModelIndex = ANIM_ALIEN;
	m_vecMonsterCreate.push_back(pDesc);

	pDesc.fPos = _float3(410.f, 1.f, 310.710f);
	pDesc.eModelIndex = ANIM_PONY;
	m_vecMonsterCreate.push_back(pDesc);

	fRound_Time = 0.f;
	return S_OK;
}

void CGamePlay_Round::Update(_float fTimeDelta)
{

	if (fRound_Time <= 0.f)
	{
		CTank::TANK_DESC Tank_Desc{};
		CHelicopter::HELICOPTER_DESC Helicopter_Desc{};
		CAlien::ALIEN_DESC Alien_Desc{};
		CPony::PONY_DESC Pony_Desc{};
		for(auto pMonster : m_vecMonsterCreate)
		{
			ANIMMODEL_INDEX eModel = pMonster.eModelIndex;
			switch (eModel)
			{
			case Client::ANIM_HELICOPTER:
			{
				Helicopter_Desc.vecTargetPos = vecBrainPos;
				Helicopter_Desc.eID = LEVEL_GAMEPLAY;
				Helicopter_Desc.fPosition = (pMonster).fPos;
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster_Attack_Far"), TEXT("Prototype_GameObject_Helicopter"), &Helicopter_Desc);

				break;
			}
			case Client::ANIM_TANK:
			{
				Tank_Desc.vecTargetPos = vecBrainPos;
				Tank_Desc.m_pBuild = m_pBrain;
				Tank_Desc.eID = LEVEL_GAMEPLAY;
				Tank_Desc.fPosition = (pMonster).fPos;
				Tank_Desc.iCell_Idx = (pMonster).iCell_Idx;
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster_Attack_Far"), TEXT("Prototype_GameObject_Tank"), &Tank_Desc);

				break;
			}
			case Client::ANIM_ALIEN:
			{
				Alien_Desc.eID = LEVEL_GAMEPLAY;
				Alien_Desc.fPosition = (pMonster).fPos;
				Alien_Desc.vecTargetPos = vecBrainPos;
				Alien_Desc.matBrainCoreWorld = matBrainCoreWorld;
				Alien_Desc.matPlayerWorld = matPlayerWorld;
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster_Attack_Near"), TEXT("Prototype_GameObject_Alien"), &Alien_Desc);
				break;
			}

			case Client::ANIM_PONY:
				Pony_Desc.eID = LEVEL_GAMEPLAY;
				Pony_Desc.fPosition = (pMonster).fPos;
				Pony_Desc.vecTargetPos = vecBrainPos;
				Pony_Desc.matBrainCoreWorld = matBrainCoreWorld;
				Pony_Desc.matPlayerWorld = matPlayerWorld;
				m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster_Attack_Near"), TEXT("Prototype_GameObject_Pony"), &Pony_Desc);

				break;

			default:
				break;
			}
		}
	}
	// 현재 라운드의 시간만 증가됨
	fRound_Time += fTimeDelta;

	
}

CGamePlay_Round* CGamePlay_Round::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGamePlay_Round* pInstance = new CGamePlay_Round(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
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
