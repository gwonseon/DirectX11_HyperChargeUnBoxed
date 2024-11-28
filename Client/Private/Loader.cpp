#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"

#include "BackGround.h"
#include "CrossLine.h"
#include "InGameUI.h"
#include "MenuUI.h"
#include "ButtonUI.h"
#include "NumberUI.h"
#include "UI_3D.h"


#include "Terrain.h"
#include "Camera_Free.h"

#include "Tank.h"
#include "Helicopter.h"
#include "Alien.h"
#include "Pony.h"
#include "RifleMan.h"
#include "Blimp.h"
#include "Missile_Truck.h"
#include "TruckBody.h"
#include "TruckShooter.h"
#include "Tracker.h"
#include "Dead_Model.h"

#include "Environment.h"
#include "BrainCore.h"
#include "Energy_Machine.h"
#include "Energy_Lader.h"
#include "Energy_Cap.h"


#include "Trap_Marks.h"
#include "Trap_Bricks.h"
#include "Broken_Bricks.h"

#include "Coin.h"
#include "Battery.h"
#include "Coin_Item.h"
#include "Hp_Item.h"
#include "Collector.h"

#include "Bullet.h"
#include "Monster_Bullet.h"

#include "Head_Player.h"
#include "Body_Player.h"
#include "Weapon.h"
#include "Player.h"
#include "Pivot.h"
#include "CollisionBox.h"
#include "UI_CircleGuage.h"
#include "FPS_Pivot.h"

#include "Sky.h"

#include "Particle_Explosion.h"
#include "Particle_Snow.h"
#include "Grass_Instancing.h"
#include "Truck_Missile.h"


#include "Explosion.h"
#include "Effect_Explosion.h"
#include "Effect_Flare_Rifle.h"
#include "Effect_Explosion_Tank.h"
#include "Terrain_Crushed.h"
#include "Slash_Mesh.h"
#include "Rader_Effect.h"
#include "Effect_Electricity.h"
#include "Missile_Flame.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

/* 자원을 로드한다. (서브 스레드)*/
_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return 1;

	return 0;
}

HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	/* 서브스레드를 생성한다( 메인스렏)*/

	/* unsigned (__stdcall* _beginthreadex_proc_type)(void*); */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;
	
	return S_OK;
}

// (서브 스레드)
HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT		hr = { 0 };
	
	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_For_LogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlayLevel();
		break;
	case LEVEL_YARD:
		hr = Loading_For_GameYardLevel();
		break;
	case LEVEL_IMGUI:
		hr = Loading_For_ImGuiLevel();
		break;
	case LEVEL_NAVIGATION:
		hr = Loading_For_NavigationLevel();
		break;
	case LEVEL_MONSTERSPAWN:
		hr = Loading_For_MonsterSpawnLevel();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_CriticalSection);

	return S_OK;
}

#ifdef _DEBUG

void CLoader::Output_LoadingState()
{
	SetWindowText(g_hWnd, m_strLoadingText.c_str());
}

#endif


HRESULT CLoader::Loading_For_LogoLevel()
{
	m_strLoadingText = TEXT("텍스쳐 로딩중입니다.");
	/* For.Prototype_Component_Texture_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_GameTitle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/LogoLevel/T_U_HyperchargeLogoBase.png")))))
		return E_FAIL;
	// 뒷배경
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Menu_Back"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/LogoLevel/T_U_BackgroundStats_Background.png")))))
		return E_FAIL;

	// UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Button0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MenuUI/UI%d.png"),2))))
		return E_FAIL;


	m_fPersent += 20.f;
	m_strLoadingText = TEXT("모델 로딩중입니다.");

	m_fPersent += 20.f;
	m_strLoadingText = TEXT("셰이더 로딩중입니다.");

	m_fPersent += 20.f;
	m_strLoadingText = TEXT("객체원형 로딩중입니다.");
	// 뒷배경
	if(m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_BackGround_Menu")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround_Menu"),
			CBackGround::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// UI
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_ButtonUI_Menu")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ButtonUI_Menu"),
			CButtonUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	
	// Title
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_GameTitle")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GameTitle"),
			CMenuUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}


	m_fPersent += 20.f;
	m_strLoadingText = TEXT("로딩 완료되었습니다.");
	m_fPersent += 20.f;
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLevel()
{
	m_strLoadingText = TEXT("텍스쳐 로딩중입니다.");

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;
	Loading_Effect(LEVEL_GAMEPLAY);

#pragma region UI텍스처 생성
	// 크로스 라인
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Logo2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/CrossLine/CrossLine%d.png"), 24))))
		return E_FAIL;

	// LButton
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_LButton"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/LButton.dds")))))
		return E_FAIL;

	// RButton
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RButton"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/RButton.dds")))))
		return E_FAIL;

	// V_Icon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_VIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/VKey_ICon.dds")))))
		return E_FAIL;
	// F_Icon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/FKey_ICon.dds")))))
		return E_FAIL;
	// C_Icon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/CKey_ICon.dds")))))
		return E_FAIL;

	// Shift
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Shift"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/ShiftUI.dds")))))
		return E_FAIL;

	// Space
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Space.dds")))))
		return E_FAIL;

	// EnergyIcon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_EnergyIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/EnergyIcon.dds")))))
		return E_FAIL;

	// HPIcon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HpIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/HP_ICon.dds")))))
		return E_FAIL;

	// CreditIcon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CreditIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Credit_Icon.dds")))))
		return E_FAIL;

	// RunIcon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RunIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Run_Icon.dds")))))
		return E_FAIL;

	// JumpIcon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_JumpIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Jump_Icon.dds")))))
		return E_FAIL;

	// ModeChangeIcon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ModeChangeIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/ModeChange_Icon%d.dds"),2))))
		return E_FAIL;

	// Punch_Icon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PuchIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Punch_Icon.dds")))))
		return E_FAIL;

	// ViewChange_Icon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ViewChangeIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/View_Icon.dds")))))
		return E_FAIL;

	// Death
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Death"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Death.dds")))))
		return E_FAIL;

	// Battery
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Battery"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Battery%d.dds"),5))))
		return E_FAIL;

	// Character_UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Character"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Character/UI_Character%d.png"), 12))))
		return E_FAIL;

	// UI_BackGround 글씨 띄우는거 뒷 배경
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UIBackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/UI_BackGround%d.png"), 2))))
		return E_FAIL;
	
	// UI_Bar  체력 에너지 배터리 등등
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UIBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/UI_Bar%d.dds"), 5))))
		return E_FAIL;

	// 데미지 입었을 때 배경
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UIDamaged"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/DamageIndicator.png")))))
		return E_FAIL;
	
	//CircleGuage
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CircleGuage"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/CircleGuage.png")))))
		return E_FAIL;	
	
	//CenterUI 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CenterUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/CenterUI%d.png"),3))))
		return E_FAIL;

	//Slice
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Slice"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Slice.dds")))))
		return E_FAIL;

	// Number 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Number"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/%d.dds"), 10))))
		return E_FAIL;


#pragma endregion UI텍스처 생성

	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("모델 로딩중입니다.");


	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Loading_DataFile_For_GameLevel();


	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("셰이더 로딩중입니다.");

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;


	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("객체원형 로딩중입니다.");

		// 슬래쉬
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Slash")) == nullptr)
	{
		/* 부서짐 */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Slash"),
			CSlash_Mesh::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	/* Prototype_GameObject_Sky */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Sky")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
			CSky::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	
	// 크로스 라인
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_CrossLine")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CrossLine"),
			CCrossLine::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	//UI
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_UI")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"),
			CInGameUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	//UI_circle
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Circle_UI")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Circle_UI"),
			CUI_CircleGuage::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	//UINumber
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_UINumber")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UINumber"),
			CNumberUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 터레인
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Terrain")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
			CTerrain::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	/* Prototype_GameObject_Camera_Free */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Camera_Free")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
			CCamera_Free::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// 플레이어
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Player")) == nullptr)
	{
		/* Prototype GameObject Player*/
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
			CPlayer::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// 플레이어 몸
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Body_Player")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),
			CBody_Player::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 플레이어 머리
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Head_Player")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Head_Player"),
			CHead_Player::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 플레이어 TPS 피봇
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Pivot")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pivot"),
			CPivot::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// 플레이어 FPS vlqht
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_FPSPivot")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FPSPivot"),
			CFPS_Pivot::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 플레이어 무기
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Weapon")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"),
			CWeapon::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 플레이어 칼
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Katana")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Katana"),
			CWeapon_Katana::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 무기 아이템
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_WeaponItem")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WeaponItem"),
			CWeapon_Item::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 코인
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Coin")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Coin"),
			CCoin::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// Battery 
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Battery")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Battery"),
			CBattery::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// CoinItem 
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_CoinItem")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CoinItem"),
			CCoin_Item::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// HpItem 
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_HpItem")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HpItem"),
			CHp_Item::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	/* Tank */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Tank")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tank"),
			CTank::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	
	/* Heli */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Helicopter")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Helicopter"),
			CHelicopter::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	/* Alien */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Alien")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Alien"),
			CAlien::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}


	/* PONY */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Pony")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pony"),
			CPony::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	/* RIFLEMAN */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_RifleMan")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RifleMan"),
			CRifleMan::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	
	/* Blimp */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Blimp")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Blimp"),
			CBlimp::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	
	// Environment
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Environment_ImGui")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment_ImGui"),
			CEnvironment::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// BrainCore
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_BrainCore")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BrainCore"),
			CBrainCore::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// EnergyMachine
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_EnergyMachine")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnergyMachine"),
			CEnergy_Machine::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// EnergyMachine Lader
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_EnergyLader")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnergyLader"),
			CEnergy_Lader::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// EnergyMachine Lader
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_EnergyCap")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnergyCap"),
			CEnergy_Cap::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 트랩 마크
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_TrapMarks")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TrapMarks"),
			CTrap_Marks::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 트랩 레고 벽돌
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_TrapBricks")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TrapBricks"),
			CTrap_Bricks::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 레고 부서짐
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_broken")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_broken"),
			CBroken_Bricks::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	//Bullet
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Bullet")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet"),
			CBullet::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	//MonsterBullet
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_MonsterBullet")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterBullet"),
			CMonster_Bullet::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}


	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_Component_Collider_AABB")) == nullptr)
	{
		/* For.Prototype_Component_Collider_AABB */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
			return E_FAIL;
	}
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_Component_Collider_OBB")) == nullptr)
	{
		/* For.Prototype_Component_Collider_OBB */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_Component_Collider_Sphere")) == nullptr)
	{
		/* For.Prototype_Component_Collider_Sphere */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
			return E_FAIL;
	}



	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("로딩 완료되었습니다.");
	m_fPersent += 20.f;
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GameYardLevel()
{
	m_strLoadingText = TEXT("텍스쳐 로딩중입니다.");


	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png")))))
		return E_FAIL;


	Loading_Effect(LEVEL_YARD);
#pragma region UI텍스처 생성
	// Nuclear
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_Nuclear"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Nuclear.dds")))))
		return E_FAIL;

	// 크로스 라인
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_Logo2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/CrossLine/CrossLine%d.png"), 24))))
		return E_FAIL;

	// UI_MISSILE_TIMER
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_Missile_Timer"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/MISSILE_TIMER.dds")))))
		return E_FAIL;
	
	// LButton
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_LButton"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/LButton.dds")))))
		return E_FAIL;

	// RButton
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_RButton"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/RButton.dds")))))
		return E_FAIL;

	// V_Icon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_VIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/VKey_ICon.dds")))))
		return E_FAIL;
	// F_Icon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_FIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/FKey_ICon.dds")))))
		return E_FAIL;
	// C_Icon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_CIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/CKey_ICon.dds")))))
		return E_FAIL;

	// Shift
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_Shift"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/ShiftUI.dds")))))
		return E_FAIL;

	// Space
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_Space"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Space.dds")))))
		return E_FAIL;

	// EnergyIcon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_EnergyIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/EnergyIcon.dds")))))
		return E_FAIL;

	// HPIcon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_HpIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/HP_ICon.dds")))))
		return E_FAIL;

	// CreditIcon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_CreditIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Credit_Icon.dds")))))
		return E_FAIL;

	// RunIcon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_RunIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Run_Icon.dds")))))
		return E_FAIL;

	// JumpIcon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_JumpIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Jump_Icon.dds")))))
		return E_FAIL;

	// ModeChangeIcon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_ModeChangeIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/ModeChange_Icon%d.dds"), 2))))
		return E_FAIL;

	// Punch_Icon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_PuchIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Punch_Icon.dds")))))
		return E_FAIL;

	// ViewChange_Icon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_ViewChangeIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/View_Icon.dds")))))
		return E_FAIL;

	// Death
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_Death"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Death.dds")))))
		return E_FAIL;

	// Battery
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_Battery"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Battery%d.dds"), 5))))
		return E_FAIL;

	// Character_UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_Character"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Character/UI_Character%d.png"), 12))))
		return E_FAIL;

	// UI_BackGround 글씨 띄우는거 뒷 배경
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_UIBackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/UI_BackGround%d.png"), 2))))
		return E_FAIL;

	// UI_Bar  체력 에너지 배터리 등등
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_UIBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/UI_Bar%d.dds"), 5))))
		return E_FAIL;

	// 데미지 입었을 때 배경
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_UIDamaged"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/DamageIndicator.png")))))
		return E_FAIL;

	//CircleGuage
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_CircleGuage"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/CircleGuage.png")))))
		return E_FAIL;

	//CenterUI 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_CenterUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/CenterUI%d.png"), 3))))
		return E_FAIL;

	//Slice
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_Slice"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/Slice.dds")))))
		return E_FAIL;

	// Number 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Texture_Number"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/%d.dds"), 10))))
		return E_FAIL;


