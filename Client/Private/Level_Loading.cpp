#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#include "Loader.h"

#include "Level_Logo.h"
#include "Level_gamePlay.h"
#include "Level_ImGui.h"
#include "Monster_Path.h"
#include "Navigation_Leve.h"
#include "BackGround.h"
#include "Level_Yard.h"

#include "GameInstance.h"



CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID)
{
	ShowCursor(false);

	/* 추후에 로딩이 끝날 시에 넘길 레벨에 대한 정보를 저장한다. */
	m_eNextLevelID = eNextLevelID;

	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_BackGround")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
			CBackGround::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_LoadingUI")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LoadingUI"),
			CLoading_UI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	if (FAILED(Ready_Layer_UI_GameTitle(TEXT("Layer_UI_TItle"))))
		return E_FAIL;

  	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_UI_Loading(TEXT("Layer_UI_Loading"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_UI_LOGO(TEXT("Layer_UI_Logo"))))
		return E_FAIL;

	
	/* 로딩 작업을 직접 수행할 하청업체를 선정한다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	ShowCursor(false);
	m_fLoading_Per = m_pLoader->Get_LoadingPer();
	
	/* 로더가 다음레벨에 대한 자원 생성을 끝냈다라면 */
 	if (true == m_pLoader->isFinished() /*&&
		GetKeyState(VK_SPACE) & 0x8000*/)
	{
		HRESULT			hr = {};

		/* 다음레벨 아이디에 맞는 실제 레벨을 할당해준다. */
		switch (m_eNextLevelID)
		{
		case LEVEL_LOGO:
 			hr = m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_Logo::Create(m_pDevice, m_pContext));
			break;
		case LEVEL_GAMEPLAY:
			hr = m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_GamePlay::Create(m_pDevice, m_pContext));
			break;
		case LEVEL_YARD:
			hr = m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_Yard::Create(m_pDevice, m_pContext));
			break;
		case LEVEL_IMGUI:
			hr = m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_ImGui::Create(m_pDevice, m_pContext));
			break;
		case LEVEL_NAVIGATION:
			hr = m_pGameInstance->Open_Level(m_eNextLevelID, CNavigation_Leve::Create(m_pDevice, m_pContext));
			break;
		case LEVEL_MONSTERSPAWN:
			hr = m_pGameInstance->Open_Level(m_eNextLevelID, CMonster_Path::Create(m_pDevice, m_pContext));
			break;
		}

		if (FAILED(hr))
			return;
	}

}

HRESULT CLevel_Loading::Render()
{
	__super::Render();

#ifdef _DEBUG
	m_pLoader->Output_LoadingState();
#endif

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CBackGround::UIOBJECT_DESC Desc{};
	Desc.eLevel = LEVEL_LOADING;
	Desc.fX = g_iWinSizeX * 0.5f;
	Desc.fY = g_iWinSizeY * 0.5f;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;
	Desc.iData = 10;
	Desc.fDepth = 0.3f;
	(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_LOADING, pLayerTag, TEXT("Prototype_GameObject_BackGround"), &Desc));

	return S_OK;
}
HRESULT CLevel_Loading::Ready_Layer_UI_Loading(const _tchar* pLayerTag)
{

	CLoading_UI::LOADINGUI_DESC Desc{};
	Desc.eLevel = LEVEL_LOADING;
	Desc.fX = g_iWinSizeX * 0.9f;
	Desc.fY = g_iWinSizeY * 0.8f;
	Desc.fSizeX = 150;
	Desc.fSizeY = 150;
	Desc.iData = 10;
	Desc.fDepth = 0.1f;
	Desc.eTag = CLoading_UI::LOADING_GAGE;
	(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_LOADING, pLayerTag, TEXT("Prototype_GameObject_LoadingUI"), &Desc));



	return S_OK;
}
HRESULT CLevel_Loading::Ready_Layer_UI_LOGO(const _tchar* pLayerTag)
{

	return S_OK;
}
HRESULT CLevel_Loading::Ready_Layer_UI_GameTitle(const _tchar* pLayerTag)
{
	CLoading_UI::LOADINGUI_DESC Desc2{};
	Desc2.eLevel = LEVEL_LOADING;
	Desc2.fX = g_iWinSizeX * 0.4f;
	Desc2.fY = g_iWinSizeY * 0.75f;
	Desc2.fSizeX = g_iWinSizeX * 0.8f;
	Desc2.fSizeY = g_iWinSizeY * 0.7f;
	Desc2.iData = 10;
	Desc2.eTargetLevel = m_eNextLevelID;
	Desc2.fDepth = 0.2f;
	Desc2.eTag = CLoading_UI::LOADING_BACKGROUND_GAMENAME;
	(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_LOADING, pLayerTag, TEXT("Prototype_GameObject_LoadingUI"), &Desc2));

	CLoading_UI::LOADINGUI_DESC Desc{};
	Desc.eLevel = LEVEL_LOADING;
	Desc.fX = g_iWinSizeX * 0.2f;
	Desc.fY = g_iWinSizeY * 0.75f;
	Desc.fSizeX = 500.f;
	Desc.fSizeY = 50.f;
	Desc.iData = 10;
	Desc.eTargetLevel = m_eNextLevelID;
	Desc.fDepth = 0.1f;
	Desc.eTag = CLoading_UI::LOADING_GAMENAME;
	(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_LOADING, pLayerTag, TEXT("Prototype_GameObject_LoadingUI"), &Desc));

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();
	Safe_Release(m_pLoader);
}
