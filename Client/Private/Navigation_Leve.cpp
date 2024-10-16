#include "stdafx.h"
#include "..\Public\Navigation_Leve.h"
#include "Level_Loading.h"
#include "GameInstance.h"

#include "Environment.h"
#include "Terrain.h"

CNavigation_Leve::CNavigation_Leve(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CNavigation_Leve::Initialize()
{
	ShowCursor(true);
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))			return E_FAIL;	// 카메라 생성
	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))			return E_FAIL;	// 지형 생성
	if (FAILED(Ready_Lights()))										return E_FAIL;	// 빛
	if (FAILED(m_pGameInstance->Close_Level(LEVEL_LOADING)))		return E_FAIL;	// 로딩 닫기

	// 터레인 피킹을 위해 터레인 컴포넌트 가져오기
	pVIBuffer_Terrain = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_NAVIGATION, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));
	Load_Map();


	m_pSave = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Save.jpg"));
	m_pLoad = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Load.jpg"));

	my_Savetexture = *m_pSave->Get_SRV().begin();
	my_Loadtexture = *m_pLoad->Get_SRV().begin();
	return S_OK;
}

void CNavigation_Leve::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (Save == true)
	{
		Save_Navigation(fTimeDelta);
		Save = false;
	}
	if (m_pGameInstance->Get_DIKeyState_Down(DIK_TAB))
	{
		if (eNaviMode == CREATE_NAVIPOINT)
			eNaviMode = SELECT_NAVIPOINT;
		else
			eNaviMode = CREATE_NAVIPOINT;
	}
	// 컨트롤 우클릭은 맨 뒤 삭제
	if ((m_pGameInstance->Get_DIKeyState_Pressing(DIK_LCONTROL)) && eNaviMode == CREATE_NAVIPOINT)
	{
		if ((m_pGameInstance->Get_DIMouseState_Down(DIM_RB)))
		{
			if(m_vecCollision.size() > 0)
			{
				m_vecCollision.back()->Set_Dead();
				m_vecCollision.pop_back();
			}
		}
	}


	if ((m_pGameInstance->Get_DIMouseState_Down(DIM_LB)))
	{
		_float3 fMousePos = m_pGameInstance->Get_MousePos_NDC(g_hWnd, g_iWinSizeX, g_iWinSizeY);
		XMMATRIX invProj = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_PROJ);
		XMMATRIX invView = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW);
		XMVECTOR RayPos, RayDir;
	
			m_pGameInstance->Get_MouseRayDirection(fMousePos, invProj, invView, &RayPos, &RayDir);
	
			RayDir = XMVector3Normalize(RayDir);
			_bool bPickCheck = false;
			// 찍은 위치가 박스와 충돌하면 박스 위치 반환 아니면 찍은 위치 반환
			for (auto& pCollisionBox : m_vecCollision)
			{
				XMFLOAT3 fBoxPos{}, fMinPoint{}, fMaxPoint{};
				fBoxPos = pCollisionBox->Get_PickingPos();
				m_pGameInstance->CreateBoundingBox(fBoxPos, { 0.3f,0.3f, 0.3f }, fMinPoint, fMaxPoint);

				float distance;
				if (m_pGameInstance->Picking_Box(RayPos, RayDir, fMinPoint, fMaxPoint, distance, pCollisionBox->Get_BoundingBox()))
				{
					bPickCheck = true;
					m_fPickingPos = fBoxPos;
					if (eNaviMode == SELECT_NAVIPOINT)
					{
						m_iSelected_index = pCollisionBox->Get_IndexNumber();
					}

					break;
				}
			}
		
		if(bPickCheck == false)
		{
			const _float3* VtxPos = pVIBuffer_Terrain->Get_VtxPos();  // _float3 배열의 시작 주소 반환
			_uint VtxCountX = pVIBuffer_Terrain->Get_VtxCountX();
			_uint VtxCountZ = pVIBuffer_Terrain->Get_VtxCountZ();

			m_fPickingPos = m_pGameInstance->Picking_Terrain(RayPos, RayDir, VtxPos, VtxCountX, VtxCountZ);

			if (eNaviMode == SELECT_NAVIPOINT)
			{
				m_iSelected_index = -1;
			}
		}

		
		if(eNaviMode == CREATE_NAVIPOINT)
		{
			if (m_fPickingPos.x == 0.f && m_fPickingPos.y == 0.f && m_fPickingPos.z == 0.f)
			{
			}
			else
			{
				Add_Point(fTimeDelta, m_fPickingPos);
				m_iCount++;
				if (m_iCount == 3)
					m_iCount = 0;
			}
		}
		
	}

	
	// 선택모드일 때
	if (eNaviMode == SELECT_NAVIPOINT)
	{
		if (m_iSelected_index != -1)
		{
			auto pSelected = m_vecCollision[m_iSelected_index];
			
			if ((m_pGameInstance->Get_DIKeyState_Pressing(DIK_LCONTROL)))
			{
				if ((m_pGameInstance->Get_DIMouseState_Down(DIM_RB)))
				{
					if (m_vecCollision.size() > 0)
					{
						// 선택된 객체 삭제
						pSelected->Set_Dead();
						m_vecCollision.erase(m_vecCollision.begin() + m_iSelected_index);
						m_iSelected_index = -1;
						// 인덱스 번호 새로 부여하기
						int i = 0;
						for (auto& pCol : m_vecCollision)
						{
							pCol->Set_IndexNumber(i);
							++i;
						}

					}
				}
			}
		}
	}



	// 나가기
	if (m_pGameInstance->Get_DIKeyState_Down(DIK_ESCAPE))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_NAVIGATION, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOGO))))
			return;
	}


}

