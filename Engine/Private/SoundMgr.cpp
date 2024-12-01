#include "..\Public\SoundMgr.h"

CSoundMgr::CSoundMgr(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}


HRESULT CSoundMgr::Ready_Sound()
{
	m_pSystem = nullptr;
	channelGroup = nullptr;
	m_fVolume = 0.f;
	nowBGM = L"";


	// 사운드를 담당하는 대표객체를 생성하는 함수
	result = System_Create(&m_pSystem, FMOD_VERSION);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	result = m_pSystem->init(512, FMOD_INIT_NORMAL, NULL);

	LoadSoundFile(L"YardBackGround.wav");
	LoadSoundFile(L"PlayLevelBack.wav");
	LoadSoundFile(L"KatanaBackGround.wav");
	LoadSoundFile(L"LogoBackGround.wav");
	LoadSoundFile(L"PlayLevelBack2.wav");

	// 미사일 트럭
	LoadSoundFile(L"MissiletTruckUp.wav");  // 크레인 올라감
	LoadSoundFile(L"FE_MissileTruck_WarningVoice.wav"); // 미사일 발사함을 알림
	LoadSoundFile(L"FE_MissileTruck_WarningSiren.wav");
	LoadSoundFile(L"FE_MissileTruck_RocketLaunch.wav"); // 발사 불꽃 소리
	LoadSoundFile(L"FE_MissileTruck_Idle_Loop_Short.wav"); // 시동
	LoadSoundFile(L"FE_Base_Destroyed_Junk_03.wav"); // 부서짐

	// 외계인
	LoadSoundFile(L"FE_Mothership_Flying_AlienVoice_Level_02_06.wav"); // 외계어 시부림
	LoadSoundFile(L"FE_Mothership_LittleSpinner_Death_03.wav"); // Dead
	LoadSoundFile(L"FE_Mothership_AlienVoice_TeethBite_05.wav"); // 외계아
	LoadSoundFile(L"FE_Mothership_LittleSpinner_Pain_12.wav"); // 외계인 맞음
	LoadSoundFile(L"FE_Diarama_Alienqueen.wav"); // 외계아
	LoadSoundFile(L"FE_Mothership_LittleSpinner_Pain_05.wav"); // Dead

	
	// 헬기
	LoadSoundFile(L"FE_NPC_BH60_Loop_HealthMid.wav"); // 프로펠러
	LoadSoundFile(L"FE_NPC_BH60_Metal_Break_04.wav"); // 부서짐
	LoadSoundFile(L"FE_NPC_BH60_MinigunR_Tail.wav"); // 총알 발사
	LoadSoundFile(L"FE_NPC_BH60_PilotSOS_06.wav");	//  추락할 때 메이데이
	LoadSoundFile(L"FE_NPC_ArmyMen_Cobra_Far.wav"); // 멀리서 날아가는소리
	LoadSoundFile(L"FE_Playground_PropellerAirplane.wav"); // 멀리서 날아가는소리

	
	// 라이플맨
	LoadSoundFile(L"FE_Grunt_Death_12.wav"); // 라이플맨 죽음
	LoadSoundFile(L"FE_Grunt_Pain_21.wav"); //  라이플맨 아픔

	// 탱크
	LoadSoundFile(L"FE_BigTank_Drive.wav"); // 탱크움직임
	LoadSoundFile(L"04_12_2016_ArmyMen_Tank_1333_Drive_Drive_Close_Short.wav"); // 탱크움직임

	LoadSoundFile(L"FE_BigTank_Explosion.wav"); // 탱크, 헬기 폭발
	LoadSoundFile(L"FE_Explosion_Small_Close_01.wav"); // 탱크 발사
	LoadSoundFile(L"FE_BigTank_Fire_Close_01.wav"); // 탱크 발사
	LoadSoundFile(L"FE_BigTank_Fire_Close_02.wav"); // 탱크 발사
	LoadSoundFile(L"04_12_2016_ArmyMen_Tank_1333_Fire_Expl_03.wav"); // 탱크 발사

	// 포니
	LoadSoundFile(L"FE_Ninja_Animal_Puma.wav"); // 포니 물기
	LoadSoundFile(L"FE_TRex_FS_Light_01.wav"); // 포니 걷기
	LoadSoundFile(L"FE_TRex_FS_Light_02.wav"); // 포니 걷기
	LoadSoundFile(L"RunAway.wav"); // 포니 걷기

	
	// Ui
	LoadSoundFile(L"fe_ui_unlock_06.wav");



	// PowerNode
	LoadSoundFile(L"FE_Turret_Activate.wav"); // 작동 시적
	LoadSoundFile(L"FE_Powernode_base_Mono.wav"); // 평상시 소리
	LoadSoundFile(L"FE_Radar_Play.wav"); // Rader Sound 
	
	// 브레인코아
	LoadSoundFile(L"FE_CoreBeeingAttacked.wav"); // 피 없음 경고
	LoadSoundFile(L"FE_Base_Braincore_Bubble_01.wav"); // 버블
	LoadSoundFile(L"FE_Base_Braincore_Alarm1.wav"); // 피 없음 경고


	// 빌드할 때	
	LoadSoundFile(L"FE_BuildCredits_Recieved_Cash_02.wav"); // 돈 나감
	LoadSoundFile(L"FE_Buildable_Trap_Destroy.wav"); // 부서짐
	LoadSoundFile(L"FE_Buildable_Barricades_Destroy.wav"); // 레고 부서짐
	LoadSoundFile(L"FE_Buildable_Barricades_Build_Complete.wav"); // 빌드 됨


	// KATANA
	LoadSoundFile(L"FE_Blade_Impact_Spark_01.wav"); // 전기 생성
	LoadSoundFile(L"FE_Buildable_Barricades_Build_Complete.wav"); // 빌드 됨
	LoadSoundFile(L"FE_Buildable_Barricades_Build_Complete.wav"); // 빌드 됨
	
	
	// 배터리
	LoadSoundFile(L"FE_Battery_Catch.wav");  // 줍기
	LoadSoundFile(L"FE_Battery_Drop_01.wav"); // 떨구기
	LoadSoundFile(L"FE_Battery_Drop_02.wav"); // 떨구기
	LoadSoundFile(L"FE_Battery_In_Powernode.wav"); // 배터리 넣기
	LoadSoundFile(L"FE_Battery_Out_Powernode.wav"); // 배터리 빼기
	LoadSoundFile(L"FE_Battery_Pickup.wav"); // 조용히 줍기


	// 총발사
	LoadSoundFile(L"FE_Soldier_ShockTrooper_Xenon_Bullet_Impact_01.wav"); // 레이저같은 총소리
	LoadSoundFile(L"FE_Soldier_Pistol_Fire_Far_03.wav"); // 조금 멀리서 한 발


	// 폭발
	LoadSoundFile(L"FE_SmallTank_Explosion_Close_03.wav"); // 바로 앞 작은 폭발
	LoadSoundFile(L"FE_Explosion_Medium_Close_01.wav"); // 바로 앞 작은 폭발2
	LoadSoundFile(L"FE_Explosion_Big_Close_01.wav"); // 바로 앞 큰 폭발
	LoadSoundFile(L"FE_MissileTruck_Explosion_Nuke.wav"); // 바로 앞 큰 폭발

	
	// 행동
	LoadSoundFile(L"FE_FX_Pickup_Coin.wav"); // 아이템 줍기
	LoadSoundFile(L"fx_pickuphealth.wav"); // 아이템 줍기
	LoadSoundFile(L"Coin.wav"); // 코인 줍기

	LoadSoundFile(L"FE_Footstep_Teddy_Foley_Reload.wav"); // 장전
	LoadSoundFile(L"FE_Footstep_Teddy_Walk_Marine_01.wav"); // 걷기
	LoadSoundFile(L"FE_Footstep_Teddy_Walk_Marine_02.wav"); // 걷기
	LoadSoundFile(L"FE_Footstep_Teddy_Walk_Marine_03.wav"); // 걷기
	LoadSoundFile(L"FE_Footstep_Teddy_Walk_Marine_04.wav"); // 걷기
	LoadSoundFile(L"FE_Footstep_Teddy_Walk_Marine_05.wav"); // 걷기
	LoadSoundFile(L"FE_Footstep_Teddy_Walk_Marine_06.wav"); // 걷기
	LoadSoundFile(L"FE_Footstep_Teddy_Walk_Marine_07.wav"); // 걷기
	LoadSoundFile(L"FE_Footstep_Teddy_Walk_Marine_08.wav"); // 걷기


	LoadSoundFile(L"FE_FS_Grass_01.wav"); // 잔디 걷기
	LoadSoundFile(L"FE_FS_Grass_02.wav"); // 걷기
	LoadSoundFile(L"FE_FS_Grass_03.wav"); // 걷기
	LoadSoundFile(L"FE_FS_Grass_04.wav"); // 걷기
	LoadSoundFile(L"FE_FS_Grass_05.wav"); // 걷기
	LoadSoundFile(L"FE_FS_Grass_06.wav"); // 걷기
	LoadSoundFile(L"FE_FS_Grass_07.wav"); // 걷기
	LoadSoundFile(L"FE_FS_Grass_08.wav"); // 걷기
	LoadSoundFile(L"FE_Player_Landing_Low_Grass.wav"); // 잔디받 찾기


	LoadSoundFile(L"FE_Player_Melee_Punch_Miss_02_extra.wav"); // 펀치

	// 목소리

	LoadSoundFile(L"FE_VO_Blaze_Coredefend_01.wav"); // 코어 지켜
	LoadSoundFile(L"FE_VO_Blaze_Doublejump_01.wav"); // 더블 점프
	LoadSoundFile(L"FE_VO_Blaze_Jump_01.wav"); // 점프
	LoadSoundFile(L"FE_VO_Blaze_Enemyspotted_01.wav"); // 적 생성
	LoadSoundFile(L"FE_VO_Blaze_Melee_03.wav"); // 펀치
	LoadSoundFile(L"FE_VO_Blaze_Needcredits_01.wav"); //코인이 필요해
	LoadSoundFile(L"FE_VO_Blaze_Needdefences_01.wav"); // 방어물을 만들어야해
	LoadSoundFile(L"FE_VO_Blaze_Needtraps_01.wav"); // 트랩이 필요해

	LoadSoundFile(L"FE_VO_Blaze_Nicekill_01.wav"); // 잘 죽였어
	LoadSoundFile(L"FE_VO_Blaze_Powerneeded_01.wav"); // 에너지 코어 파워가 필요해

	LoadSoundFile(L"Sword1.wav"); // katana1
	LoadSoundFile(L"Sword2.wav"); // katana2


	// 튜토리얼
	LoadSoundFile(L"FE_VO_Blaze_Compliment_01.wav"); // 웰던
	LoadSoundFile(L"FE_VO_Blaze_Pain_03.wav"); // 아파
	LoadSoundFile(L"FE_VO_Blaze_Pickuphere_01.wav"); // 주워 저거

	
    return S_OK;
}

