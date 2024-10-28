#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"

#include "BackGround.h"
#include "CrossLine.h"
#include "InGameUI.h"
#include "MenuUI.h"
#include "ButtonUI.h"
#include "NumberUI.h"


#include "Terrain.h"
#include "Camera_Free.h"

#include "Tank.h"
#include "Helicopter.h"
#include "Alien.h"
#include "Pony.h"



#include "Environment.h"
#include "BrainCore.h"
#include "Coin.h"
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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/CenterUI%d.png"),2))))
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
	/* For.Prototype_GameObject_BackGround */


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

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Player")) == nullptr)
	{
		/* Prototype GameObject Player*/
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

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Head_Player")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Head_Player"),
			CHead_Player::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Pivot")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pivot"),
			CPivot::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_FPSPivot")) == nullptr)
	{

		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FPSPivot"),
			CFPS_Pivot::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Weapon")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"),
			CWeapon::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Katana")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Katana"),
			CWeapon_Katana::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_WeaponItem")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WeaponItem"),
			CWeapon_Item::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Coin")) == nullptr)
	{
		/* Prototype_GameObject_Weapon */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Coin"),
			CCoin::Create(m_pDevice, m_pContext))))
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

	// Coin
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Coin")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Coin"),
			CBrainCore::Create(m_pDevice, m_pContext))))
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

HRESULT CLoader::Loading_For_ImGuiLevel()
{
	m_strLoadingText = TEXT("텍스쳐 로딩중입니다.");

	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("모델 로딩중입니다.");

	Loading_DataFile(LEVEL_IMGUI);
	// 몬스터

	//----------------------------------------------------------------------------------------------------
	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("셰이더 로딩중입니다.");

	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("객체원형 로딩중입니다.");

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

	Loading_DataFile_For_MonsterSpawnLevel();

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
		if (iAnimModelIndex == 0)
		{
			PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
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
		else if (iAnimModelIndex == 8 || iAnimModelIndex == 10)
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
		iAnimModelIndex++;
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
	// 코인
	PreTransformMatrix = XMMatrixScaling(10.f, 10.f, 10.f);
	Model_Component_Result = Model_Component + to_wstring(41 + ENVIRONMENT_EA);
	Model_Path_Result = Model_Build_Path + to_wstring(41) + Ext;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, Model_Component_Result,
		CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, 40))))
		return E_FAIL;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------------
	const _wstring Model_Bullet_Path = TEXT("../Bin/Resources/Model/ModelData_Bullet");
	cout << "Bullet ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	const _wstring Model_Bullet_Component = TEXT("Prototype_Component_Model_Bullet");
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f);
	for(int i = 0; i <4;i++)
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



	// 애니메이션
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	_int iAnimModelIndex = 0;
	const _wstring ModelAnim_Component = TEXT("Prototype_Component_Model_Anim");
	const _wstring ModelAnim_Path = TEXT("../Bin/Resources/AnimModel/ModelData_Anim");

	cout << "애니메이션 ---------------------------------------------------------------------------" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;

	while (iAnimModelIndex < 12)
	{
		if (iAnimModelIndex == 0)
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
		else if (iAnimModelIndex == 8 || iAnimModelIndex == 10)
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

HRESULT CLoader::Loading_DataFile_For_NavigationLevel()
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
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NAVIGATION, Model_Component_Result,
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
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NAVIGATION, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iModelIndex))))
			return E_FAIL;

	}
	CloseHandle(hBuildFile);
	cout << "Build Read 완료" << endl;
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
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NAVIGATION, Model_Component_Result,
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
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NAVIGATION, Model_Component_Result,
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
		if (iAnimModelIndex == 0)
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
		else if (iAnimModelIndex == 8 || iAnimModelIndex == 10)
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
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_NAVIGATION, ModelAnim_Component_Result,
			CModel::Create_ReadDataFile_For_Anim(m_pDevice, m_pContext, CModel::TYPE_ANIM, ModelAnim_Path_Result, PreTransformMatrix, iAnimModelIndex))))
			return E_FAIL;
		iAnimModelIndex++;
	}

	return S_OK;
}

HRESULT CLoader::Loading_DataFile_For_MonsterSpawnLevel()
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
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MONSTERSPAWN, Model_Component_Result,
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
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MONSTERSPAWN, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iModelIndex))))
			return E_FAIL;

	}
	CloseHandle(hBuildFile);
	cout << "Build Read 완료" << endl;
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
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MONSTERSPAWN, Model_Component_Result,
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
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MONSTERSPAWN, Model_Component_Result,
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
		if (iAnimModelIndex == 0)
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
		else if (iAnimModelIndex == 8 || iAnimModelIndex == 10)
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
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MONSTERSPAWN, ModelAnim_Component_Result,
			CModel::Create_ReadDataFile_For_Anim(m_pDevice, m_pContext, CModel::TYPE_ANIM, ModelAnim_Path_Result, PreTransformMatrix, iAnimModelIndex))))
			return E_FAIL;
		iAnimModelIndex++;
	}
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