#pragma endregion UI텍스처 생성

	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("모델 로딩중입니다.");

	Loading_DataFile_For_YardLevel();
	Loading_DataFile_For_Instancing_YardLevel();

#pragma region 인스턴싱

	/* For.Prototype_Component_VIBuffer_Particle_Snow*/
	CVIBuffer_Instancing::INSTANCING_DESC		ParticleSnowDesc{};
	ParticleSnowDesc.iNumInstance = 3000;
	ParticleSnowDesc.vCenter = _float3(645.424f, 0.f, 559.107f);
	ParticleSnowDesc.vRange = _float3(128.f, 0.f, 128.f);
	ParticleSnowDesc.vSize = _float2(100.f, 100.f);
	ParticleSnowDesc.vSpeed = _float2(1.f, 7.f);
	ParticleSnowDesc.vLifeTime = _float2(3.f, 10.f);
	ParticleSnowDesc.isLoop = true;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_VIBuffer_Particle_Snow"),
		CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, &ParticleSnowDesc))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Particle_Explosion */
	CVIBuffer_Instancing::INSTANCING_DESC		ParticleExploDesc{};
	ParticleExploDesc.iNumInstance = 700;
	ParticleExploDesc.vCenter = _float3(645.424f, 0.f, 559.107f);
	ParticleExploDesc.vRange = _float3(4.f, 4.f, 4.f);
	ParticleExploDesc.vSize = _float2(1.01f, 1.1f);
	ParticleExploDesc.vSpeed = _float2(0.3f, 1.f);
	ParticleExploDesc.vLifeTime = _float2(0.1f, 0.5f);
	ParticleExploDesc.vPivot = _float3(0.f, -0.5f, 0.f);
	ParticleExploDesc.isLoop = true;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_VIBuffer_Particle_Explosion"),
		CVIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, &ParticleExploDesc))))
		return E_FAIL;

	// 잔디
	CVIBuffer_Instancing::INSTANCING_DESC	GrassInstancing{};
	GrassInstancing.iNumInstance = m_iGrass_Count[0];
	GrassInstancing.vCenter = _float3(645.424f, 0.f, 559.107f);
	GrassInstancing.vRange = _float3(128.f, 0.f, 128.f);
	GrassInstancing.vSize = _float2(8.f, 8.f);

	 _wstring Grass_Path = TEXT("../Bin/Resources/Model/ModelData_Build108.dat");
	_matrix			PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(100.f, 100.f, 100.f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_VIBuffer_Grass108"),
		CVIBuffer_Grass::Create(m_pDevice, m_pContext, Grass_Path, PreTransformMatrix, 0,m_vecGrassPos[0], &GrassInstancing))))
		return E_FAIL;

	GrassInstancing.iNumInstance = m_iGrass_Count[1];
	Grass_Path = TEXT("../Bin/Resources/Model/ModelData_Build109.dat");
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_VIBuffer_Grass109"),
		CVIBuffer_Grass::Create(m_pDevice, m_pContext, Grass_Path, PreTransformMatrix, 0, m_vecGrassPos[1], &GrassInstancing))))
		return E_FAIL;

	GrassInstancing.iNumInstance = m_iGrass_Count[2];
	Grass_Path = TEXT("../Bin/Resources/Model/ModelData_Build110.dat");
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_VIBuffer_Grass110"),
		CVIBuffer_Grass::Create(m_pDevice, m_pContext, Grass_Path, PreTransformMatrix, 0, m_vecGrassPos[2], &GrassInstancing))))
		return E_FAIL;

	GrassInstancing.iNumInstance = m_iGrass_Count[3];
	Grass_Path = TEXT("../Bin/Resources/Model/ModelData_Build111.dat");
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_VIBuffer_Grass111"),
		CVIBuffer_Grass::Create(m_pDevice, m_pContext, Grass_Path, PreTransformMatrix, 0, m_vecGrassPos[3], &GrassInstancing))))
		return E_FAIL;