void CSoundMgr::PlaySoundW(const wstring pSoundKey, CHANNELID eID, float fVolume)
{
	auto iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](const auto& iter) -> bool
		{
			return lstrcmpW(pSoundKey.c_str(), iter.first.c_str()) == 0;
		});

	if (iter == m_mapSound.end())
		return;

	bool bPlay = false;

	if (m_pChannelArr[eID]->isPlaying(&bPlay))
	{
		result = m_pSystem->playSound(iter->second, nullptr, false, &m_pChannelArr[eID]);
	}

	if (fVolume != 0)
		m_pChannelArr[eID]->setVolume(fVolume);

	m_pSystem->update();
}

void CSoundMgr::PlayBGM(const wstring pSoundKey, float fVolume)
{
	auto iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](const auto& iter) -> bool
		{
			return lstrcmpW(pSoundKey.c_str(), iter.first.c_str()) == 0;
		});

	if (iter == m_mapSound.end())
		return;

	result = m_pSystem->playSound(iter->second, nullptr, false, &m_pChannelArr[SOUND_BGM]);
	m_pChannelArr[SOUND_BGM]->setMode(FMOD_LOOP_NORMAL);

	if (fVolume != 0)
		m_pChannelArr[SOUND_BGM]->setVolume(fVolume);

	nowBGM = pSoundKey;
}

