#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#include "Loader.h"

#include "Level_Logo.h"
#include "Level_gamePlay.h"
#include "Level_ImGui.h"

#include "BackGround.h"



#include "GameInstance.h"


CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID)
{
	/* 추후에 로딩이 끝날 시에 넘길 레벨에 대한 정보를 저장한다. */
	m_eNextLevelID = eNextLevelID;

	/* 로딩화면을 보여주기위한 객체들을 생성한다. */

  	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_UI_Loading(TEXT("Layer_UI_Loading"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_UI_LOGO(TEXT("Layer_UI_Logo"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_UI_GameTitle(TEXT("Layer_UI_TItle"))))
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


	m_fLoading_Per = m_pLoader->Get_LoadingPer();
	if (m_pBackGround != nullptr)
		m_pBackGround->Update(fTimeDelta);

	if (m_pLoadingUI != nullptr)
	{
		m_pLoadingUI->Set_Percent(m_fLoading_Per);
		m_pLoadingUI->Update(fTimeDelta);

	}
	if (m_pLoadingUI_Logo != nullptr)
	{
		m_pLoadingUI_Logo->Set_Percent(m_fLoading_Per);
		m_pLoadingUI_Logo->Update(fTimeDelta);

	}
	m_pLoadingUI_GameTitle->Update(fTimeDelta);
	
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

		case LEVEL_IMGUI:
			hr = m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_ImGui::Create(m_pDevice, m_pContext));
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
	m_pBackGround = CBackGround::Create(m_pDevice, m_pContext);
	CBackGround::UIOBJECT_DESC	Desc{};
	Desc.eLevel = LEVEL_LOADING;
	Desc.fX = g_iWinSizeX * 0.5f;
	Desc.fY = g_iWinSizeY * 0.5f;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;
	Desc.iData = 10;
	Desc.fDepth = 0.5f;
	m_pBackGround->Initialize(&Desc);
	return S_OK;
}
HRESULT CLevel_Loading::Ready_Layer_UI_Loading(const _tchar* pLayerTag)
{
	m_pLoadingUI = CLoading_UI::Create(m_pDevice, m_pContext);

	CLoading_UI::LOADINGUI_DESC	Desc{};
	Desc.eLevel = LEVEL_LOADING;
	Desc.fX = g_iWinSizeX * 0.85f;
	Desc.fY = g_iWinSizeY * 0.75f;
	Desc.fSizeX = 270;
	Desc.fSizeY = 270;
	Desc.iData = 10;
	Desc.fDepth = 0.1f;
	Desc.eTag = CLoading_UI::LOADING_GAGE;
	m_pLoadingUI->Initialize(&Desc);
	return S_OK;

		
}
HRESULT CLevel_Loading::Ready_Layer_UI_LOGO(const _tchar* pLayerTag)
{
	m_pLoadingUI_Logo = CLoading_UI::Create(m_pDevice, m_pContext);

	CLoading_UI::LOADINGUI_DESC	Desc{};
	Desc.eLevel = LEVEL_LOADING;
	Desc.fX = g_iWinSizeX * 0.84f;
	Desc.fY = g_iWinSizeY * 0.75f;
	Desc.fSizeX = 180;
	Desc.fSizeY = 150;
	Desc.iData = 10;
	Desc.fDepth = 0.1f;
	Desc.eTag = CLoading_UI::LOADING_LOGO;
	m_pLoadingUI_Logo->Initialize(&Desc);
	return S_OK;

}

HRESULT CLevel_Loading::Ready_Layer_UI_GameTitle(const _tchar* pLayerTag)
{
	m_pLoadingUI_GameTitle = CLoading_UI::Create(m_pDevice, m_pContext);

	CLoading_UI::LOADINGUI_DESC	Desc{};
	Desc.eLevel = LEVEL_LOADING;
	Desc.fX = g_iWinSizeX * 0.5;
	Desc.fY = g_iWinSizeY * 0.3;
	Desc.fSizeX = 500;
	Desc.fSizeY = 100;
	Desc.iData = 10;
	Desc.fDepth = 0.f;
	Desc.eTag = CLoading_UI::LOADING_GAMENAME;
	m_pLoadingUI_GameTitle->Initialize(&Desc);

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

	Safe_Release(m_pLoadingUI_GameTitle);
	Safe_Release(m_pLoadingUI_Logo);
	Safe_Release(m_pLoadingUI);
	Safe_Release(m_pLoader);
	Safe_Release(m_pBackGround);

}