#pragma endregion 인스턴싱




	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("셰이더 로딩중입니다.");

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxParticleRect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Shader_VtxParticleRect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxParticleRect.hlsl"), VTXPARTICLE_RECT::Elements, VTXPARTICLE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxParticlePoint */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Shader_VtxParticlePoint"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxParticlePoint.hlsl"), VTXPARTICLE_POINT::Elements, VTXPARTICLE_POINT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Particle_Mesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Shader_VtxParticleMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxParticleMesh.hlsl"), VTXPARTICLE_MESH::Elements, VTXPARTICLE_MESH::iNumElements))))
		return E_FAIL;



	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("객체원형 로딩중입니다.");
	

	// 땅 부서짐
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Effect_Terrain_Crushed")) == nullptr)
	{
		/* 부서짐 */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Terrain_Crushed"),
			CTerrain_Crushed::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// 슬래쉬
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Slash")) == nullptr)
	{
		/* 부서짐 */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Slash"),
			CSlash_Mesh::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// Collector Item
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Collect_Item")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Collect_Item"),
			CCollector::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// 3D UI
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_3DUI")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_3DUI"),
			CUI_3D::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Particle_Snow")) == nullptr)
	{
		/* Prototype_GameObject_Particle_Snow */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Snow"),
			CParticle_Snow::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Particle_Explosion")) == nullptr)
	{
		/* Prototype_GameObject_Particle_Explosion */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Explosion"),
			CParticle_Explosion::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}


	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Grass")) == nullptr)
	{
		/* Prototype_GameObject_Particle_Explosion */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Grass"),
			CGrass_Instancing::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	/* Explosion 포탄 폭발 콜라이더  */ 
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Explosion")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Explosion"),
			CExplosion::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	/* Prototype_GameObject_Sky */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Sky")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
			CSky::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}


	// 크로스 라인
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_CrossLine")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CrossLine"),
			CCrossLine::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	//UI
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_UI")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"),
			CInGameUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	//UI_circle
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Circle_UI")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Circle_UI"),
			CUI_CircleGuage::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	//UINumber
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_UINumber")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UINumber"),
			CNumberUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 터레인
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Terrain")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
			CTerrain::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	/* Prototype_GameObject_Camera_Free */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Camera_Free")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
			CCamera_Free::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// 플레이어
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Player")) == nullptr)
	{
		/* Prototype GameObject Player*/
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
			CPlayer::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// 플레이어 몸
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Body_Player")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),
			CBody_Player::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 플레이어 머리
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Head_Player")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Head_Player"),
			CHead_Player::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 플레이어 TPS 피봇
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Pivot")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pivot"),
			CPivot::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// 플레이어 FPS vlqht
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_FPSPivot")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FPSPivot"),
			CFPS_Pivot::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 플레이어 무기
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Weapon")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"),
			CWeapon::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 플레이어 칼
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Katana")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Katana"),
			CWeapon_Katana::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 무기 아이템
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_WeaponItem")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WeaponItem"),
			CWeapon_Item::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	
	// 코인
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Coin")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Coin"),
			CCoin::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// Battery 
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Battery")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Battery"),
			CBattery::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// CoinItem 
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_CoinItem")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CoinItem"),
			CCoin_Item::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// HpItem 
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_HpItem")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HpItem"),
			CHp_Item::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	
	/* Tank */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Tank")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tank"),
			CTank::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	/* Heli */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Helicopter")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Helicopter"),
			CHelicopter::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	/* Alien */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Alien")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Alien"),
			CAlien::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}


	/* PONY */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Pony")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pony"),
			CPony::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	/* RIFLEMAN */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_RifleMan")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RifleMan"),
			CRifleMan::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	/* Blimp */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Blimp")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Blimp"),
			CBlimp::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// 미사일 트럭
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_MissileTruck")) == nullptr)
	{
		/* Prototype GameObject Player*/
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MissileTruck"),
			CMissile_Truck::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// 미사일 트럭 몸
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_MissileTruck_Body")) == nullptr)
	{
		/* Prototype GameObject Player*/
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MissileTruck_Body"),
			CTruckBody::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// 미사일 트럭 발사대
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_MissileTruck_Shooter")) == nullptr)
	{
		/* Prototype GameObject Player*/
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MissileTruck_Shooter"),
			CTruckShooter::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// 미사일
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Missile")) == nullptr)
	{
		/* Prototype GameObject Player*/
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Missile"),
			CTruck_Missile::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	
	// 추적장치
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Tracker")) == nullptr)
	{
		/* Prototype GameObject Player*/
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tracker"),
			CTracker::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// Environment
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Environment_ImGui")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment_ImGui"),
			CEnvironment::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// BrainCore
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_BrainCore")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BrainCore"),
			CBrainCore::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// EnergyMachine
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_EnergyMachine")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnergyMachine"),
			CEnergy_Machine::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// EnergyMachine Lader
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_EnergyLader")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnergyLader"),
			CEnergy_Lader::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// EnergyMachine Lader
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_EnergyCap")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnergyCap"),
			CEnergy_Cap::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 트랩 마크
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_TrapMarks")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TrapMarks"),
			CTrap_Marks::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 트랩 레고 벽돌
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_TrapBricks")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TrapBricks"),
			CTrap_Bricks::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 레고 부서짐
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_broken")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_broken"),
			CBroken_Bricks::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	//Bullet
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Bullet")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet"),
			CBullet::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	//MonsterBullet
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_MonsterBullet")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterBullet"),
			CMonster_Bullet::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}


	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_Component_Collider_AABB")) == nullptr)
	{
		/* For.Prototype_Component_Collider_AABB */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Collider_AABB"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
			return E_FAIL;
	}
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_Component_Collider_OBB")) == nullptr)
	{
		/* For.Prototype_Component_Collider_OBB */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Collider_OBB"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_Component_Collider_Sphere")) == nullptr)
	{
		/* For.Prototype_Component_Collider_Sphere */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, TEXT("Prototype_Component_Collider_Sphere"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
			return E_FAIL;
	}



	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("로딩 완료되었습니다.");
	m_fPersent += 20.f;
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_ImGuiLevel()
{
	m_strLoadingText = TEXT("텍스쳐 로딩중입니다.");

	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("모델 로딩중입니다.");

	Loading_DataFile(LEVEL_IMGUI);
	Loading_DataFile_For_Instancing_ImGuiLevel();

	// 잔디
	CVIBuffer_Instancing::INSTANCING_DESC	GrassInstancing{};
	GrassInstancing.iNumInstance = m_iGrass_Count[0];
	GrassInstancing.vCenter = _float3(645.424f, 0.f, 559.107f);
	GrassInstancing.vRange = _float3(128.f, 0.f, 128.f);
	GrassInstancing.vSize = _float2(8.f, 8.f);

	_wstring Grass_Path = TEXT("../Bin/Resources/Model/ModelData_Build108.dat");
	_matrix			PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(100.f, 100.f, 100.f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_VIBuffer_Grass108"),
		CVIBuffer_Grass::Create(m_pDevice, m_pContext, Grass_Path, PreTransformMatrix, 0, m_vecGrassPos[0], &GrassInstancing))))
		return E_FAIL;

	GrassInstancing.iNumInstance = m_iGrass_Count[1];
	Grass_Path = TEXT("../Bin/Resources/Model/ModelData_Build109.dat");
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_VIBuffer_Grass109"),
		CVIBuffer_Grass::Create(m_pDevice, m_pContext, Grass_Path, PreTransformMatrix, 0, m_vecGrassPos[1], &GrassInstancing))))
		return E_FAIL;

	GrassInstancing.iNumInstance = m_iGrass_Count[2];
	Grass_Path = TEXT("../Bin/Resources/Model/ModelData_Build110.dat");
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_VIBuffer_Grass110"),
		CVIBuffer_Grass::Create(m_pDevice, m_pContext, Grass_Path, PreTransformMatrix, 0, m_vecGrassPos[2], &GrassInstancing))))
		return E_FAIL;

	GrassInstancing.iNumInstance = m_iGrass_Count[3];
	Grass_Path = TEXT("../Bin/Resources/Model/ModelData_Build111.dat");
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_VIBuffer_Grass111"),
		CVIBuffer_Grass::Create(m_pDevice, m_pContext, Grass_Path, PreTransformMatrix, 0, m_vecGrassPos[3], &GrassInstancing))))
		return E_FAIL;

	//----------------------------------------------------------------------------------------------------
	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("셰이더 로딩중입니다.");

	/* For.Prototype_Component_VIBuffer_Particle_Mesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Shader_VtxParticleMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxParticleMesh.hlsl"), VTXPARTICLE_MESH::Elements, VTXPARTICLE_MESH::iNumElements))))
		return E_FAIL;

	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("객체원형 로딩중입니다.");


	// BrainCore
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_BrainCore")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BrainCore"),
			CBrainCore::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// EnergyMachine
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_EnergyMachine")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnergyMachine"),
			CEnergy_Machine::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// EnergyMachine Lader
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_EnergyLader")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnergyLader"),
			CEnergy_Lader::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// EnergyMachine Lader
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_EnergyCap")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnergyCap"),
			CEnergy_Cap::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 트랩 마크
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_TrapMarks")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TrapMarks"),
			CTrap_Marks::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 트랩 레고 벽돌
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_TrapBricks")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TrapBricks"),
			CTrap_Bricks::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 레고 부서짐
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_broken")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_broken"),
			CBroken_Bricks::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Grass")) == nullptr)
	{
		/* Prototype_GameObject_Particle_Explosion */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Grass"),
			CGrass_Instancing::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// 터레인
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Terrain_ImGui")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain_ImGui"),
			CTerrain::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	/* Prototype_GameObject_Camera_Free */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Camera_Free_ImGui")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free_ImGui"),
			CCamera_Free::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Coin")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Coin"),
			CCoin::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	///* 몬스터 */
	//if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Monster_ImGui")) == nullptr)
	//{
	//	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_ImGui"),
	//		CMonster::Create(m_pDevice, m_pContext))))
	//		return E_FAIL;
	//}

	/* Prototype GameObject Player*/
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Player")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
			CPlayer::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Body_Player")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),
			CBody_Player::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}


	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Weapon")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"),
			CWeapon::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// Environment
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Environment_ImGui")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment_ImGui"),
			CEnvironment::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Save")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Save"),
			CBackGround::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Collision_Box")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Collision_Box"),
			CCollisionBox::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_Component_Collider_AABB")) == nullptr)
	{
		/* For.Prototype_Component_Collider_AABB */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Collider_AABB"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
			return E_FAIL;
	}
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_Component_Collider_OBB")) == nullptr)
	{
		/* For.Prototype_Component_Collider_OBB */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Collider_OBB"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_Component_Collider_Sphere")) == nullptr)
	{
		/* For.Prototype_Component_Collider_Sphere */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Collider_Sphere"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
			return E_FAIL;
	}

	// 플레이어
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Player")) == nullptr)
	{
		/* Prototype GameObject Player*/
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
			CPlayer::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// 플레이어 몸
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Body_Player")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),
			CBody_Player::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 플레이어 머리
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Head_Player")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Head_Player"),
			CHead_Player::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 플레이어 TPS 피봇
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Pivot")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pivot"),
			CPivot::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// 플레이어 FPS vlqht
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_FPSPivot")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FPSPivot"),
			CFPS_Pivot::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 플레이어 무기
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Weapon")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"),
			CWeapon::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 플레이어 칼
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Katana")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Katana"),
			CWeapon_Katana::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 무기 아이템
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_WeaponItem")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WeaponItem"),
			CWeapon_Item::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 코인
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Coin")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Coin"),
			CCoin::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// Battery 
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Battery")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Battery"),
			CBattery::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("로딩 완료되었습니다.");
	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_NavigationLevel()
{
	m_strLoadingText = TEXT("텍스쳐 로딩중입니다.");

	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("모델 로딩중입니다.");

	Loading_DataFile_For_NavigationLevel();

	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("셰이더 로딩중입니다.");

	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("객체원형 로딩중입니다.");

	// 터레인
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Terrain")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
			CTerrain::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	/* Prototype_GameObject_Camera_Free */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Camera_Free")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
			CCamera_Free::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// Environment
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Environment_ImGui")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment_ImGui"),
			CEnvironment::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	//Bullet
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Bullet")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet"),
			CBullet::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	//MonsterBullet
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_MonsterBullet")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterBullet"),
			CMonster_Bullet::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// BrainCore
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_BrainCore")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BrainCore"),
			CBrainCore::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Collision_Box")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Collision_Box"),
			CCollisionBox::Create(m_pDevice, m_pContext))))
			return E_FAIL;

	}
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Player")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
			CPlayer::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Body_Player")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),
			CBody_Player::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}


	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Weapon")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"),
			CWeapon::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_Component_Collider_AABB")) == nullptr)
	{
		/* For.Prototype_Component_Collider_AABB */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Collider_AABB"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
			return E_FAIL;
	}
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_Component_Collider_OBB")) == nullptr)
	{
		/* For.Prototype_Component_Collider_OBB */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Collider_OBB"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_Component_Collider_Sphere")) == nullptr)
	{
		/* For.Prototype_Component_Collider_Sphere */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Collider_Sphere"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
			return E_FAIL;
	}
	// Environment
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Environment_ImGui")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment_ImGui"),
			CEnvironment::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 플레이어
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Player")) == nullptr)
	{
		/* Prototype GameObject Player*/
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
			CPlayer::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// 플레이어 몸
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Body_Player")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),
			CBody_Player::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 플레이어 머리
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Head_Player")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Head_Player"),
			CHead_Player::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 플레이어 TPS 피봇
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Pivot")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pivot"),
			CPivot::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// 플레이어 FPS vlqht
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_FPSPivot")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FPSPivot"),
			CFPS_Pivot::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 플레이어 무기
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Weapon")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"),
			CWeapon::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 플레이어 칼
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Katana")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Katana"),
			CWeapon_Katana::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 무기 아이템
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_WeaponItem")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WeaponItem"),
			CWeapon_Item::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// 코인
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Coin")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Coin"),
			CCoin::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// Battery 
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Battery")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Battery"),
			CBattery::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// CoinItem 
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_CoinItem")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CoinItem"),
			CCoin_Item::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("로딩 완료되었습니다.");
	m_fPersent += 20.f;
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_MonsterSpawnLevel()
{
	m_strLoadingText = TEXT("텍스쳐 로딩중입니다.");

	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("모델 로딩중입니다.");

	Loading_DataFile_For_MonsterSpawnLevel(m_eTargetLevel);

	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("셰이더 로딩중입니다.");

	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("객체원형 로딩중입니다.");

	// 터레인
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Terrain")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
			CTerrain::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	/* Prototype_GameObject_Camera_Free */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Camera_Free")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
			CCamera_Free::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// Environment
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Environment_ImGui")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment_ImGui"),
			CEnvironment::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Save")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Save"),
			CBackGround::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Collision_Box")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Collision_Box"),
			CCollisionBox::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}


	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_Component_Collider_AABB")) == nullptr)
	{
		/* For.Prototype_Component_Collider_AABB */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Collider_AABB"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
			return E_FAIL;
	}
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_Component_Collider_OBB")) == nullptr)
	{
		/* For.Prototype_Component_Collider_OBB */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Collider_OBB"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_Component_Collider_Sphere")) == nullptr)
	{
		/* For.Prototype_Component_Collider_Sphere */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Collider_Sphere"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
			return E_FAIL;
	}
	//Bullet
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Bullet")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet"),
			CBullet::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	//MonsterBullet
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_MonsterBullet")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterBullet"),
			CMonster_Bullet::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// BrainCore
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_BrainCore")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BrainCore"),
			CBrainCore::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Collision_Box")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Collision_Box"),
			CCollisionBox::Create(m_pDevice, m_pContext))))
			return E_FAIL;

	}


	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("로딩 완료되었습니다.");
	m_fPersent += 20.f;
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_DataFile(LEVELID eLevelID)
{
	_int iEnvironmentIndex = 0;
	_int iPathIndex = 0;
	_matrix			PreTransformMatrix = XMMatrixIdentity();
	const _wstring Model_Component = TEXT("Prototype_Component_Model_Environment");
	const _wstring Model_Path = TEXT("../Bin/Resources/Model/ModelData_NonAnim");
	const _wstring Ext = TEXT(".dat");
	// PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	PreTransformMatrix = XMMatrixScaling(100.f, 100.f, 100.f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	cout << "Environment ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while(iPathIndex < ENVIRONMENT_EA)
	{
		const _wstring Model_Component_Result = Model_Component + to_wstring(iEnvironmentIndex);
		const _wstring Model_Path_Result = Model_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iEnvironmentIndex))))
			return E_FAIL;
		iEnvironmentIndex++;
		iPathIndex++;
	}
	const _wstring Model_Build_Path = TEXT("../Bin/Resources/Model/ModelData_Build");
	iPathIndex = 0;
	cout << "BUILD ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while (iPathIndex < BUILD_EA)
	{
		const _wstring Model_Component_Result = Model_Component + to_wstring(iEnvironmentIndex);
		const _wstring Model_Path_Result = Model_Build_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iEnvironmentIndex))))
			return E_FAIL;
		iEnvironmentIndex++;
		iPathIndex++;
	}

	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	const _wstring Model_Component_Character = TEXT("Prototype_Component_Model_Character");
	const _wstring Model_Character_Path = TEXT("../Bin/Resources/Model/ModelData_Character");
	iPathIndex = 0;
	_uint iCharacterIndex = 0;
	cout << "Character ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while (iPathIndex < 2)
	{
		const _wstring Model_Component_Result = Model_Component_Character + to_wstring(iCharacterIndex);
		const _wstring Model_Path_Result = Model_Character_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iCharacterIndex))))
			return E_FAIL;
		iCharacterIndex++;
		iPathIndex++;
	}


	const _wstring Model_Component_Weapon = TEXT("Prototype_Component_Model_Weapon");
	const _wstring Model_Weapon_Path = TEXT("../Bin/Resources/Model/ModelData_Weapon");
	iPathIndex = 0;
	_uint iWeaponIndex = 0;
	cout << "WEAPON ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;

	while (iPathIndex < WEAPON_EA)
	{

		const _wstring Model_Component_Result = Model_Component_Weapon + to_wstring(iWeaponIndex);
		const _wstring Model_Path_Result = Model_Weapon_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iWeaponIndex))))
			return E_FAIL;
		iWeaponIndex++;
		iPathIndex++;
	}


	// 애니메이션
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	_int iAnimModelIndex = 0;
	const _wstring ModelAnim_Component = TEXT("Prototype_Component_Model_Anim");
	const _wstring ModelAnim_Path = TEXT("../Bin/Resources/AnimModel/ModelData_Anim");

	cout << "애니메이션 ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;

	while (iAnimModelIndex < 12)
	{
		if ( iAnimModelIndex == 11)
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f,0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		}
		else if (iAnimModelIndex == 2) // Tank
		{
			PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		}
		else if (  iAnimModelIndex == 7)
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(185.f));
		}
		else if (iAnimModelIndex == 6)
		{
			PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 5.f, 0.f);
		}
		else if (iAnimModelIndex == 8 || iAnimModelIndex == 10 || iAnimModelIndex == 0)
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
		}
		else
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f)) ;
		}
