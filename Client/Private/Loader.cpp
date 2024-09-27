#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"

#include "BackGround.h"
#include "CrossLine.h"
#include "InGameUI.h"

#include "Terrain.h"
#include "Camera_Free.h"
#include "Monster.h"
#include "Environment.h"

#include "MenuUI.h"
#include "ButtonUI.h"


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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Menu.png")))))
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


	// 크로스 라인
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Logo2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/CrossLine/CrossLine%d.png"), 24))))
		return E_FAIL;

	// HP
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/ClovA_system%d.png"), 6))))
		return E_FAIL;

	// ArmCannon
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayUI/AmmoCount%d.png"), 3))))
		return E_FAIL;


	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("모델 로딩중입니다.");


	// 몬스터
	_matrix			PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", PreTransformMatrix))))
	//	return E_FAIL;


	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("셰이더 로딩중입니다.");


	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;


	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("객체원형 로딩중입니다.");
	/* For.Prototype_GameObject_BackGround */

	// 크로스 라인
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_CrossLine")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CrossLine"),
			CCrossLine::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	//HP
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_UI")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"),
			CInGameUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	// ArmCannon
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_ArmCannon")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ArmCannon"),
			CInGameUI::Create(m_pDevice, m_pContext))))
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
	/* 몬스터 */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Monster")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
			CMonster::Create(m_pDevice, m_pContext))))
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


	Loading_DataFile();
	
	// 몬스터

	//----------------------------------------------------------------------------------------------------
	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("셰이더 로딩중입니다.");

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

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
	/* 몬스터 */
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Monster_ImGui")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_ImGui"),
			CMonster::Create(m_pDevice, m_pContext))))
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

	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_strLoadingText = TEXT("로딩 완료되었습니다.");
	m_fPersent += 20.f;//----------------------------------------------------------------------------------------------------
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_DataFile()
{
	_int iEnvironmentIndex = 0;
	_matrix			PreTransformMatrix = XMMatrixIdentity();
	const _wstring Model_Component = TEXT("Prototype_Component_Model_Environment");


	const _wstring Model_Path = TEXT("../Bin/Resources/Model_Bin/ModelData_NonAnim");
	const _wstring Ext = TEXT(".dat");


	// PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	PreTransformMatrix = XMMatrixScaling(100.f, 100.f, 100.f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	while(iEnvironmentIndex < 19)
	{
		const _wstring Model_Component_Result = Model_Component + to_wstring(iEnvironmentIndex);
		const _wstring Model_Path_Result = Model_Path + to_wstring(iEnvironmentIndex) + Ext;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, Model_Component_Result,
			CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Path_Result, PreTransformMatrix, iEnvironmentIndex))))
			return E_FAIL;
		iEnvironmentIndex++;
	}
	
	//Model_Component_Result = Model_Component + to_wstring(iEnvironmentIndex);
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment1"),
	//	CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Model_Component_Result, PreTransformMatrix, iEnvironmentIndex))))
	//	return E_FAIL;
	//iEnvironmentIndex++;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment2"),
	//	CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Model_Bin/ModelData_NonAnim2.dat"), PreTransformMatrix, iEnvironmentIndex))))
	//	return E_FAIL;
	//iEnvironmentIndex++;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment3"),
	//	CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Model_Bin/ModelData_NonAnim3.dat"), PreTransformMatrix, iEnvironmentIndex))))
	//	return E_FAIL;
	//iEnvironmentIndex++;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment4"),
	//	CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Model_Bin/ModelData_NonAnim4.dat"), PreTransformMatrix, iEnvironmentIndex))))
	//	return E_FAIL;
	//iEnvironmentIndex++;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment5"),
	//	CModel::Create_ReadDataFile(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Model_Bin/ModelData_NonAnim5.dat"), PreTransformMatrix, iEnvironmentIndex))))
	//	return E_FAIL;
	//iEnvironmentIndex++;


	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment6"),
	//	CModel::Create_For_FBX(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Model_Bin/ModelData_NonAnim6.dat"), PreTransformMatrix, iNumber))))
	//	return E_FAIL;
	//iNumber++;


	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment7"),
	//	CModel::Create_For_FBX(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Model_Bin/ModelData_NonAnim7.dat"), PreTransformMatrix, iNumber))))
	//	return E_FAIL;
	//iNumber++;


	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment8"),
	//	CModel::Create_For_FBX(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Model_Bin/ModelData_NonAnim8.dat"), PreTransformMatrix, iNumber))))
	//	return E_FAIL;
	//iNumber++;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment9"),
	//	CModel::Create_For_FBX(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Model_Bin/ModelData_NonAnim9.dat"), PreTransformMatrix, iNumber))))
	//	return E_FAIL;
	//iNumber++;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment10"),
	//	CModel::Create_For_FBX(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environment/Desk1.fbx", PreTransformMatrix, iNumber))))
	//	return E_FAIL;
	//iNumber++;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment11"),
	//	CModel::Create_For_FBX(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environment/Desk2.fbx", PreTransformMatrix, iNumber))))
	//	return E_FAIL;
	//iNumber++;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment12"),
	//	CModel::Create_For_FBX(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environment/Desk3.fbx", PreTransformMatrix, iNumber))))
	//	return E_FAIL;
	//iNumber++;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment13"),
	//	CModel::Create_For_FBX(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environment/KeyPad.fbx", PreTransformMatrix, iNumber))))
	//	return E_FAIL;
	//iNumber++;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment14"),
	//	CModel::Create_For_FBX(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environment/Vent1.fbx", PreTransformMatrix, iNumber))))
	//	return E_FAIL;
	//iNumber++;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment15"),
	//	CModel::Create_For_FBX(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environment/Sprinkler.fbx", PreTransformMatrix, iNumber))))
	//	return E_FAIL;
	//iNumber++;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment16"),
	//	CModel::Create_For_FBX(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environment/Trim.fbx", PreTransformMatrix, iNumber))))
	//	return E_FAIL;
	//iNumber++;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment17"),
	//	CModel::Create_For_FBX(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environment/Vent0.fbx", PreTransformMatrix, iNumber))))
	//	return E_FAIL;
	//iNumber++;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_IMGUI, TEXT("Prototype_Component_Model_Environment18"),
	//	CModel::Create_For_FBX(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Environment/card1.fbx", PreTransformMatrix, iNumber))))
	//	return E_FAIL;
	//iNumber++;

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
