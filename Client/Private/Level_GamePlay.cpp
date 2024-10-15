#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"
#include "Level_Loading.h"
#include "GameInstance.h"

#include "CrossLine.h"
#include "InGameUI.h"
#include "Terrain.h"
#include "Monster.h"
#include "Environment.h"
#include "Weapon.h"
#include <Tank.h>
#include "Helicopter.h"
#include <Alien.h>
#include <Pony.h>


CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;


	 
	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_PlayerBuild(TEXT("Layer_PlayerBuild"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;



	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_WeaponITem(TEXT("Layer_WeaponItem"))))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_UI_MACHINE_HP(TEXT("Layer_UIHp"))))
		return E_FAIL;


	if (FAILED(Ready_Layer_UI_Button(TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;



	// Load_Map();

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	
		

	Interaction_Weapon();

	if (m_pGameInstance->Get_DIKeyState_Down(DIK_ESCAPE))
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

void CLevel_GamePlay::Interaction_Weapon()
{
	_vector vPlayerPos = m_pPlayer->Get_Position();
	_float3 fPlayerPos{}, fWeaponPos{};
	XMStoreFloat3(&fPlayerPos, vPlayerPos);
	for (int i = 0; i < 2; i++)
	{
	_vector vWeaponPos = m_pWeaponItem[i]->Get_Position();
	XMStoreFloat3(&fWeaponPos, vWeaponPos);
	if (((fPlayerPos.x - fWeaponPos.x) * (fPlayerPos.x - fWeaponPos.x) + (fPlayerPos.y - fWeaponPos.y) * (fPlayerPos.y - fWeaponPos.y) + (fPlayerPos.z - fWeaponPos.z) * (fPlayerPos.z - fWeaponPos.z)) <= 50.f)
	{

		m_pWeaponItem[i]->Set_Interation(true);
		if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_E))
		{
			m_pWeaponItem[i]->Set_Charging(true); // 아이템에서 차징중임을 알려줌
			
		}
		else
		{
			m_pWeaponItem[i]->Set_Charging(false);
		
		}


		// 플레이어에게 장착된 장비가 무엇인지 알려줌
		_bool bEquip{};
		_uint iEuquipNum{};
		m_pWeaponItem[i]->Set_WeaponItem_Equip(bEquip, iEuquipNum);
		if (bEquip == true)
		{
			m_pPlayer->Set_EquipNumber(iEuquipNum);
		}
	}
	else
	{
		
		m_pWeaponItem[i]->Set_Charging(false);
		m_pWeaponItem[i]->Set_Interation(false);
	}
}
	_int iCheck = 0;
	for(int i = 0; i< 2; i++)
	{
		if (m_pWeaponItem[i]->Get_Charging() == true)
			iCheck++;
	}
	if(iCheck > 0)
		m_pGuage->Set_Charging(true);
	else
		m_pGuage->Set_Charging(false);
}

HRESULT CLevel_GamePlay::Ready_Layer_UI_MACHINE_HP(const _tchar* pLayerTag)
{
	//CInGameUI::UIOBJECT_DESC			Desc{};

	//Desc.fX = 1200;
	//Desc.fY = 640;
	//Desc.fSizeX = 80.f;
	//Desc.fSizeY = 80.f;
	//Desc.iData = 0;

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &Desc)))
	//	return E_FAIL;

	return S_OK;
}