//		cout <<  endl << "------------------------------------------------------" << endl   << iAnimModelIndex;
		const _wstring ModelAnim_Component_Result = ModelAnim_Component + to_wstring(iAnimModelIndex);
		const _wstring ModelAnim_Path_Result = ModelAnim_Path + to_wstring(iAnimModelIndex) + Ext;
		cout << iAnimModelIndex << "번 애님모델" << endl;
		cout << "--------------------------------------------------" << endl;
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, ModelAnim_Component_Result,
			CModel::Create_ReadDataFile_For_Anim(m_pDevice, m_pContext, CModel::TYPE_ANIM, ModelAnim_Path_Result, PreTransformMatrix, iAnimModelIndex))))
			return E_FAIL;
		++iAnimModelIndex;
	}

	const _wstring Model_Component_Trap = TEXT("Prototype_Component_Model_Trap");
	const _wstring Model_Trap_Path = TEXT("../Bin/Resources/Model/ModelData_Trap");
	iPathIndex = 0;
	_uint iTrapIndex = 0;
	cout << "Trap ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while (iPathIndex < TRAP_EA)
	{
		const _wstring Model_Component_Result = Model_Component_Trap + to_wstring(iTrapIndex);
		const _wstring Model_Path_Result = Model_Trap_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iTrapIndex))))
			return E_FAIL;
		iTrapIndex++;
		iPathIndex++;
	}
	return S_OK;
}