HRESULT CNavigation_Leve::Render()
{
	__super::Render();
//	ImGui::SetNextWindowSize(ImVec2(300, 300)); // 가로 400, 세로 600 크기로 설정
	ImGui::SetNextWindowSizeConstraints(ImVec2(20, 10), ImVec2(400, 400)); // 최소 크기 200x200, 최대 크기 800x600
	ImGui::Begin("ParentWindow", nullptr, ImGuiWindowFlags_None); // 창 이동 가능
	
	if (eNaviMode == CREATE_NAVIPOINT)
	{
		ImGui::Text("Create Mode");
		ImGui::Text(" ");		ImGui::Text(" ");
	}
	else
	{
		ImGui::Text("Select Mode");
		ImGui::Text(" ");		ImGui::Text(" ");
	}
	if (ImGui::BeginListBox("Positions"))
	{
		_uint i = 0;
		for (auto& pCol : m_vecCollision)
		{
			bool isSelected = (i == m_iSelected_index);

			if (ImGui::Selectable(std::to_string(i).c_str(), isSelected))
			{
				m_iSelected_index = i;	// 선택된 인덱스 저장
			}
			if (m_iSelected_index == pCol->Get_IndexNumber())
			{
				pCol->Set_PickingCheck(true);
			}
			else
			{
				pCol->Set_PickingCheck(false);
			}
			
			ImGui::Text("X=%.2f, Y=%.2f, Z=%.2f", pCol->Get_PickingPos().x, pCol->Get_PickingPos().y, pCol->Get_PickingPos().z);
			i++;
		}

		// 리스트 박스 끝
		ImGui::EndListBox();
	}


	ImGui::End();


	ImGui::SetNextWindowSize(ImVec2(300, 100)); // 가로 400, 세로 300 크기로 설정
	ImGui::SetNextWindowSizeConstraints(ImVec2(30, 30), ImVec2(150, 100)); // 최소 크기 200x200, 최대 크기 800x600
	ImGui::Begin("Save_Load", nullptr, ImGuiWindowFlags_None);
	if (ImGui::ImageButton("Save", my_Savetexture, ImVec2(50, 50), ImVec2(0, 0)))
	{
		Save = true;

	}
	ImGui::SameLine();
	if (ImGui::ImageButton("Load", my_Loadtexture, ImVec2(50, 50), ImVec2(0, 0)))
	{
		
	}
	ImGui::End();


#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Navigation레벨입니다."));
#endif

	return S_OK;
}

