#include "stdafx.h"
#include "..\Public\Navigation_Leve.h"
#include "Level_Loading.h"
#include "GameInstance.h"

#include "Environment.h"


CNavigation_Leve::CNavigation_Leve(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CNavigation_Leve::Initialize()
{
	// 네비게이션


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

	HANDLE hFile = CreateFile(L"../Bin/Data/Navigation.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Environment File Failed", L"Error", MB_OK);
		return E_FAIL;
	}
	DWORD dwByte = 0;


	while(ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr) && dwByte > 0)
	{
		_vector vA = XMVectorSet(vPoints[0].x, vPoints[0].y, vPoints[0].z, 1.f);
		_vector vB = XMVectorSet(vPoints[1].x, vPoints[1].y, vPoints[1].z, 1.f);
		_vector vC = XMVectorSet(vPoints[2].x, vPoints[2].y, vPoints[2].z, 1.f);
		_vector vCross = XMVector3Cross(vB - vA, vC - vB);
		_float fDot{};
		_vector vUp = { 0.f,1.f,0.f,0.f };

		// 잘못된 순서로 찍은 삼각형의 위치 변경
		XMVECTOR vDot = XMVector3Dot(vCross, vUp);
		XMStoreFloat(&fDot, vDot);
		if (fDot < 0)
		{
			_float3 fNewB{}, fNewC{};
			XMStoreFloat3(&fNewB, vC);
			XMStoreFloat3(&fNewC, vB);
			vPoints[1] = fNewB;
			vPoints[2] = fNewC;
		}
	
		// 삼각형의 각 꼭짓점에 콜리전 박스 생성
		CCollisionBox::COLLISIONBOX_DESC CollisionDesc{};
		CollisionDesc.iImGuiMode = NAVIGATION;
		CollisionDesc.eLevel = LEVEL_NAVIGATION;
		CollisionDesc.iPoint_Number = 0;		// 배열의 뒷자리 숫자
		CollisionDesc.fPosition = vPoints[0];   // 점 위치 
		fPoints[0] = vPoints[0].x;		fPoints[1] = vPoints[0].y; 		fPoints[2] = vPoints[0].z;
		CollisionDesc.iIndexNumber = m_iIndex;
		m_vecCollision.push_back(static_cast<CCollisionBox*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_NAVIGATION, TEXT("Layer_Collision"), TEXT("Prototype_GameObject_Collision_Box"), &CollisionDesc)));
		++m_iIndex;

		CollisionDesc.iPoint_Number = 1;		// 배열의 뒷자리 숫자
		CollisionDesc.fPosition = vPoints[1];   // 점 위치 
		fPoints[0] = vPoints[1].x;		fPoints[1] = vPoints[1].y; 		fPoints[2] = vPoints[1].z;
		CollisionDesc.iIndexNumber = m_iIndex;
		m_vecCollision.push_back(static_cast<CCollisionBox*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_NAVIGATION, TEXT("Layer_Collision"), TEXT("Prototype_GameObject_Collision_Box"), &CollisionDesc)));
		++m_iIndex;
		
		CollisionDesc.iPoint_Number = 2;		// 배열의 뒷자리 숫자
		CollisionDesc.fPosition = vPoints[2];   // 점 위치 
		fPoints[0] = vPoints[2].x;		fPoints[1] = vPoints[2].y; 		fPoints[2] = vPoints[2].z;
		CollisionDesc.iIndexNumber = m_iIndex;
		m_vecCollision.push_back(static_cast<CCollisionBox*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_NAVIGATION, TEXT("Layer_Collision"), TEXT("Prototype_GameObject_Collision_Box"), &CollisionDesc)));
		++m_iIndex;

	}
	m_iCount = 0;
	m_bClick = true;
	m_bAfter_AddPoints = false;
	CloseHandle(hFile);

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
		{
			eNaviMode = CREATE_NAVIPOINT;
			if(m_vecCollision.size() > 0)
			{
				fPoints[0] = m_vecCollision.back()->Get_PickingPos().x;
				fPoints[1] = m_vecCollision.back()->Get_PickingPos().y;
				fPoints[2] = m_vecCollision.back()->Get_PickingPos().z;
			}
		}
	}
	if (m_pGameInstance->Get_DIKeyState_Down(DIK_RETURN))
	{
		// 클릭할 수 있는 상태로 바꿔준다.
		if (eNaviMode == CREATE_NAVIPOINT)
		{
			if(m_bAfter_AddPoints == true)
			{
				if (m_iCount < 2)
					m_iCount++;
				m_bDelete = true;
			}
			
		}
		m_bClick = true;
	}
	// 컨트롤 우클릭은 맨 뒤 삭제
	if ((m_pGameInstance->Get_DIKeyState_Pressing(DIK_LCONTROL)) && eNaviMode == CREATE_NAVIPOINT && m_bDelete == true)
	{
		if ((m_pGameInstance->Get_DIMouseState_Down(DIM_RB)))
		{
			if(m_vecCollision.size() > 0 && m_iCount >= 0)
			{
				m_vecCollision.back()->Set_Dead();
				m_vecCollision.pop_back();
				--m_iCount;
				
				if (m_iCount < 0)
				{
					m_iCount = 0;
					fPoints[0] =m_vecCollision.back()->Get_PickingPos().x;
					fPoints[1] =m_vecCollision.back()->Get_PickingPos().y;
					fPoints[2] =m_vecCollision.back()->Get_PickingPos().z;
					m_bAfter_AddPoints = false;
					m_bDelete = false; // 0번 인덱스 삭제한 뒤에는 삭제하면 안됨
				}
				else
				{
					fPoints[0] = vPoints[m_iCount].x;
					fPoints[1] = vPoints[m_iCount].y;
					fPoints[2] = vPoints[m_iCount].z;
				}
				--m_iIndex;
			}
		}
	}

	if (eNaviMode == CREATE_NAVIPOINT )
	{
		if(m_bClick == false)
		{
			m_vecCollision.back()->Set_Position(XMVectorSet(fPoints[0], fPoints[1], fPoints[2], 1.f));
			vPoints[m_iCount].x = fPoints[0];
			vPoints[m_iCount].y = fPoints[1];
			vPoints[m_iCount].z = fPoints[2];

		}
	}

	if (m_pGameInstance->Get_DIMouseState_Down(DIM_LB) && m_bClick == true)
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
				m_bClick = false;
			}
		}
	}

	// 스페이스바 누르면 셀 생성
	if (m_pGameInstance->Get_DIKeyState_Down(DIK_SPACE))
	{
		// Y가 0일 때 0.1f로 올려줌
		if (vPoints[2].x != 0.f && vPoints[2].z != 0.f)
		{
			for (int i = 0; i < 3; i++)
			{
				if (vPoints[i].y == 0.f)
					vPoints[i].y = 0.2f;
			}
			// 셀 생성 해줌
			m_pTerrain->Get_NaviCom()->Create_Cell(vPoints);
			for (int i = 0; i < 3; i++)
			{
				// 생성후 포인트 배열 초기화 해주기
				vPoints[i].x = 0.f;
				vPoints[i].y = 0.f;
				vPoints[i].z = 0.f;
			}
			m_bAfter_AddPoints = false;
			m_bClick = false;
			m_bDelete = false;
			m_iCount = 0;
		}
	}
	
	// 선택모드일 때
	if (eNaviMode == SELECT_NAVIPOINT)
	{
		if (m_iSelected_index != -1 && m_vecCollision.size() > 0)
		{
			if (m_iSelected_index > m_iSelected_index)
			{
				--m_iSelected_index;
				return;
			}
			auto pSelected = m_vecCollision[m_iSelected_index];
			// 위치 변경이 되도록

			// 삭제
			if ((m_pGameInstance->Get_DIKeyState_Pressing(DIK_LCONTROL)))
			{
				if ((m_pGameInstance->Get_DIMouseState_Down(DIM_RB)))
				{
					if (m_vecCollision.size() > 0)
					{
						// 선택된 객체 삭제
						_uint iNum = pSelected->Get_ArrayNumber();
						if (iNum == 0)
						{
							m_pTerrain->Get_NaviCom()->Delete_Cell(m_iSelected_index / 3);
							pSelected->Set_Dead();
							m_vecCollision.erase(m_vecCollision.begin() + m_iSelected_index);

							pSelected = m_vecCollision[m_iSelected_index];
							pSelected->Set_Dead();
							m_vecCollision.erase(m_vecCollision.begin() + m_iSelected_index);
							
							pSelected = m_vecCollision[m_iSelected_index];
							pSelected->Set_Dead();
							m_vecCollision.erase(m_vecCollision.begin() + m_iSelected_index);
							m_iIndex -= 3;
							if (m_iIndex < 0)
								m_iIndex = 0;
						}
						else if (iNum == 1)
						{
							m_pTerrain->Get_NaviCom()->Delete_Cell((m_iSelected_index - 1) / 3);
							pSelected->Set_Dead();
							m_vecCollision.erase(m_vecCollision.begin() + m_iSelected_index);

							pSelected = m_vecCollision[m_iSelected_index];
							pSelected->Set_Dead();
							m_vecCollision.erase(m_vecCollision.begin() + m_iSelected_index);

							--m_iSelected_index;
							pSelected = m_vecCollision[m_iSelected_index];
							pSelected->Set_Dead();
							m_vecCollision.erase(m_vecCollision.begin() + m_iSelected_index);
							m_iIndex -= 3;
							if (m_iIndex < 0)
								m_iIndex = 0;
							
						}
						else if (iNum == 2)
						{
							m_pTerrain->Get_NaviCom()->Delete_Cell((m_iSelected_index - 2) / 3);
							pSelected->Set_Dead();
							m_vecCollision.erase(m_vecCollision.begin() + m_iSelected_index);

							--m_iSelected_index;
							pSelected = m_vecCollision[m_iSelected_index];
							pSelected->Set_Dead();
							m_vecCollision.erase(m_vecCollision.begin() + m_iSelected_index);

							--m_iSelected_index;
							pSelected = m_vecCollision[m_iSelected_index];
							pSelected->Set_Dead();
							m_vecCollision.erase(m_vecCollision.begin() + m_iSelected_index);
							m_iIndex -= 3;
							if (m_iIndex < 0)
								m_iIndex = 0;
						}
						
						// 전체 인덱스 넘버가 삭제한 애들 수만큼 앞으로 와야함
						for (auto& pCol : m_vecCollision)
						{
							_uint iIndex = pCol->Get_IndexNumber();
							if(iIndex >= m_iSelected_index)
								pCol->Set_IndexNumber(iIndex - 3);
						}
						if (m_vecCollision.size() > 2)
						{
							fPoints[0] = m_vecCollision.back()->Get_PickingPos().x;
							fPoints[1] = m_vecCollision.back()->Get_PickingPos().y;
							fPoints[2] = m_vecCollision.back()->Get_PickingPos().z;
							m_iSelected_index = -1;
						}
						else
							m_iSelected_index = 0;
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
	ImGui::Text("Position");
	ImGui::DragFloat3("Position", fPoints, 0.1f, 0.f, 3000.f);

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
		CollisionDesc.iIndexNumber = m_iIndex; // 전체 포인트의 숫자
		m_vecCollision.push_back(static_cast<CCollisionBox*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_NAVIGATION, TEXT("Layer_Collision"), TEXT("Prototype_GameObject_Collision_Box"), &CollisionDesc)));
		vPoints[0] = { fPointPos.x,fPointPos.y,fPointPos.z };
		fPoints[0] = fPointPos.x;
		fPoints[1] = fPointPos.y;
		fPoints[2] = fPointPos.z;
		m_iIndex++;
		m_bAfter_AddPoints = true;
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
		vPoints[1] = { fPointPos.x,fPointPos.y,fPointPos.z };
		fPoints[0] = fPointPos.x;
		fPoints[1] = fPointPos.y;
		fPoints[2] = fPointPos.z; 
		m_iIndex++;
		m_bAfter_AddPoints = true;
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
		vPoints[2] = { fPointPos.x,fPointPos.y,fPointPos.z };
		fPoints[0] = fPointPos.x;
		fPoints[1] = fPointPos.y;
		fPoints[2] = fPointPos.z;
		m_iIndex++;
		m_bAfter_AddPoints = true;
		break;
	}

	default:
		break;
	}


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
	CGameObject* pTerrain =	m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_NAVIGATION, pLayerTag, TEXT("Prototype_GameObject_Terrain"), &pDesc);
	m_pTerrain = static_cast<CTerrain*>(pTerrain);
	return S_OK;
}
HRESULT CNavigation_Leve::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC			Desc{};
	Desc.vEye = _float4(386.295f, 10.f, 450.425f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNearZ = 0.1f;
	Desc.fFar = 500.f;
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	Desc.fSpeedPerSec = 60.f;
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