HRESULT CLoader::Loading_DataFile_For_GameLevel()
{
	_int iPathIndex{}, iModelIndex{}, iEnvironmentIndex = 0;
	DWORD dwByte = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------------

	_matrix			PreTransformMatrix = XMMatrixIdentity();
	const _wstring Model_Component = TEXT("Prototype_Component_Model_Environment");
	const _wstring Model_Path = TEXT("../Bin/Resources/Model/ModelData_NonAnim");
	const _wstring Ext = TEXT(".dat");
	PreTransformMatrix = XMMatrixScaling(100.f, 100.f, 100.f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	cout << "Environment ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;

	HANDLE hFile = CreateFile(L"../Bin/Data/GamePlayLevel_Env_Index.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load GamePlayLevel_Env_Index File Failed", L"Error", MB_OK);
		return E_FAIL;
	}

	while (ReadFile(hFile, &iModelIndex, sizeof(_int), &dwByte, nullptr) && dwByte > 0)
	{
		const _wstring Model_Component_Result = Model_Component + to_wstring(iModelIndex);
		const _wstring Model_Path_Result = Model_Path + to_wstring(iModelIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iModelIndex))))
			return E_FAIL;
		
	}
	CloseHandle(hFile);
	cout << "Environment Read 완료" << endl;

	//-----------------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------------
	const _wstring Model_Build_Path = TEXT("../Bin/Resources/Model/ModelData_Build");

	cout << "BUILD ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;

	HANDLE hBuildFile = CreateFile(L"../Bin/Data/GamePlayLevel_Build_Index.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hBuildFile)
	{
		MessageBox(NULL, L"Load GamePlayLevel_Build_Index File Failed", L"Error", MB_OK);
		return E_FAIL;
	}

	while (ReadFile(hBuildFile, &iModelIndex, sizeof(_int), &dwByte, nullptr) && dwByte > 0)
	{
		const _wstring Model_Component_Result = Model_Component + to_wstring(iModelIndex + ENVIRONMENT_EA);
		const _wstring Model_Path_Result = Model_Build_Path + to_wstring(iModelIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iModelIndex ))))
			return E_FAIL;
		
	}
	CloseHandle(hBuildFile);
	cout << "Build Read 완료" << endl;
	// 브레인 코어
	_wstring Model_Component_Result = Model_Component + to_wstring(40 + ENVIRONMENT_EA);
	_wstring Model_Path_Result = Model_Build_Path + to_wstring(40) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 40))))
		return E_FAIL;
	// 에너지 머신
	 Model_Component_Result = Model_Component + to_wstring(44 + ENVIRONMENT_EA);
	 Model_Path_Result = Model_Build_Path + to_wstring(44) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 44))))
		return E_FAIL;
	// 에너지 머신 레이더
	Model_Component_Result = Model_Component + to_wstring(45 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(45) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 45))))
		return E_FAIL;
	// 에너지 Cap
	Model_Component_Result = Model_Component + to_wstring(43 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(43) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 45))))
		return E_FAIL;

	// 코인
	PreTransformMatrix = XMMatrixScaling(10.f, 10.f, 10.f);
	Model_Component_Result = Model_Component + to_wstring(41 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(41) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 40))))
		return E_FAIL;

	// 뱃지아이템
	PreTransformMatrix = XMMatrixScaling(10.f, 10.f, 10.f);
	Model_Component_Result = Model_Component + to_wstring(46 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(46) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 46))))
		return E_FAIL;
	// Coin_L아이템
	PreTransformMatrix = XMMatrixScaling(10.f, 10.f, 10.f);
	Model_Component_Result = Model_Component + to_wstring(47 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(47) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 47))))
		return E_FAIL;
	// Coin_M아이템
	PreTransformMatrix = XMMatrixScaling(10.f, 10.f, 10.f);
	Model_Component_Result = Model_Component + to_wstring(48 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(48) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 48))))
		return E_FAIL;
	// Coin_S아이템
	PreTransformMatrix = XMMatrixScaling(10.f, 10.f, 10.f);
	Model_Component_Result = Model_Component + to_wstring(49 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(49) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 49))))
		return E_FAIL;
	// HP아이템
	PreTransformMatrix = XMMatrixScaling(10.f, 10.f, 10.f);
	Model_Component_Result = Model_Component + to_wstring(50 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(50) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 50))))
		return E_FAIL;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------------
	const _wstring Model_Bullet_Path = TEXT("../Bin/Resources/Model/ModelData_Bullet");
	cout << "Bullet ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	const _wstring Model_Bullet_Component = TEXT("Prototype_Component_Model_Bullet");
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f);
	for(int i = 0; i < BULLET_EA;i++)
	{
		const _wstring Model_Component_Bullet_Result = Model_Bullet_Component + to_wstring(i);
		const _wstring Model_Path_Bullet_Result = Model_Bullet_Path + to_wstring(i) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Bullet_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Bullet_Result, PreTransformMatrix, i))))
			return E_FAIL;
	}
	cout << "Bullet Read 완료" << endl;

	//-----------------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------------		
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	const _wstring Model_Component_Character = TEXT("Prototype_Component_Model_Character");
	const _wstring Model_Character_Path = TEXT("../Bin/Resources/Model/ModelData_Character");
	iPathIndex = 0;
	_uint iCharacterIndex = 0;
	cout << "Character ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while (iPathIndex < 2)
	{
		const _wstring Model_Component_Result = Model_Component_Character + to_wstring(iCharacterIndex);
		const _wstring Model_Path_Result = Model_Character_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iCharacterIndex))))
			return E_FAIL;
		iCharacterIndex++;
		iPathIndex++;
	}

	const _wstring Model_Component_Weapon = TEXT("Prototype_Component_Model_Weapon");
	const _wstring Model_Weapon_Path = TEXT("../Bin/Resources/Model/ModelData_Weapon");
	iPathIndex = 0;
	_uint iWeaponIndex = 0;
	cout << "WEAPON ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while (iPathIndex < WEAPON_EA)
	{

		const _wstring Model_Component_Result = Model_Component_Weapon + to_wstring(iWeaponIndex);
		const _wstring Model_Path_Result = Model_Weapon_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iWeaponIndex))))
			return E_FAIL;
		iWeaponIndex++;
		iPathIndex++;
	}

	const _wstring Model_Component_Trap = TEXT("Prototype_Component_Model_Trap");
	const _wstring Model_Trap_Path = TEXT("../Bin/Resources/Model/ModelData_Trap");
	iPathIndex = 0;
	_uint iTrapIndex = 0;
	cout << "Trap ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while (iPathIndex < TRAP_EA)
	{
		const _wstring Model_Component_Result = Model_Component_Trap + to_wstring(iTrapIndex);
		const _wstring Model_Path_Result = Model_Trap_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iTrapIndex))))
			return E_FAIL;
		iTrapIndex++;
		iPathIndex++;
	}


	const _wstring Model_Component_Effect = TEXT("Prototype_Component_Model_Effect");
	const _wstring Model_Effect_Path = TEXT("../Bin/Resources/Model/ModelData_Effect");
	iPathIndex = 0;
	_uint iEffectIndex = 0;
	cout << "Effect ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while (iPathIndex < EFFECT_EA)
	{
		const _wstring Model_Component_Result = Model_Component_Effect + to_wstring(iEffectIndex);
		const _wstring Model_Path_Result = Model_Effect_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iEffectIndex))))
			return E_FAIL;
		iEffectIndex++;
		iPathIndex++;
	}

	// 애니메이션
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	_int iAnimModelIndex = 0;
	const _wstring ModelAnim_Component = TEXT("Prototype_Component_Model_Anim");
	const _wstring ModelAnim_Path = TEXT("../Bin/Resources/AnimModel/ModelData_Anim");

	cout << "애니메이션 ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;

	while (iAnimModelIndex < 12)
	{
		if (iAnimModelIndex == 11)
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
		}
		else if (iAnimModelIndex == 2) // Tank
		{
			PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		}
		else if (iAnimModelIndex == 7)
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(185.f));
		}
		else if (iAnimModelIndex == 6)
		{
			PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 5.f, 0.f);
		}
		else if (iAnimModelIndex == 8 || iAnimModelIndex == 10 || iAnimModelIndex == 0)
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
		}
		else
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
		}
		//		cout <<  endl << "------------------------------------------------------" << endl   << iAnimModelIndex;
		const _wstring ModelAnim_Component_Result = ModelAnim_Component + to_wstring(iAnimModelIndex);
		const _wstring ModelAnim_Path_Result = ModelAnim_Path + to_wstring(iAnimModelIndex) + Ext;
		cout << iAnimModelIndex << "번 애님모델" << endl;
		cout << "--------------------------------------------------" << endl;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelAnim_Component_Result,
			CModel::Create_ReadDataFile_For_Anim(m_pDevice, m_pContext, CModel::TYPE_ANIM, ModelAnim_Path_Result, PreTransformMatrix, iAnimModelIndex))))
			return E_FAIL;
		iAnimModelIndex++;
	}

	return S_OK;
}