void CNavigation_Leve::Add_Point(_float fTimeDelta, _float3 fPointPos)
{
	// m_iCount 개수로 몇 번째 점인지 구분한다.
	switch (m_iCount)
	{
	case 0:
	{
		CCollisionBox::COLLISIONBOX_DESC CollisionDesc{};
		CollisionDesc.iImGuiMode = NAVIGATION;
		CollisionDesc.eLevel = LEVEL_NAVIGATION;
		CollisionDesc.iPoint_Number = 0;		// 배열의 뒷자리 숫자
		CollisionDesc.fPosition = fPointPos;   // 점 위치 
		CollisionDesc.iIndexNumber = m_iIndex;
		m_vecCollision.push_back(static_cast<CCollisionBox*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_NAVIGATION, TEXT("Layer_Collision"), TEXT("Prototype_GameObject_Collision_Box"), &CollisionDesc)));
		m_iIndex++;
		vPoints[0] = { fPointPos.x,fPointPos.y,fPointPos.z };
		break;
	}
	case 1:
	{
		CCollisionBox::COLLISIONBOX_DESC CollisionDesc{};
		CollisionDesc.iImGuiMode = NAVIGATION;
		CollisionDesc.eLevel = LEVEL_NAVIGATION;
		CollisionDesc.iPoint_Number = 1;
		CollisionDesc.fPosition = fPointPos;
		CollisionDesc.iIndexNumber = m_iIndex;
		m_vecCollision.push_back(static_cast<CCollisionBox*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_NAVIGATION, TEXT("Layer_Collision"), TEXT("Prototype_GameObject_Collision_Box"), &CollisionDesc)));
		m_iIndex++;
		vPoints[1] = { fPointPos.x,fPointPos.y,fPointPos.z };
		break;
	}
	case 2:
	{
		CCollisionBox::COLLISIONBOX_DESC CollisionDesc{};
		CollisionDesc.iImGuiMode = NAVIGATION;
		CollisionDesc.eLevel = LEVEL_NAVIGATION;
		CollisionDesc.iPoint_Number = 2;
		CollisionDesc.fPosition = fPointPos;
		CollisionDesc.iIndexNumber = m_iIndex;
		m_vecCollision.push_back(static_cast<CCollisionBox*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_NAVIGATION, TEXT("Layer_Collision"), TEXT("Prototype_GameObject_Collision_Box"), &CollisionDesc)));
		m_iIndex++;
		vPoints[2] = { fPointPos.x,fPointPos.y,fPointPos.z };
		
		//if (m_pGameInstance->Find_Prototype_Component(LEVEL_NAVIGATION, TEXT("Prototype_Component_Navigation")) != nullptr)
		//{
		//	static_cast<CNavigation*>(m_pGameInstance->Find_Prototype_Component(LEVEL_NAVIGATION, TEXT("Prototype_Component_Navigation")))->Create_Cell(vPoints);
		//}
		break;
	}

	default:
		break;
	}


}

void CNavigation_Leve::Select_Point(_float fTimeDelta, _float3 fPointPos)
{




}

