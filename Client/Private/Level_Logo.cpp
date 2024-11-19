#include "stdafx.h"
#include "..\Public\Level_Logo.h"

#include "Level_Loading.h"

#include "GameInstance.h"

#include "BackGround.h"
#include "CrossLine.h"
#include "InGameUI.h"
#include "MenuUI.h"


CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{

	if (FAILED(Ready_Layer_Menu_UI(TEXT("Layer_Menu_UI"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Menu_BackGround(TEXT("Layer_Menu_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_GameTitle(TEXT("Layer_Menu_GameTitle"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Close_Level(LEVEL_LOADING)))
		return E_FAIL;


	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	ShowCursor(TRUE);
	if (GetKeyState(VK_NUMPAD1) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_YARD))))
			return;
		return;
			
	}
	if (GetKeyState(VK_NUMPAD2) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_IMGUI))))
			return;
		return;
	}
	if (GetKeyState(VK_NUMPAD3) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_NAVIGATION))))
			return;
		return;
	}
	if (GetKeyState(VK_NUMPAD4) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_MONSTERSPAWN))))
			return;
		return;
	}
	if(m_pButton_GamePlay != nullptr)
	{
		if (true == dynamic_cast<CButtonUI*> (m_pButton_GamePlay)->Get_bClick())
		{
			if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
				return;
			return;
		}
	}
	if (m_pButton_ImGui != nullptr)
	{
		if (true == dynamic_cast<CButtonUI*> (m_pButton_ImGui)->Get_bClick())
		{
			if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_IMGUI))))
				return;
			return;
		}
	}
}

HRESULT CLevel_Logo::Render()
{
	__super::Render();

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Menu_BackGround(const _tchar* pLayerTag)
{
	CMenuUI::MENUUI_DESC			Desc{};
	Desc.fX = g_iWinSizeX * 0.5f;
	Desc.fY = g_iWinSizeY * 0.5f;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;
	Desc.iData = 10;
	Desc.eLevel = LEVEL_LOGO;
	Desc.fDepth = 0.2f;
	Desc.eTag = CMenuUI::LOGO_BACKGOUND;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_BackGround_Menu"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Menu_UI(const _tchar* pLayerTag)
{
	CButtonUI::BUTTONUI_DESC			Desc{};
	Desc.fX = 250;
	Desc.fY = 300;
	Desc.fSizeX = 300;
	Desc.fSizeY = 50;
	Desc.iData = 10;
	Desc.fDepth = 0.0f;
	Desc.eTag = CButtonUI::BUTTON_PLAY;
	(m_pButton_GamePlay = m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_ButtonUI_Menu"), &Desc));
	// GmaePlay

	Desc.fX = 250;
	Desc.fY = 400;
	Desc.fSizeX = 300;
	Desc.fSizeY = 50;
	Desc.iData = 10;
	Desc.fDepth = 0.0f;
	Desc.eTag = CButtonUI::BUTTON_CREATE;
	(m_pButton_ImGui = m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_ButtonUI_Menu"), &Desc));
	// ImGui

	Desc.fX = 250;
	Desc.fY = 500;
	Desc.fSizeX = 300;
	Desc.fSizeY = 50;
	Desc.iData = 10;
	Desc.fDepth = 0.0f;
	Desc.eTag = CButtonUI::BUTTON_MINI;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_ButtonUI_Menu"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_GameTitle(const _tchar* pLayerTag)
{

	CMenuUI::MENUUI_DESC	Desc{};
	Desc.eLevel = LEVEL_LOGO;
	Desc.fX = g_iWinSizeX * 0.35;
	Desc.fY = g_iWinSizeY * 0.2;
	Desc.fSizeX = 700;
	Desc.fSizeY = 150;
	Desc.iData = 10;
	Desc.fDepth = 0.1f;
	Desc.eTag = CMenuUI::LOGO_GAMENAME;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_GameTitle"), &Desc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();

}