HRESULT CLoader::Loading_DataFile_For_YardLevel()
{
	_int iPathIndex{}, iModelIndex{}, iEnvironmentIndex = 0;
	DWORD dwByte = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------------

	_matrix			PreTransformMatrix = XMMatrixIdentity();
	const _wstring Model_Component = TEXT("Prototype_Component_Model_Environment");
	const _wstring Model_Path = TEXT("../Bin/Resources/Model/ModelData_NonAnim");
	const _wstring Ext = TEXT(".dat");
	PreTransformMatrix = XMMatrixScaling(100.f, 100.f, 100.f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	cout << "Environment ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;

	HANDLE hFile = CreateFile(L"../Bin/Data/GameYardLevel_Env_Index.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load GameYardLevel_Env_Index File Failed", L"Error", MB_OK);
		return E_FAIL;
	}

	while (ReadFile(hFile, &iModelIndex, sizeof(_int), &dwByte, nullptr) && dwByte > 0)
	{
		const _wstring Model_Component_Result = Model_Component + to_wstring(iModelIndex);
		const _wstring Model_Path_Result = Model_Path + to_wstring(iModelIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iModelIndex))))
			return E_FAIL;

	}
	CloseHandle(hFile);
	cout << "Environment Read 완료" << endl;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------------
	const _wstring Model_Build_Path = TEXT("../Bin/Resources/Model/ModelData_Build");

	cout << "BUILD ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;

	HANDLE hBuildFile = CreateFile(L"../Bin/Data/GameYardLevel_Build_Index.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hBuildFile)
	{
		MessageBox(NULL, L"Load GameYardLevel_Build_Index File Failed", L"Error", MB_OK);
		return E_FAIL;
	}

	while (ReadFile(hBuildFile, &iModelIndex, sizeof(_int), &dwByte, nullptr) && dwByte > 0)
	{
	
		const _wstring Model_Component_Result = Model_Component + to_wstring(iModelIndex + ENVIRONMENT_EA);
		const _wstring Model_Path_Result = Model_Build_Path + to_wstring(iModelIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iModelIndex))))
			return E_FAIL;
	}
	CloseHandle(hBuildFile);
	cout << "Build Read 완료" << endl;
	// 브레인 코어
	_wstring Model_Component_Result = Model_Component + to_wstring(40 + ENVIRONMENT_EA);
	_wstring Model_Path_Result = Model_Build_Path + to_wstring(40) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 40))))
		return E_FAIL;
	// 에너지 머신
	Model_Component_Result = Model_Component + to_wstring(44 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(44) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 44))))
		return E_FAIL;
	// 에너지 머신 레이더
	Model_Component_Result = Model_Component + to_wstring(45 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(45) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 45))))
		return E_FAIL;
	// 에너지 Cap
	Model_Component_Result = Model_Component + to_wstring(43 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(43) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 45))))
		return E_FAIL;

	// 코인
	PreTransformMatrix = XMMatrixScaling(10.f, 10.f, 10.f);
	Model_Component_Result = Model_Component + to_wstring(41 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(41) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 40))))
		return E_FAIL;

	//외계인 아이템
	Model_Component_Result = Model_Component + to_wstring(154 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(154) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 46))))
		return E_FAIL;

	//외계인 아이템
	Model_Component_Result = Model_Component + to_wstring(155 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(155) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 46))))
		return E_FAIL;

	// 뱃지아이템
	Model_Component_Result = Model_Component + to_wstring(46 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(46) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 46))))
		return E_FAIL;

	// Coin_L아이템
	Model_Component_Result = Model_Component + to_wstring(47 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(47) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 47))))
		return E_FAIL;
	// Coin_M아이템
	Model_Component_Result = Model_Component + to_wstring(48 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(48) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 48))))
		return E_FAIL;
	// Coin_S아이템
	Model_Component_Result = Model_Component + to_wstring(49 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(49) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 49))))
		return E_FAIL;
	// HP아이템
	Model_Component_Result = Model_Component + to_wstring(50 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(50) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 50))))
		return E_FAIL;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------------
	const _wstring Model_Bullet_Path = TEXT("../Bin/Resources/Model/ModelData_Bullet");
	cout << "Bullet ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	const _wstring Model_Bullet_Component = TEXT("Prototype_Component_Model_Bullet");
	for (int i = 0; i < BULLET_EA; i++)
	{
		if (i == 4)
		{
			PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f)/* * XMMatrixRotationZ(XMConvertToRadians(-90.f))*/; // 미사일
		}
		
		else
		{
			PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f);

		}

		const _wstring Model_Component_Bullet_Result = Model_Bullet_Component + to_wstring(i);
		const _wstring Model_Path_Bullet_Result = Model_Bullet_Path + to_wstring(i) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Bullet_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Bullet_Result, PreTransformMatrix, i))))
			return E_FAIL;
	}
	cout << "Bullet Read 완료" << endl;

	//-----------------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------------		
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	const _wstring Model_Component_Character = TEXT("Prototype_Component_Model_Character");
	const _wstring Model_Character_Path = TEXT("../Bin/Resources/Model/ModelData_Character");
	iPathIndex = 0;
	_uint iCharacterIndex = 0;
	cout << "Character ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while (iPathIndex < 2)
	{
		const _wstring Model_Component_Result = Model_Component_Character + to_wstring(iCharacterIndex);
		const _wstring Model_Path_Result = Model_Character_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iCharacterIndex))))
			return E_FAIL;
		iCharacterIndex++;
		iPathIndex++;
	}

	const _wstring Model_Component_Weapon = TEXT("Prototype_Component_Model_Weapon");
	const _wstring Model_Weapon_Path = TEXT("../Bin/Resources/Model/ModelData_Weapon");
	iPathIndex = 0;
	_uint iWeaponIndex = 0;
	cout << "WEAPON ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while (iPathIndex < WEAPON_EA)
	{

		const _wstring Model_Component_Result = Model_Component_Weapon + to_wstring(iWeaponIndex);
		const _wstring Model_Path_Result = Model_Weapon_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iWeaponIndex))))
			return E_FAIL;
		iWeaponIndex++;
		iPathIndex++;
	}

	const _wstring Model_Component_Trap = TEXT("Prototype_Component_Model_Trap");
	const _wstring Model_Trap_Path = TEXT("../Bin/Resources/Model/ModelData_Trap");
	iPathIndex = 0;
	_uint iTrapIndex = 0;
	cout << "Trap ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while (iPathIndex < TRAP_EA)
	{
		const _wstring Model_Component_Result = Model_Component_Trap + to_wstring(iTrapIndex);
		const _wstring Model_Path_Result = Model_Trap_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iTrapIndex))))
			return E_FAIL;
		iTrapIndex++;
		iPathIndex++;
	}

	const _wstring Model_Component_Effect = TEXT("Prototype_Component_Model_Effect");
	const _wstring Model_Effect_Path = TEXT("../Bin/Resources/Model/ModelData_Effect");
	iPathIndex = 0;
	_uint iEffectIndex = 0;
	cout << "Effect ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while (iPathIndex < EFFECT_EA)
	{
		const _wstring Model_Component_Result = Model_Component_Effect + to_wstring(iEffectIndex);
		const _wstring Model_Path_Result = Model_Effect_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iEffectIndex))))
			return E_FAIL;
		iEffectIndex++;
		iPathIndex++;
	}

	// 애니메이션
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	_int iAnimModelIndex = 0;
	const _wstring ModelAnim_Component = TEXT("Prototype_Component_Model_Anim");
	const _wstring ModelAnim_Path = TEXT("../Bin/Resources/AnimModel/ModelData_Anim");

	cout << "애니메이션 ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;

	while (iAnimModelIndex < 13)
	{
		if (iAnimModelIndex == 11)
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
		}
		else if (iAnimModelIndex == 2) // Tank
		{
			PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		}
		else if (iAnimModelIndex == 7)
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(185.f));
		}
		else if (iAnimModelIndex == 6)
		{
			PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 5.f, 0.f);
		}
		else if (iAnimModelIndex == 8 || iAnimModelIndex == 10 || iAnimModelIndex == 0)
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
		}
		else
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
		}
		//		cout <<  endl << "------------------------------------------------------" << endl   << iAnimModelIndex;
		const _wstring ModelAnim_Component_Result = ModelAnim_Component + to_wstring(iAnimModelIndex);
		const _wstring ModelAnim_Path_Result = ModelAnim_Path + to_wstring(iAnimModelIndex) + Ext;
		cout << iAnimModelIndex << "번 애님모델" << endl;
		cout << "--------------------------------------------------" << endl;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_YARD, ModelAnim_Component_Result,
			CModel::Create_ReadDataFile_For_Anim(m_pDevice, m_pContext, CModel::TYPE_ANIM, ModelAnim_Path_Result, PreTransformMatrix, iAnimModelIndex))))
			return E_FAIL;
		iAnimModelIndex++;
	}

	return S_OK;
}

