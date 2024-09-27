#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"
#include "Level_Loading.h"
#include "GameInstance.h"


#include "CrossLine.h"
#include "InGameUI.h"
#include "Camera_Free.h"
#include "Terrain.h"
#include "Monster.h"


CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI_HP(TEXT("Layer_UIHp"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI_ArmCannon(TEXT("Layer_UIArmCannon"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI_CrossLine(TEXT("Layer_UICrossLine"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;



	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_GAMEPLAY, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOGO))))
			return;
	}
}

HRESULT CLevel_GamePlay::Render()
{
	__super::Render();

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI_HP(const _tchar* pLayerTag)
{
	CInGameUI::UIOBJECT_DESC			Desc{};

	Desc.fX = 1200;
	Desc.fY = 640;
	Desc.fSizeX = 80.f;
	Desc.fSizeY = 80.f;
	Desc.iData = 0;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI_ArmCannon(const _tchar* pLayerTag)
{
	CInGameUI::UIOBJECT_DESC			Desc{};

	Desc.fX = 80;
	Desc.fY = 640;
	Desc.fSizeX = 40.f;
	Desc.fSizeY = 40.f;
	Desc.iData = 1;
	
	Desc.m_iCount = 2;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_ArmCannon"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI_CrossLine(const _tchar* pLayerTag)
{
	CCrossLine::UIOBJECT_DESC			Desc{};
	Desc.fX = g_iWinSizeX * 0.5f;
	Desc.fY = g_iWinSizeY * 0.5f;
	Desc.fSizeX = 20.f;
	Desc.fSizeY = 20.f;
	Desc.iData = 10;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_CrossLine"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Terrain(const _tchar* pLayerTag)
{

	if(FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC			Desc{};

	Desc.vEye = _float4(0.f, 10.f, -5.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNearZ = 0.1f;
	Desc.fFar = 500.f;
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	Desc.fSpeedPerSec = 20.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fMouseSensor = 0.1f;
	Desc.eLevel = LEVEL_GAMEPLAY;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag,
		TEXT("Prototype_GameObject_Camera_Free"), &Desc)))
		return E_FAIL;

	return S_OK;

}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC	LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar* pLayerTag)
{
	for (size_t i = 0; i < 20; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag,
			TEXT("Prototype_GameObject_Monster"))))
			return E_FAIL;
	}
	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

}