void CSoundMgr::StopSound(CHANNELID eID)
{
	m_pChannelArr[eID]->stop();

}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		m_pChannelArr[i]->stop();
}

void CSoundMgr::SetChannelVolume(CHANNELID eID, float fVolume)
{
	m_pChannelArr[eID]->setVolume(fVolume);
	m_pSystem->update();
}

void CSoundMgr::VolumeFade(bool _bOnOff, float _fMinusVolume, float _fPlusVolume)
{
	if (!_bOnOff) //off == false;
	{
		//fVolume -= 0.03f;
		m_fVolume -= _fMinusVolume;
		if (m_fVolume <= 0)
		{
			m_fVolume = 0;
			StopSound(SOUND_BGM);
		}
		SetChannelVolume(SOUND_BGM, m_fVolume);
	}
	else //on == true
	{
		//fVolume += 0.10f;
		m_fVolume += _fPlusVolume;
		if (m_fVolume >= VOLUME_BGM) m_fVolume = VOLUME_BGM;
		SetChannelVolume(SOUND_BGM, m_fVolume);
	}
}

void CSoundMgr::VolumeFade_boss()
{
	m_fVolume -= 0.001f;
	if (m_fVolume <= 0)
	{
		m_fVolume = 0;
		StopSound(SOUND_BGM);
	}
	SetChannelVolume(SOUND_BGM, m_fVolume);
}

void CSoundMgr::LoadSoundFile(const wstring soundFile)
{
	char szCurPath[128] = "../Bin/Sound/"; // 상대 경로
	char szFullPath[256] = "";             // 전체 경로를 저장할 배열

	strcpy_s(szFullPath, szCurPath);       // 상대 경로를 전체 경로 배열에 복사

	// wstring을 string으로 변환
	int len = WideCharToMultiByte(CP_ACP, 0, soundFile.c_str(), -1, szFullPath + strlen(szCurPath), static_cast<int>(sizeof(szFullPath) - strlen(szCurPath)), NULL, NULL);

	// "../Bin/Sound/" + "Success.wav" 처럼 결합됨

	FMOD::Sound* pSound = nullptr;
	FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_DEFAULT, 0, &pSound);

	if (eRes == FMOD_OK)
	{
		m_mapSound.emplace(soundFile, pSound); // 변환된 문자열을 사운드 키로 사용
	}

	m_pSystem->update();
}

CSoundMgr* CSoundMgr::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSoundMgr* pInstance = new CSoundMgr(pDevice, pContext);

	if (FAILED(pInstance->Ready_Sound()))
	{
		MSG_BOX("Failed to Created : CSoundMgr");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSoundMgr::Free()
{
	__super::Free();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	for (auto& Mypair : m_mapSound)
	{
		Mypair.second->release();
	}
	m_mapSound.clear();
	channelGroup->release();
	m_pSystem->release();

}
