#include "stdafx.h"
#include "..\Public\Monster_Path.h"


#include "Camera_Free.h"
#include "Monster.h"
#include "Level_Loading.h"
#include <Terrain.h>
CMonster_Path::CMonster_Path(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CMonster_Path::Initialize()
{
	ShowCursor(true);
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))			return E_FAIL;	// 카메라 생성
	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))			return E_FAIL;	// 지형 생성
	if (FAILED(Ready_Lights()))										return E_FAIL;	// 빛
	if (FAILED(m_pGameInstance->Close_Level(LEVEL_LOADING)))		return E_FAIL;	// 로딩 닫기


	pVIBuffer_Terrain = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_IMGUI, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));

	// 저장 로드 버튼(이미지 버튼)
	Create_ImageButton();

	return S_OK;
}

void CMonster_Path::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	
}

HRESULT CMonster_Path::Render()
{
	return E_NOTIMPL;
}

HRESULT CMonster_Path::Ready_Layer_Terrain(const _tchar* pLayerTag)
{
	CTerrain::TERRAIN_DESC pDesc{};
	pDesc.eID = LEVEL_MONSTERSPAWN;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_MONSTERSPAWN, pLayerTag, TEXT("Prototype_GameObject_Terrain_MonsterSpawn"), &pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CMonster_Path::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC			Desc{};
	Desc.vEye = _float4(0.f, 10.f, -5.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNearZ = 0.1f;
	Desc.fFar = 500.f;
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fMouseSensor = 0.05f;
	Desc.eLevel = LEVEL_MONSTERSPAWN;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_MONSTERSPAWN, pLayerTag,
		TEXT("Prototype_GameObject_Camera_Free_MonsterSpawn"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Path::Ready_Lights()
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

void CMonster_Path::Create_ImageButton()
{
	m_pSave = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Save.jpg"));
	m_pLoad = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Load.jpg"));

	// 사진의 리소스뷰 가져오기
	my_Savetexture = *m_pSave->Get_SRV().begin();
	my_Loadtexture = *m_pLoad->Get_SRV().begin();
}

void CMonster_Path::ButtonImage_List()
{
	ImGui::BeginChild("Choose Monster");
	int iButton = 0;
	auto& SRVs = m_pMonster->Get_SRV();
	for (auto iter = SRVs.begin(); iter != SRVs.end(); ++iter)
	{
		if (iButton % 4 != 0)
			ImGui::SameLine();
		string tag = "Monster" + to_string(iButton);
		if (ImGui::ImageButton(tag.c_str(), *iter, ImVec2(50, 50), ImVec2(0, 0)))
		{
			m_iModelIndex = iButton;
		}
		iButton++;
	}
	ImGui::EndChild();
}

CMonster_Path* CMonster_Path::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

void CMonster_Path::Free()
{
}