HRESULT CNavigation_Leve::Save_Navigation(_float fTimeDelta)
{
	_ulong		dwByte = { 0 };
	HANDLE		hFile = CreateFile(TEXT("../Bin/Data/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[3];
	
	for (auto& pCol : m_vecCollision)
	{
		_uint iArray_index = pCol->Get_ArrayNumber();
		vPoints[iArray_index] = pCol->Get_PickingPos();
		if (iArray_index == 2)
		{
			WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		}
	}
	CloseHandle(hFile);
	MessageBox(NULL, L"Environment Saved Successfully", L"Success", MB_OK);

	return S_OK;
}



CNavigation_Leve* CNavigation_Leve::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNavigation_Leve* pInstance = new CNavigation_Leve(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CNavigation_Leve");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation_Leve::Free()
{
	__super::Free();
	Safe_Release(m_pSave);
	Safe_Release(m_pLoad);

}





HRESULT CNavigation_Leve::Ready_Layer_Terrain(const _tchar* pLayerTag)
{
	CTerrain::TERRAIN_DESC pDesc{};
	pDesc.eID = LEVEL_NAVIGATION;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_NAVIGATION, pLayerTag, TEXT("Prototype_GameObject_Terrain"), &pDesc)))
		return E_FAIL;
	return S_OK;
}
HRESULT CNavigation_Leve::Ready_Layer_Camera(const _tchar* pLayerTag)
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
	Desc.eLevel = LEVEL_NAVIGATION;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_NAVIGATION, pLayerTag,
		TEXT("Prototype_GameObject_Camera_Free"), &Desc)))
		return E_FAIL;

	return S_OK;
}
HRESULT CNavigation_Leve::Ready_Lights()
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
void CNavigation_Leve::Load_Map()
{
	HANDLE hFile = CreateFile(L"../Bin/Data/Environment.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Environment File Failed", L"Error", MB_OK);
		return;
	}
	DWORD dwByte = 0;
	LEVELID iLevel;
	_uint iImGuiMode{};
	_int  iModelIndex{};
	_float3 fPos{}, fCollisionBoxScale{}, fScale{};
	_vector	vRight{}, vUp{}, vLook{}, vecCollisionPos{};


	while (ReadFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr) && dwByte > 0)
	{

		ReadFile(hFile, &iModelIndex, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &fScale, sizeof(_float3), &dwByte, nullptr);

		ReadFile(hFile, &fCollisionBoxScale, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &iImGuiMode, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &vecCollisionPos, sizeof(_vector), &dwByte, nullptr);

		ReadFile(hFile, &vRight, sizeof(_vector), &dwByte, nullptr);
		ReadFile(hFile, &vUp, sizeof(_vector), &dwByte, nullptr);
		ReadFile(hFile, &vLook, sizeof(_vector), &dwByte, nullptr);

		CEnvironment::ENVIRONMENT_DESC			Desc{};
		Desc.eID = LEVEL_NAVIGATION;
		Desc.fPosition = fPos;
		Desc.iModelComponentIndex = iModelIndex;
		Desc.fScale = fScale;
		//cout << fScale.x << "     " << fScale.y << "            " << fScale.z << endl;
		CGameObject* pGameObj = (m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_NAVIGATION, TEXT("Layer_Environment"),
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

		ReadFile(hFile, &fCollisionBoxScale, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &iImGuiMode, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &vecCollisionPos, sizeof(_vector), &dwByte, nullptr);

		ReadFile(hFile, &vRight, sizeof(_vector), &dwByte, nullptr);
		ReadFile(hFile, &vUp, sizeof(_vector), &dwByte, nullptr);
		ReadFile(hFile, &vLook, sizeof(_vector), &dwByte, nullptr);

		CEnvironment::ENVIRONMENT_DESC			Desc{};
		Desc.eID = LEVEL_NAVIGATION;
		Desc.fPosition = fPos;
		Desc.iModelComponentIndex = iModelIndex;
		Desc.fScale = fScale;
		//cout << fScale.x << "     " << fScale.y << "            " << fScale.z << endl;
		CGameObject* pGameObj = (m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_NAVIGATION, TEXT("Layer_Environment"),
			TEXT("Prototype_GameObject_Environment_ImGui"), &Desc));
		if (pGameObj != nullptr)
		{
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Scale(0.f, fScale.x, fScale.y, fScale.z);
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Rotaion(vRight, vUp, vLook);
		}
	}

	CloseHandle(hFile);
}