HRESULT CLoader::Loading_DataFile_For_NavigationLevel()
{
	LEVELID eLevelID = LEVEL_NAVIGATION;
	_int iEnvironmentIndex = 0;
	_int iPathIndex = 0;
	_matrix			PreTransformMatrix = XMMatrixIdentity();
	const _wstring Model_Component = TEXT("Prototype_Component_Model_Environment");
	const _wstring Model_Path = TEXT("../Bin/Resources/Model/ModelData_NonAnim");
	const _wstring Ext = TEXT(".dat");
	// PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	PreTransformMatrix = XMMatrixScaling(100.f, 100.f, 100.f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	cout << "Environment ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while (iPathIndex < ENVIRONMENT_EA)
	{
		const _wstring Model_Component_Result = Model_Component + to_wstring(iEnvironmentIndex);
		const _wstring Model_Path_Result = Model_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iEnvironmentIndex))))
			return E_FAIL;
		iEnvironmentIndex++;
		iPathIndex++;
	}
	const _wstring Model_Build_Path = TEXT("../Bin/Resources/Model/ModelData_Build");
	iPathIndex = 0;
	cout << "BUILD ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while (iPathIndex < BUILD_EA)
	{
		const _wstring Model_Component_Result = Model_Component + to_wstring(iEnvironmentIndex);
		const _wstring Model_Path_Result = Model_Build_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iEnvironmentIndex))))
			return E_FAIL;
		iEnvironmentIndex++;
		iPathIndex++;
	}

	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	const _wstring Model_Component_Character = TEXT("Prototype_Component_Model_Character");
	const _wstring Model_Character_Path = TEXT("../Bin/Resources/Model/ModelData_Character");
	iPathIndex = 0;
	_uint iCharacterIndex = 0;
	cout << "Character ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while (iPathIndex < 2)
	{
		const _wstring Model_Component_Result = Model_Component_Character + to_wstring(iCharacterIndex);
		const _wstring Model_Path_Result = Model_Character_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iCharacterIndex))))
			return E_FAIL;
		iCharacterIndex++;
		iPathIndex++;
	}


	const _wstring Model_Component_Weapon = TEXT("Prototype_Component_Model_Weapon");
	const _wstring Model_Weapon_Path = TEXT("../Bin/Resources/Model/ModelData_Weapon");
	iPathIndex = 0;
	_uint iWeaponIndex = 0;
	cout << "WEAPON ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;

	while (iPathIndex < WEAPON_EA)
	{

		const _wstring Model_Component_Result = Model_Component_Weapon + to_wstring(iWeaponIndex);
		const _wstring Model_Path_Result = Model_Weapon_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iWeaponIndex))))
			return E_FAIL;
		iWeaponIndex++;
		iPathIndex++;
	}


	// 애니메이션
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	_int iAnimModelIndex = 0;
	const _wstring ModelAnim_Component = TEXT("Prototype_Component_Model_Anim");
	const _wstring ModelAnim_Path = TEXT("../Bin/Resources/AnimModel/ModelData_Anim");

	cout << "애니메이션 ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;

	while (iAnimModelIndex < 12)
	{
		if (iAnimModelIndex == 11)
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		}
		else if (iAnimModelIndex == 2) // Tank
		{
			PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		}
		else if (iAnimModelIndex == 7)
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(185.f));
		}
		else if (iAnimModelIndex == 6)
		{
			PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 5.f, 0.f);
		}
		else if (iAnimModelIndex == 8 || iAnimModelIndex == 10 || iAnimModelIndex == 0)
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
		}
		else
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
		}
		//		cout <<  endl << "------------------------------------------------------" << endl   << iAnimModelIndex;
		const _wstring ModelAnim_Component_Result = ModelAnim_Component + to_wstring(iAnimModelIndex);
		const _wstring ModelAnim_Path_Result = ModelAnim_Path + to_wstring(iAnimModelIndex) + Ext;
		cout << iAnimModelIndex << "번 애님모델" << endl;
		cout << "--------------------------------------------------" << endl;
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, ModelAnim_Component_Result,
			CModel::Create_ReadDataFile_For_Anim(m_pDevice, m_pContext, CModel::TYPE_ANIM, ModelAnim_Path_Result, PreTransformMatrix, iAnimModelIndex))))
			return E_FAIL;
		++iAnimModelIndex;
	}
	return S_OK;
}

HRESULT CLoader::Loading_DataFile_For_MonsterSpawnLevel(LEVELID eLevelID)
{
	_int iEnvironmentIndex = 0;
	_int iPathIndex = 0;
	_matrix			PreTransformMatrix = XMMatrixIdentity();
	const _wstring Model_Component = TEXT("Prototype_Component_Model_Environment");
	const _wstring Model_Path = TEXT("../Bin/Resources/Model/ModelData_NonAnim");
	const _wstring Ext = TEXT(".dat");
	// PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	PreTransformMatrix = XMMatrixScaling(100.f, 100.f, 100.f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	cout << "Environment ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while (iPathIndex < ENVIRONMENT_EA)
	{
		const _wstring Model_Component_Result = Model_Component + to_wstring(iEnvironmentIndex);
		const _wstring Model_Path_Result = Model_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iEnvironmentIndex))))
			return E_FAIL;
		iEnvironmentIndex++;
		iPathIndex++;
	}
	const _wstring Model_Build_Path = TEXT("../Bin/Resources/Model/ModelData_Build");
	iPathIndex = 0;
	cout << "BUILD ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while (iPathIndex < BUILD_EA)
	{
		const _wstring Model_Component_Result = Model_Component + to_wstring(iEnvironmentIndex);
		const _wstring Model_Path_Result = Model_Build_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iEnvironmentIndex))))
			return E_FAIL;
		iEnvironmentIndex++;
		iPathIndex++;
	}

	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	const _wstring Model_Component_Character = TEXT("Prototype_Component_Model_Character");
	const _wstring Model_Character_Path = TEXT("../Bin/Resources/Model/ModelData_Character");
	iPathIndex = 0;
	_uint iCharacterIndex = 0;
	cout << "Character ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while (iPathIndex < 2)
	{
		const _wstring Model_Component_Result = Model_Component_Character + to_wstring(iCharacterIndex);
		const _wstring Model_Path_Result = Model_Character_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iCharacterIndex))))
			return E_FAIL;
		iCharacterIndex++;
		iPathIndex++;
	}


	const _wstring Model_Component_Weapon = TEXT("Prototype_Component_Model_Weapon");
	const _wstring Model_Weapon_Path = TEXT("../Bin/Resources/Model/ModelData_Weapon");
	iPathIndex = 0;
	_uint iWeaponIndex = 0;
	cout << "WEAPON ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;

	while (iPathIndex < WEAPON_EA)
	{

		const _wstring Model_Component_Result = Model_Component_Weapon + to_wstring(iWeaponIndex);
		const _wstring Model_Path_Result = Model_Weapon_Path + to_wstring(iPathIndex) + Ext;
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iWeaponIndex))))
			return E_FAIL;
		iWeaponIndex++;
		iPathIndex++;
	}


	// 애니메이션
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	_int iAnimModelIndex = 0;
	const _wstring ModelAnim_Component = TEXT("Prototype_Component_Model_Anim");
	const _wstring ModelAnim_Path = TEXT("../Bin/Resources/AnimModel/ModelData_Anim");

	cout << "애니메이션 ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;

	while (iAnimModelIndex < 12)
	{
		if (iAnimModelIndex == 11)
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		}
		else if (iAnimModelIndex == 2) // Tank
		{
			PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		}
		else if (iAnimModelIndex == 7)
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(185.f));
		}
		else if (iAnimModelIndex == 6)
		{
			PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 5.f, 0.f);
		}
		else if (iAnimModelIndex == 8 || iAnimModelIndex == 10 || iAnimModelIndex == 0)
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
		}
		else
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
		}
		//		cout <<  endl << "------------------------------------------------------" << endl   << iAnimModelIndex;
		const _wstring ModelAnim_Component_Result = ModelAnim_Component + to_wstring(iAnimModelIndex);
		const _wstring ModelAnim_Path_Result = ModelAnim_Path + to_wstring(iAnimModelIndex) + Ext;
		cout << iAnimModelIndex << "번 애님모델" << endl;
		cout << "--------------------------------------------------" << endl;
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, ModelAnim_Component_Result,
			CModel::Create_ReadDataFile_For_Anim(m_pDevice, m_pContext, CModel::TYPE_ANIM, ModelAnim_Path_Result, PreTransformMatrix, iAnimModelIndex))))
			return E_FAIL;
		++iAnimModelIndex;
	}
	return S_OK;
}