HRESULT CLevel_GamePlay::Ready_Layer_Terrain(const _tchar* pLayerTag)
{

	if(FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CContainerObject::CONTAINEROBJECT_DESC Desc{};
	CGameObject* pPlayer = m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Player"), &Desc);
	m_pPlayer = static_cast<CPlayer*>(pPlayer);

	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_WeaponITem(const _tchar* pLayerTag)
{
	CWeapon_Item::WEAPONITEM_DESC Desc{};
	Desc.eID = LEVEL_GAMEPLAY;
	Desc.iModelIndex = 5;
	Desc.fScale = { 5.f,5.f,5.f };
	Desc.fPosition = { 400.f, 2.f, 245.f };
	CGameObject*  pItem = m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_WeaponItem"), &Desc);
	m_pWeaponItem[0] = static_cast<CWeapon_Item*>(pItem);

	
	Desc.iModelIndex = 7;
	Desc.fPosition = { 380.f, 2.f, 255.f };
	pItem = m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_WeaponItem"), &Desc);
	m_pWeaponItem[1] = static_cast<CWeapon_Item*>(pItem);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_PlayerBuild(const _tchar* pLayerTag)
{
	CBrainCore::BRAIN_CORE_DESC pDesc{};
	pDesc.eID = LEVEL_GAMEPLAY;
	pDesc.fPosition = _float3(408.f, 0.1f, 220.f);
	pDesc.fScale = { 2.f,2.f,2.f };
	pDesc.iModelComponentIndex = 0;
	
	m_pBrain = static_cast<CBrainCore*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_BrainCore"), &pDesc));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC			Desc{};

	Desc.vEye = _float4(418.755f, 1.5f, 245.710f, 1.f);
	Desc.fPosition = _float3(418.755f, 1.5f, 245.710f);
	Desc.vAt = _float4(0.f, 0.f, 1.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNearZ = 0.1f;
	Desc.fFar = 500.f;
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	Desc.fSpeedPerSec = 20.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fMouseSensor = 0.1f;
	Desc.eLevel = LEVEL_GAMEPLAY;
	Desc.matPlayerWorld = m_pPlayer->Get_Transform()->Get_WorldMatrixPtr();
	Desc.m_vecTPS_CamPos = m_pPlayer->Get_TPSPosptr();
	Desc.m_vecFPS_CamPos = m_pPlayer->Get_FPSPosptr();
	Desc.iViewState = m_pPlayer->Get_ViewState();
	Desc.m_vecWeaponPos = m_pPlayer->Get_WeaponPos();
	Desc.m_vecWeaponDir = m_pPlayer->Get_WeaponDir();
	m_pCamera = static_cast<CCamera_Free*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &Desc));
	m_pPlayer->Set_CameraAt(m_pCamera->Get_Camera_At());
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

	CTank::TANK_DESC Tank_Desc{};
	Tank_Desc.eID = LEVEL_GAMEPLAY;
	Tank_Desc.fPosition = _float3(400.755f, 0.f, 255.710f);
	Tank_Desc.fSpeedPerSec = 5.f;
	Tank_Desc.fScale = _float3(1.f, 1.f, 1.f);
	Tank_Desc.iModelComponentIndex = ANIM_TANK;
	Tank_Desc.vecTargetPos = m_pBrain->Get_BrainPos();
	Tank_Desc.matBrainCoreWorld = m_pBrain->Get_Transform()->Get_WorldMatrixPtr();
	Tank_Desc.matPlayerWorld = m_pPlayer->Get_Transform()->Get_WorldMatrixPtr();
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag,
		TEXT("Prototype_GameObject_Tank"), &Tank_Desc)))
		return E_FAIL;
	
	CHelicopter::HELICOPTER_DESC Helicopter_Desc{};
	Helicopter_Desc.eID = LEVEL_GAMEPLAY;
	Helicopter_Desc.fPosition = _float3(380.755f, 5.f,300.710f);
	Helicopter_Desc.fSpeedPerSec = 10.f;
	Helicopter_Desc.fScale = _float3(1.f, 1.f, 1.f);
	Helicopter_Desc.iModelComponentIndex = ANIM_HELICOPTER;
	Helicopter_Desc.vecTargetPos = m_pBrain->Get_BrainPos();
	Helicopter_Desc.matBrainCoreWorld = m_pBrain->Get_Transform()->Get_WorldMatrixPtr();
	Helicopter_Desc.matPlayerWorld = m_pPlayer->Get_Transform()->Get_WorldMatrixPtr();
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag,
		TEXT("Prototype_GameObject_Helicopter"), &Helicopter_Desc)))
		return E_FAIL;

	CAlien::ALIEN_DESC Alien_Desc{};
	Alien_Desc.eID = LEVEL_GAMEPLAY;
	Alien_Desc.fPosition = _float3(400.f, 3.f, 300.710f);
	Alien_Desc.fSpeedPerSec = 10.f;
	Alien_Desc.fScale = _float3(0.8f, 0.8f, 0.8f);
	Alien_Desc.iModelComponentIndex = ANIM_ALIEN;
	Alien_Desc.vecTargetPos = m_pBrain->Get_BrainPos();
	Alien_Desc.matBrainCoreWorld = m_pBrain->Get_Transform()->Get_WorldMatrixPtr();
	Alien_Desc.matPlayerWorld = m_pPlayer->Get_Transform()->Get_WorldMatrixPtr();
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag,
		TEXT("Prototype_GameObject_Alien"), &Alien_Desc)))
		return E_FAIL;

	CPony::PONY_DESC Pony_Desc{};
	Pony_Desc.eID = LEVEL_GAMEPLAY;
	Pony_Desc.fPosition = _float3(410.f, 1.f, 310.710f);
	Pony_Desc.fSpeedPerSec = 10.f;
	Pony_Desc.fScale = _float3(0.8f, 0.8f, 0.8f);
	Pony_Desc.iModelComponentIndex = ANIM_PONY;
	Pony_Desc.vecTargetPos = m_pBrain->Get_BrainPos();
	Pony_Desc.matBrainCoreWorld = m_pBrain->Get_Transform()->Get_WorldMatrixPtr();
	Pony_Desc.matPlayerWorld = m_pPlayer->Get_Transform()->Get_WorldMatrixPtr();
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag,
		TEXT("Prototype_GameObject_Alien"), &Pony_Desc)))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Load_Map()
{
	HANDLE hFile = CreateFile(L"../Bin/Data/Environment.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Environment File Failed", L"Error", MB_OK);
		return;
	}
	DWORD dwByte = 0;
	LEVELID iLevel;
	_int  iModelIndex;
	_float3 fPos;
	_float3 fScale;
	_vector	vRight{};
	_vector	vUp{};
	_vector	vLook{};

	while (ReadFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr) && dwByte > 0)
	{

		ReadFile(hFile, &iModelIndex, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &fScale, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &vRight, sizeof(_vector), &dwByte, nullptr);
		ReadFile(hFile, &vUp, sizeof(_vector), &dwByte, nullptr);
		ReadFile(hFile, &vLook, sizeof(_vector), &dwByte, nullptr);

		CEnvironment::ENVIRONMENT_DESC			Desc{};
		Desc.eID = LEVEL_GAMEPLAY;
		Desc.fPosition = fPos;
		Desc.iModelComponentIndex = iModelIndex;
		Desc.fScale = fScale;
		//cout << fScale.x << "     " << fScale.y << "            " << fScale.z << endl;
		CGameObject* pGameObj = (m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_GAMEPLAY, TEXT("Layer_Environment"),
			TEXT("Prototype_GameObject_Environment_ImGui"), &Desc));
		if (pGameObj != nullptr)
		{
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Scale(0.f, fScale.x, fScale.y, fScale.z);
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Rotaion(vRight, vUp, vLook);
		}
	}
	CloseHandle(hFile);

	 hFile = CreateFile(L"../Bin/Data/Build.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Build File Failed", L"Error", MB_OK);
		return;
	}

	while (ReadFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr) && dwByte > 0)
	{
		ReadFile(hFile, &iModelIndex, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &fScale, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &vRight, sizeof(_vector), &dwByte, nullptr);
		ReadFile(hFile, &vUp, sizeof(_vector), &dwByte, nullptr);
		ReadFile(hFile, &vLook, sizeof(_vector), &dwByte, nullptr);

		CEnvironment::ENVIRONMENT_DESC			Desc{};
		Desc.eID = LEVEL_GAMEPLAY;
		Desc.fPosition = fPos;
		Desc.iModelComponentIndex = iModelIndex;
		Desc.fScale = fScale;
		//cout << fScale.x << "     " << fScale.y << "            " << fScale.z << endl;
		CGameObject* pGameObj = (m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_GAMEPLAY, TEXT("Layer_Environment"),
			TEXT("Prototype_GameObject_Environment_ImGui"), &Desc));
		if (pGameObj != nullptr)
		{
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Scale(0.f, fScale.x, fScale.y, fScale.z);
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Rotaion(vRight, vUp, vLook);
		}
	}

	CloseHandle(hFile);
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

	ShowCursor(true);


}