HRESULT CLoader::Loading_Effect(LEVELID eLevelID)
{
	/* For.Prototype_Component_Texture_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Texture_Explosion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
		return E_FAIL;

	/*플레어 총 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Texture_Flare"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Shot/Flare%d.png"), 9))))
		return E_FAIL;
		
	/*플레어 총 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Texture_Flare_DDS"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Shot/Flare%d.dds"), 4))))
		return E_FAIL;

	/*탱크 폭발 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Texture_Tank_Explosion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Explosion/ExplosionEffect%d.dds"),8))))
		return E_FAIL;

	/* 전기 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Texture_Lightning"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Lightning/Electricity%d.dds"), 6))))
		return E_FAIL;

	// 디졸브
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Texture_Dissolved"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Dissolve/Dissolve%d.dds"), 4))))
		return E_FAIL;

	
	/* Prototype_GameObject_DeadModel */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_DeadModel")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DeadModel"),
			CDead_Model::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	/* Prototype_GameObject_Missile_Flare */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Missile_Flare")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Missile_Flare"),
			CMissile_Flame::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}


	/* Prototype_GameObject_Effect_Katana */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Katana_Effect")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Katana_Effect"),
			CKatana_Effect::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	/* Prototype_GameObject_Effect_Explosion */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Effect_Explosion")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Explosion"),
			CEffect_Explosion::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	/* Prototype_GameObject_Effect_Lightning */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Effect_Lightning")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Lightning"),
			CEffect_Electricity::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	/* Rader */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Rader_Effect")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rader_Effect"),
			CRader_Effect::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	/* Prototype_GameObject_Effect_Explosion */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Effect_Tank_Explosion")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Tank_Explosion"),
			CEffect_Explosion_Tank::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	/* Rifle Flare */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Rifle_Flare")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rifle_Flare"),
			CEffect_Flare_Rifle::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CLoader::Loading_DataFile_For_Instancing_YardLevel()
{
	// 108번 잔디 인스턴스 생성
	_uint m_iModelIndex = 108;
	m_iGrass_Count[0] = 0;
	HANDLE hFile{};
	DWORD dwByte = 0;
	LEVELID iLevel;
	_float3 fPos{};
	_wstring Grass_Path = TEXT("../Bin/Data/Grass");
	_wstring Last_Path = TEXT(".dat");
	_wstring Result_Path = Grass_Path + TEXT("_Yard") + to_wstring(m_iModelIndex) + Last_Path;
	hFile = CreateFile(Result_Path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Grass_Yard File Failed", L"Error", MB_OK);
		return E_FAIL; 
	} 
	while (ReadFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr) && dwByte > 0)
	{
		ReadFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
		m_vecGrassPos[0].push_back(fPos);
		m_iGrass_Count[0]++;
	}
	CloseHandle(hFile);

	// 109번 잔디 인스턴스 생성
	m_iModelIndex = 109;
	m_iGrass_Count[1] = 0;
	dwByte = 0;
	
	Result_Path = Grass_Path + TEXT("_Yard") + to_wstring(m_iModelIndex) + Last_Path;
	hFile = CreateFile(Result_Path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Grass_Yard File Failed", L"Error", MB_OK);
		return E_FAIL;
	}
	while (ReadFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr) && dwByte > 0)
	{
		ReadFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
		m_vecGrassPos[1].push_back(fPos);
		m_iGrass_Count[1]++;
	}
	CloseHandle(hFile);

	// 110번 잔디 인스턴스 생성
	m_iModelIndex = 110;
	m_iGrass_Count[2] = 0;
	dwByte = 0;

	Result_Path = Grass_Path + TEXT("_Yard") + to_wstring(m_iModelIndex) + Last_Path;
	hFile = CreateFile(Result_Path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Grass_Yard File Failed", L"Error", MB_OK);
		return E_FAIL;
	}
	while (ReadFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr) && dwByte > 0)
	{
		ReadFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
		m_vecGrassPos[2].push_back(fPos);
		m_iGrass_Count[2]++;
	}
	CloseHandle(hFile);

	// 111번 잔디 인스턴스 생성
	m_iModelIndex = 111;
	m_iGrass_Count[3] = 0;
	dwByte = 0;

	Result_Path = Grass_Path + TEXT("_Yard") + to_wstring(m_iModelIndex) + Last_Path;
	hFile = CreateFile(Result_Path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Grass_Yard File Failed", L"Error", MB_OK);
		return E_FAIL;
	}
	while (ReadFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr) && dwByte > 0)
	{
		ReadFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
		m_vecGrassPos[3].push_back(fPos);
		m_iGrass_Count[3]++;
	}
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CLoader::Loading_DataFile_For_Instancing_ImGuiLevel()
{
	// 108번 잔디 인스턴스 생성
	_uint m_iModelIndex = 108;
	m_iGrass_Count[0] = 0;
	HANDLE hFile{};
	DWORD dwByte = 0;
	LEVELID iLevel;
	_float3 fPos{};
	_wstring Grass_Path = TEXT("../Bin/Data/Grass");
	_wstring Last_Path = TEXT(".dat");
	_wstring Result_Path = Grass_Path + TEXT("_Yard") + to_wstring(m_iModelIndex) + Last_Path;
	hFile = CreateFile(Result_Path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Grass_Yard File Failed", L"Error", MB_OK);
		return E_FAIL;
	}
	while (ReadFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr) && dwByte > 0)
	{
		ReadFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
		m_vecGrassPos[0].push_back(fPos);
		m_iGrass_Count[0]++;
	}
	CloseHandle(hFile);

	// 109번 잔디 인스턴스 생성
	m_iModelIndex = 109;
	m_iGrass_Count[1] = 0;
	dwByte = 0;

	Result_Path = Grass_Path + TEXT("_Yard") + to_wstring(m_iModelIndex) + Last_Path;
	hFile = CreateFile(Result_Path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Grass_Yard File Failed", L"Error", MB_OK);
		return E_FAIL;
	}
	while (ReadFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr) && dwByte > 0)
	{
		ReadFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
		m_vecGrassPos[1].push_back(fPos);
		m_iGrass_Count[1]++;
	}
	CloseHandle(hFile);
	
	// 110번 잔디 인스턴스 생성
	m_iModelIndex = 110;
	m_iGrass_Count[2] = 0;
	dwByte = 0;

	Result_Path = Grass_Path + TEXT("_Yard") + to_wstring(m_iModelIndex) + Last_Path;
	hFile = CreateFile(Result_Path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Grass_Yard File Failed", L"Error", MB_OK);
		return E_FAIL;
	}
	while (ReadFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr) && dwByte > 0)
	{
		ReadFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
		m_vecGrassPos[2].push_back(fPos);
		m_iGrass_Count[2]++;
	}
	CloseHandle(hFile);

	// 111번 잔디 인스턴스 생성
	m_iModelIndex = 111;
	m_iGrass_Count[3] = 0;
	dwByte = 0;

	Result_Path = Grass_Path + TEXT("_Yard") + to_wstring(m_iModelIndex) + Last_Path;
	hFile = CreateFile(Result_Path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Grass_Yard File Failed", L"Error", MB_OK);
		return E_FAIL;
	}
	while (ReadFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr) && dwByte > 0)
	{
		ReadFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
		m_vecGrassPos[3].push_back(fPos);
		m_iGrass_Count[3]++;
	}
	CloseHandle(hFile);

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