HRESULT CLevel_GamePlay::Ready_Layer_UI_Button(const _tchar* pLayerTag)
{
	CUI_CircleGuage::CIRCLEGAUGE_DESC pCircleDesc{};
	pCircleDesc.eLevel = LEVEL_GAMEPLAY;
	pCircleDesc.fSizeX = 200.f;
	pCircleDesc.fSizeY = 200.f;
	pCircleDesc.iData = 0;
	pCircleDesc.fX = g_iWinSizeX * 0.5f;
	pCircleDesc.fY = g_iWinSizeY * 0.5f;
	pCircleDesc.fDepth = 0.f;
	CGameObject* pGuage= m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Circle_UI"), &pCircleDesc);
	m_pGuage = static_cast<CUI_CircleGuage*>(pGuage);

	CCrossLine::UIOBJECT_DESC			Desc{};
	Desc.fX = g_iWinSizeX * 0.5f;
	Desc.fY = g_iWinSizeY * 0.5f;
	Desc.fSizeX = 20.f;
	Desc.fSizeY = 20.f;
	Desc.iData = 10;
	Desc.fDepth = 0.1f;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_CrossLine"), &Desc)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc{};
	pDesc.eLevel = LEVEL_GAMEPLAY;
	pDesc.eUITag = CInGameUI::UI_F;
	pDesc.fSizeX = 30.f;
	pDesc.fSizeY = 30.f;
	pDesc.iData = 0;
	pDesc.fX = g_iWinSizeX - 75.f;
	pDesc.fY = 500.f;
	pDesc.fDepth = 0.1f;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc1{};
	pDesc1.eLevel = LEVEL_GAMEPLAY;
	pDesc1.eUITag = CInGameUI::UI_V;
	pDesc1.fSizeX = 30.f;
	pDesc1.fSizeY = 30.f;
	pDesc1.iData = 0;
	pDesc1.fX = g_iWinSizeX - 75.f;
	pDesc1.fY = 540.f;
	pDesc1.fDepth = 0.1f;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc1)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc2{};
	pDesc2.eLevel = LEVEL_GAMEPLAY;
	pDesc2.eUITag = CInGameUI::UI_SPACE;
	pDesc2.fSizeX = 70.f;
	pDesc2.fSizeY = 30.f;
	pDesc2.iData = 0;
	pDesc2.fX = g_iWinSizeX - 55.f;
	pDesc2.fY = 460.f;
	pDesc2.fDepth = 0.1f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc2)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc19{};
	pDesc19.eLevel = LEVEL_GAMEPLAY;
	pDesc19.eUITag = CInGameUI::UI_JUMP_ICON;
	pDesc19.fSizeX = 28.f;
	pDesc19.fSizeY = 32.f;
	pDesc19.iData = 0;
	pDesc19.fX = g_iWinSizeX - 110.f;
	pDesc19.fY = 460.f;
	pDesc19.fDepth = 0.1f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc19)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc3{};
	pDesc3.eLevel = LEVEL_GAMEPLAY;
	pDesc3.eUITag = CInGameUI::UI_SHIFT;
	pDesc3.fSizeX = 70.f;
	pDesc3.fSizeY = 30.f;
	pDesc3.iData = 0;
	pDesc3.fX = g_iWinSizeX - 55.f;
	pDesc3.fY = 420.f;
	pDesc3.fDepth = 0.1f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc3)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc18{};
	pDesc18.eLevel = LEVEL_GAMEPLAY;
	pDesc18.eUITag = CInGameUI::UI_RUN_ICON;
	pDesc18.fSizeX = 30.f;
	pDesc18.fSizeY = 30.f;
	pDesc18.iData = 0;
	pDesc18.fX = g_iWinSizeX - 110.f;
	pDesc18.fY = 420.f;
	pDesc18.fDepth = 0.1f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc18)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc20{};
	pDesc20.eLevel = LEVEL_GAMEPLAY;
	pDesc20.eUITag = CInGameUI::UI_MODECHANGE_ICON;
	pDesc20.fSizeX = 30.f;
	pDesc20.fSizeY = 30.f;
	pDesc20.iData = 0;
	pDesc20.fX = g_iWinSizeX - 110.f;
	pDesc20.fY = 500.f;
	pDesc20.fDepth = 0.1f;
	pDesc20.iIndex = 0;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc20)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc21{};
	pDesc21.eLevel = LEVEL_GAMEPLAY;
	pDesc21.eUITag = CInGameUI::UI_PUNCH_ICON;
	pDesc21.fSizeX = 30.f;
	pDesc21.fSizeY = 30.f;
	pDesc21.iData = 0;
	pDesc21.fX = g_iWinSizeX - 110.f;
	pDesc21.fY = 540.f;
	pDesc21.fDepth = 0.1f;
	pDesc21.iIndex = 0;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc21)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc23{};
	pDesc23.eLevel = LEVEL_GAMEPLAY;
	pDesc23.eUITag = CInGameUI::UI_C;
	pDesc23.fSizeX = 30.f;
	pDesc23.fSizeY = 30.f;
	pDesc23.iData = 0;
	pDesc23.fX = g_iWinSizeX - 75.f;
	pDesc23.fY = 580.f;
	pDesc23.fDepth = 0.1f;
	pDesc23.iIndex = 0;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc23)))
		return E_FAIL;


	CInGameUI::INGAMEUI_DESC	pDesc24{};
	pDesc24.eLevel = LEVEL_GAMEPLAY;
	pDesc24.eUITag = CInGameUI::UI_VIEWCHANGE_ICON;
	pDesc24.fSizeX = 30.f;
	pDesc24.fSizeY = 30.f;
	pDesc24.iData = 0;
	pDesc24.fX = g_iWinSizeX - 110.f;
	pDesc24.fY = 580.f;
	pDesc24.fDepth = 0.1f;
	pDesc24.iIndex = 0;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc24)))
		return E_FAIL;


	CInGameUI::INGAMEUI_DESC	pDesc9{};
	pDesc9.eLevel = LEVEL_GAMEPLAY;
	pDesc9.eUITag = CInGameUI::UI_CONVERSATIONBOX;
	pDesc9.fSizeX = 180.f;
	pDesc9.fSizeY = 90.f;
	pDesc9.iData = 0;
	pDesc9.fX = 125.f;
	pDesc9.fY = 100.f;
	pDesc9.fDepth = 0.5f;
	pDesc9.iIndex = 1;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc9)))
		return E_FAIL;


	CInGameUI::INGAMEUI_DESC	pDesc5{};
	pDesc5.eLevel = LEVEL_GAMEPLAY;
	pDesc5.eUITag = CInGameUI::UI_BATTERY;
	pDesc5.fSizeX = 20.f;
	pDesc5.fSizeY = 20.f;
	pDesc5.iData = 0;
	pDesc5.fX = 95.f;
	pDesc5.fY = 120.f;
	pDesc5.fDepth = 0.1f;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc5)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc8{};
	pDesc8.eLevel = LEVEL_GAMEPLAY;
	pDesc8.eUITag = CInGameUI::UI_BATTERY_GAGE;
	pDesc8.fSizeX = 80.f;
	pDesc8.fSizeY = 10.f;
	pDesc8.iData = 0;
	pDesc8.fX = 150;
	pDesc8.fY = 120;

	pDesc8.fDepth = 0.2f;
	pDesc8.iIndex = 4;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc8)))
		return E_FAIL;

	pDesc8.fDepth = 0.1f;
	pDesc8.iIndex = 3;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc8)))
		return E_FAIL;


	CInGameUI::INGAMEUI_DESC	pDesc10{};
	pDesc10.eLevel = LEVEL_GAMEPLAY;
	pDesc10.eUITag = CInGameUI::UI_MACHINE_HP;
	pDesc10.fSizeX = 100.f;
	pDesc10.fSizeY = 10.f;
	pDesc10.iData = 0;
	pDesc10.fX = 140;
	pDesc10.fY = 100;

	pDesc10.fDepth = 0.2f;
	pDesc10.iIndex = 4;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc10)))
		return E_FAIL;

	pDesc10.fDepth = 0.1f;
	pDesc10.iIndex = 1;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc10)))
		return E_FAIL;


	CInGameUI::INGAMEUI_DESC	pDesc11{};
	pDesc11.eLevel = LEVEL_GAMEPLAY;
	pDesc11.eUITag = CInGameUI::UI_MACHINE_ENERGY;
	pDesc11.fSizeX = 100.f;
	pDesc11.fSizeY = 10.f;
	pDesc11.iData = 0;
	pDesc11.fX = 140;
	pDesc11.fY = 80;
	pDesc11.fDepth = 0.3f;
	pDesc11.iIndex = 4;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc11)))
		return E_FAIL;

	pDesc11.fDepth = 0.2f;
	pDesc11.iIndex = 0;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc11)))
		return E_FAIL;


	// 플레이어 정보 뒷 배경
	CInGameUI::INGAMEUI_DESC	pDesc22{};
	pDesc22.eLevel = LEVEL_GAMEPLAY;
	pDesc22.eUITag = CInGameUI::UI_CONVERSATIONBOX;
	pDesc22.fSizeX = 160.f;
	pDesc22.fSizeY = 90.f;
	pDesc22.iData = 0;
	pDesc22.fX = g_iWinSizeX - 95.f;
	pDesc22.fY = g_iWinSizeY - 45.f;
	pDesc22.fDepth = 0.4f;
	pDesc22.iIndex = 1;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc22)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc13{};
	pDesc13.eLevel = LEVEL_GAMEPLAY;
	pDesc13.eUITag = CInGameUI::UI_PLAYER_HP;
	pDesc13.fSizeX = 120.f;
	pDesc13.fSizeY = 20.f;
	pDesc13.iData = 0;
	pDesc13.fX = g_iWinSizeX - 90.f;
	pDesc13.fY = g_iWinSizeY - 45.f;

	pDesc13.fDepth = 0.3f;
	pDesc13.iIndex = 4;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc13)))
		return E_FAIL;

	pDesc13.fDepth = 0.2f;
	pDesc13.iIndex = 1;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc13)))
		return E_FAIL;


	CInGameUI::INGAMEUI_DESC	pDesc14{};
	pDesc14.eLevel = LEVEL_GAMEPLAY;
	pDesc14.eUITag = CInGameUI::UI_PLAYER_ENERGY;
	pDesc14.fSizeX = 120.f;
	pDesc14.fSizeY = 20.f;
	pDesc14.iData = 0;
	pDesc14.fX = g_iWinSizeX - 90.f;
	pDesc14.fY = g_iWinSizeY - 70.f;
	pDesc14.fDepth = 0.3f;
	pDesc14.iIndex = 4;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc14)))
		return E_FAIL;

	pDesc14.fDepth = 0.2f;
	pDesc14.iIndex = 0;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc14)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc15{};
	pDesc15.eLevel = LEVEL_GAMEPLAY;
	pDesc15.eUITag = CInGameUI::UI_ENERGY_ICON;
	pDesc15.fSizeX = 23.f;
	pDesc15.fSizeY = 23.f;
	pDesc15.iData = 0;
	pDesc15.fX = g_iWinSizeX - 160.f;
	pDesc15.fY = g_iWinSizeY - 70.f;
	pDesc15.fDepth = 0.2f;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc15)))
		return E_FAIL;


	CInGameUI::INGAMEUI_DESC	pDesc16{};
	pDesc16.eLevel = LEVEL_GAMEPLAY;
	pDesc16.eUITag = CInGameUI::UI_HP_ICON;
	pDesc16.fSizeX = 15.f;
	pDesc16.fSizeY = 15.f;
	pDesc16.iData = 0;
	pDesc16.fX = g_iWinSizeX - 160.f;
	pDesc16.fY = g_iWinSizeY - 45.f;
	pDesc16.fDepth = 0.2f;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc16)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc17{};
	pDesc17.eLevel = LEVEL_GAMEPLAY;
	pDesc17.eUITag = CInGameUI::UI_CREDIT_ICON;
	pDesc17.fSizeX = 18.f;
	pDesc17.fSizeY = 18.f;
	pDesc17.iData = 0;
	pDesc17.fX = g_iWinSizeX - 160.f;
	pDesc17.fY = g_iWinSizeY - 22.f;
	pDesc17.fDepth = 0.2f;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc17)))
		return E_FAIL;


			
			/*
#ifdef _DEBUG
	CInGameUI::INGAMEUI_DESC	pDesc4{};
	pDesc4.eLevel = LEVEL_GAMEPLAY;
	pDesc4.eUITag = CInGameUI::UI_DEAD;
	pDesc4.fSizeX = 100.f;
	pDesc4.fSizeY = 100.f;
	pDesc4.iData = 0;
	pDesc4.fX = g_iWinSizeX * 0.5f;
	pDesc4.fY = g_iWinSizeY * 0.3f;
	pDesc4.fDepth = 0.1f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc4)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc7{};
	pDesc7.eLevel = LEVEL_GAMEPLAY;
	pDesc7.eUITag = CInGameUI::UI_CONVERSATIONBOX;
	pDesc7.fSizeX = 170.f;
	pDesc7.fSizeY = 60.f;
	pDesc7.iData = 0;
	pDesc7.fX = g_iWinSizeX * 0.55f;
	pDesc7.fY = 65.f;
	pDesc7.fDepth = 0.2f;
	pDesc7.iIndex = 0;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc7)))
		return E_FAIL;

	CInGameUI::INGAMEUI_DESC	pDesc6{};
	pDesc6.eLevel = LEVEL_GAMEPLAY;
	pDesc6.eUITag = CInGameUI::UI_CHARACTER;
	pDesc6.fSizeX = 100.f;
	pDesc6.fSizeY = 100.f;
	pDesc6.iData = 0;
	pDesc6.fX = g_iWinSizeX * 0.6f;
	pDesc6.fY = 65.f;
	pDesc6.fDepth = 0.1f;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc6)))
		return E_FAIL;


	CInGameUI::INGAMEUI_DESC	pDesc12{};
	pDesc12.eLevel = LEVEL_GAMEPLAY;
	pDesc12.eUITag = CInGameUI::UI_DAMAGED;
	pDesc12.fSizeX = g_iWinSizeX;
	pDesc12.fSizeY = g_iWinSizeY;
	pDesc12.iData = 0;
	pDesc12.fX = g_iWinSizeX * 0.5f;
	pDesc12.fY = g_iWinSizeY * 0.5f;
	pDesc12.fDepth = 0.f;
	pDesc12.iIndex = 0;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI"), &pDesc12)))
		return E_FAIL;
#endif
*/

	return S_OK;
}
