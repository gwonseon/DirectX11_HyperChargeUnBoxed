#include "stdafx.h"
#include "..\Public\Level_ImGui.h"


#include "Camera_Free.h"
#include "Monster.h"
#include "Level_Loading.h"
#include <Terrain.h>


CLevel_ImGui::CLevel_ImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_ImGui::Initialize()
{
	ShowCursor(true);
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))			return E_FAIL;	// 카메라 생성
	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))			return E_FAIL;	// 지형 생성
	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))			return E_FAIL;	// 몬스터
//	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))			return E_FAIL;	// 플레이어

	if (FAILED(Ready_Lights()))										return E_FAIL;	// 빛
	if (FAILED(m_pGameInstance->Close_Level(LEVEL_LOADING)))		return E_FAIL;	// 로딩 닫기

	// 터레인 피킹을 위해 터레인 컴포넌트 가져오기
	pVIBuffer_Terrain = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_IMGUI, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));

	// 저장 로드 버튼(이미지 버튼)
	Create_ImageButton();

    return S_OK;
}

void CLevel_ImGui::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);


	// ESC 누르면 창 나가짐
	// 애니메이션 없는 툴
	if (GetAsyncKeyState(VK_F1) & 0x0001)  // 애니 없는 모델
	{
		for (auto& pBuild : m_vecBuild)
		{
			pBuild->Set_PickingCheck(false);
			pBuild->Set_ImGuiMode(IMGUI_OBJECT_NONANIM);
		}
		for (auto& pEnviron : m_vecEnvironment)
		{
			pEnviron->Set_ImGuiMode(IMGUI_OBJECT_NONANIM);
		}
		
		m_eImGui_Type = IMGUI_OBJECT_NONANIM;
		m_iModelIndex = 0;
		m_fPickingPos = { 0.f,0.f,0.f };
	}
	// 애니메이션 모델 툴
	if (GetAsyncKeyState(VK_F2) & 0x0001)  // 애니 있는 모델
	{
		m_eImGui_Type = IMGUI_OBJECT_ANIM;
		m_iModelIndex = 0;
		m_fPickingPos = { 0.f,0.f,0.f };
	}
	// 건축툴
	if (GetAsyncKeyState(VK_F3) & 0x0001) // 건물툴
	{
		for (auto& pEnviron : m_vecEnvironment)
		{
			pEnviron->Set_PickingCheck(false);
			pEnviron->Set_ImGuiMode(IMGUI_BUILD);
		}
		for (auto& pBuild : m_vecBuild)
		{
			pBuild->Set_ImGuiMode(IMGUI_BUILD);
		}
		m_eImGui_Type = IMGUI_BUILD;
		m_iModelIndex = 0;
		m_fPickingPos = { 0.f,0.f,0.f };
	}
	// 코인
	if (GetAsyncKeyState(VK_F4) & 0x0001) // 코인
	{
		for (auto& pEnviron : m_vecEnvironment)
		{
			pEnviron->Set_PickingCheck(false);
			pEnviron->Set_ImGuiMode(IMGUI_ITEM);
		}
		for (auto& pBuild : m_vecBuild)
		{
			pBuild->Set_ImGuiMode(IMGUI_ITEM);
		}
		m_eImGui_Type = IMGUI_ITEM;
		m_iModelIndex = 0;
		m_fPickingPos = { 0.f,0.f,0.f };
	}
	// 모드 선택 ,   create   select 모드 
	if (m_pGameInstance->Get_DIKeyState_Down(DIK_TAB))
	{
		if (m_iModeSelect == IMGUI_SELECT)
		{
			_float3 fPos{}, fScale{}, fCollisionPos{}, fCollisionScale{};
			_vector vPos{}, vCollisionPos{};
			
			switch (m_eImGui_Type)
			{
			case Client::CLevel_ImGui::IMGUI_OBJECT_NONANIM:
				
				if(m_iEnvironment_Count > 0)
				{
					for (auto& pEnviron : m_vecEnvironment)
					{
						pEnviron->Set_PickingCheck(false);
					}
					vPos = m_vecEnvironment.back()->Get_Pos();
					fScale = m_vecEnvironment.back()->Get_Scale();
					m_vecEnvironment.back()->Set_PickingCheck(true);
					vCollisionPos = m_vecEnvironment.back()->Get_CollisionBoxPos();
					fCollisionScale = m_vecEnvironment.back()->Get_CollisionBoxScale();
					XMStoreFloat3(&fPos, vPos);
					XMStoreFloat3(&fCollisionPos, vCollisionPos);
				}
				break;
			case Client::CLevel_ImGui::IMGUI_OBJECT_ANIM:
				break;
			case Client::CLevel_ImGui::IMGUI_BUILD:
				if(m_iBuild_Count > 0)
				{
					for (auto& pBuild : m_vecBuild)
					{
						pBuild->Set_PickingCheck(false);
					}
					vPos = m_vecBuild.back()->Get_Pos();
					fScale = m_vecBuild.back()->Get_Scale();
					m_vecBuild.back()->Set_PickingCheck(true);
					vCollisionPos = m_vecBuild.back()->Get_CollisionBoxPos();
					fCollisionScale = m_vecBuild.back()->Get_CollisionBoxScale();
					XMStoreFloat3(&fPos, vPos);
					XMStoreFloat3(&fCollisionPos, vCollisionPos);
				}
				break;
			case Client::CLevel_ImGui::IMGUI_ITEM:
				vPos = m_vecCoin.back()->Get_Pos();
				fScale = m_vecCoin.back()->Get_Scale();

				break;
			case Client::CLevel_ImGui::IMGUI_END:
				break;
			default:
				break;
			}
			Position[0] = fPos.x;				Position[1] = fPos.y;				Position[2] = fPos.z;
			Scale[0] = fScale.x;				Scale[1] = fScale.y;				Scale[2] = fScale.z;
			if(m_eImGui_Type != IMGUI_ITEM)
			{
				// Coin일 때 충돌박스 안씀
				CollisionBox_Pos[0] = fCollisionPos.x;					CollisionBox_Pos[1] = fCollisionPos.y;					CollisionBox_Pos[2] = fCollisionPos.z;
				CollisionBox_Scale[0] = fCollisionScale.x;				CollisionBox_Scale[1] = fCollisionScale.y;				CollisionBox_Scale[2] = fCollisionScale.z;
			}
			m_iModeSelect = IMGUI_CREATE;
		}
		else if (m_iModeSelect == IMGUI_CREATE)
		{
			m_iModeSelect = IMGUI_SELECT;
		}
	}  
	// 선택 혹은 생성 가능하게 해주는 bool 값 변경
	if ((m_pGameInstance->Get_DIKeyState_Down(DIK_RETURN) && (bAble_Select == false)))
	{
		bAble_Select = true;
	}
	
	
	// 툴에 따른 업데이트 
	switch (m_eImGui_Type)
	{
	case Client::CLevel_ImGui::IMGUI_OBJECT_NONANIM:
		Object_NonAnim_Update(fTimeDelta);
		break;
	case Client::CLevel_ImGui::IMGUI_OBJECT_ANIM:
		break;
	case Client::CLevel_ImGui::IMGUI_BUILD:
		Build_Update(fTimeDelta);
	case Client::CLevel_ImGui::IMGUI_ITEM:
		Item_Update(fTimeDelta);
		break;
	case Client::CLevel_ImGui::IMGUI_END:
		break;
	default:
		break;
	}

	if (m_pGameInstance->Get_DIKeyState_Down(DIK_ESCAPE))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_IMGUI, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOGO))))
			return;
	}
}

HRESULT CLevel_ImGui::Render()
{
	__super::Render();
	if(m_bWindowsMove == false)
	{
		ImGui::SetWindowPos("ParentWindow", ImVec2(0, 200)); // 초기 위치 설정
	}
	ImGui::SetNextWindowSize(ImVec2(400, 600)); // 가로 400, 세로 600 크기로 설정
	ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(800, 600)); // 최소 크기 200x200, 최대 크기 800x600
	ImGui::Begin("ParentWindow", nullptr, ImGuiWindowFlags_None); // 창 이동 가능
	// ImGui 코드 작성칸
	switch (m_eImGui_Type)
	{
	case Client::CLevel_ImGui::IMGUI_OBJECT_NONANIM:
		Object_NonAnim();
		break;
	case Client::CLevel_ImGui::IMGUI_OBJECT_ANIM:
		Object_Anim();
		break;
	case Client::CLevel_ImGui::IMGUI_BUILD:
		Object_Build();
		break;
	case Client::CLevel_ImGui::IMGUI_ITEM:
		Object_Item();
		break;
	case Client::CLevel_ImGui::IMGUI_END:
		break;
	default:
		break;
	}
	ImGui::End();
    
	if (m_bWindowsMove == false)
	{
		m_bWindowsMove = true;
		ImGui::SetWindowPos("Save_Load", ImVec2(0, 0));
	}
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
		switch (m_eImGui_Type)
		{
		case Client::CLevel_ImGui::IMGUI_OBJECT_NONANIM:
			Environment_Load();
			break;
		case Client::CLevel_ImGui::IMGUI_OBJECT_ANIM:
			break;
		case Client::CLevel_ImGui::IMGUI_BUILD:
			Build_Load();
			break;
		case Client::CLevel_ImGui::IMGUI_ITEM:
			Item_Load();
			break;
		case Client::CLevel_ImGui::IMGUI_END:
			break;
		default:
			break;
		}
	}
	ImGui::End();

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("ImGui레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_ImGui::Ready_Layer_Terrain(const _tchar* pLayerTag)
{
	CTerrain::TERRAIN_DESC pDesc{};
	pDesc.eID= LEVEL_IMGUI;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_IMGUI, pLayerTag, TEXT("Prototype_GameObject_Terrain_ImGui"), &pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_ImGui::Picking_Create()
{
	if ((m_pGameInstance->Get_DIMouseState_Down(DIM_LB)) && (bAble_Select == true) && m_iModeSelect == IMGUI_CREATE)
	{
		_float3 fMousePos = m_pGameInstance->Get_MousePos_NDC(g_hWnd, g_iWinSizeX, g_iWinSizeY);
		XMMATRIX invProj = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_PROJ);
		XMMATRIX invView = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW);
		XMVECTOR RayPos, RayDir;

		m_pGameInstance->Get_MouseRayDirection(fMousePos, invProj, invView, &RayPos, &RayDir);

		RayDir = XMVector3Normalize(RayDir);
		
		const _float3* VtxPos = pVIBuffer_Terrain->Get_VtxPos();  // _float3 배열의 시작 주소 반환
		_uint VtxCountX = pVIBuffer_Terrain->Get_VtxCountX();
		_uint VtxCountZ = pVIBuffer_Terrain->Get_VtxCountZ();

	
		m_fPickingPos = m_pGameInstance->Picking_Terrain(RayPos, RayDir, VtxPos, VtxCountX, VtxCountZ);
			
			switch (m_eImGui_Type)
			{
			case Client::CLevel_ImGui::IMGUI_OBJECT_NONANIM:
				Environment_Add();
				break;
			case Client::CLevel_ImGui::IMGUI_OBJECT_ANIM:
				break;
			case Client::CLevel_ImGui::IMGUI_BUILD:
				Build_Add();
				break;
			case Client::CLevel_ImGui::IMGUI_ITEM:
				Item_Add();
				break;
			case Client::CLevel_ImGui::IMGUI_END:
				break;
			default:
				break;
			}
	

	
	}
	return S_OK;
}



void CLevel_ImGui::Object_NonAnim_Update(_float fTimeDelta)
{	
	
	if (m_iEnvironment_Count > 0)
		Environment_DataChange(fTimeDelta);
	Picking_Create();  // Create 모드
	Environment_Select();  // Select 모드

	if (Save == true)
	{
		Environment_Save();
		Save = false;
	}
}

void CLevel_ImGui::Object_Anim_Update(_float fTimeDelta)
{
}

void CLevel_ImGui::Build_Update(_float fTimeDelta)
{

	if (m_iBuild_Count > 0)
		Build_DataChange(fTimeDelta);
	Picking_Create();
	Build_Select();

	if (Save == true)
	{
		Build_Save();
		Save = false;
	}
}

void CLevel_ImGui::Item_Update(_float fTimeDelta)
{
	if (m_iCoin_Count > 0)
		Item_DataChange(fTimeDelta);
	Picking_Create();
	Item_Select();

	if (Save == true)
	{
		Item_Save();
		Save = false;
	}
}


void CLevel_ImGui::Object_NonAnim()
{
	const char* pText = "NonAnim Object Tool";
	ImGui::Text(pText);
	ImGui::Text(" ");

	if (m_iModeSelect == IMGUI_CREATE)
	{
		const char* pModeText = "Create Mode";
		ImGui::Text(pModeText);
	}
	if (m_iModeSelect == IMGUI_SELECT)
	{
		const char* pModeText = "Select Mode";
		ImGui::Text(pModeText);
	}

	// 위치 크기 방향 수정창
	ImGui::Text("Object Data");
	ImGui::DragFloat3("Position", Position, 0.1f, -200.f, 3000.f);
	ImGui::DragFloat3("Scale", Scale, 0.1f, 0.f, 10000.f);
	ImGui::Text(" ");
	ImGui::Text("CollisionBox");
	ImGui::DragFloat3("Box_Position", CollisionBox_Pos, 0.1f, -90.f, 10000.f);
	ImGui::DragFloat3("Box_Scale", CollisionBox_Scale, 0.1f, 0.f, 10000.f);

	// 모델 선택창
	ImGui::Text(" ");
	ImGui::Text(" ");
	ImGui::Text(" ");

	ImGui::Text("Environment List ");
	ImGui::BeginChild("Scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_None);
	ImGui::InputInt("ModelIndex", &m_iModelIndex, 0);
	ButtonImage_List(); // ImGui 선택 리스트 ( Environment 리스트 )
	ImGui::EndChild();
}

void CLevel_ImGui::Object_Anim()
{
	const char* pText = "Anim Object Tool";
	ImGui::Text(pText);
	ImGui::Text(" ");
	if (m_iModeSelect == IMGUI_CREATE)
	{
		const char* pModeText = "Create Mode";
		ImGui::Text(pModeText);
	}
	if (m_iModeSelect == IMGUI_SELECT)
	{
		const char* pModeText = "Select Mode";
		ImGui::Text(pModeText);
	}
	// 위치 크기 방향 수정창
	ImGui::Text("Object Data");
	ImGui::DragFloat3("Position", Position, 0.1f, -200.f, 3000.f);
	ImGui::DragFloat3("Scale", Scale, 0.1f, 0.f, 10000.f);
	ImGui::Text(" ");
	ImGui::Text("CollisionBox");
	ImGui::DragFloat3("Box_Position", CollisionBox_Pos, 0.1f, -90.f, 10000.f);
	ImGui::DragFloat3("Box_Scale", CollisionBox_Scale, 0.1f, 0.f, 10000.f);
	// 모델 선택창
	ImGui::Text(" ");
	ImGui::Text(" ");
	ImGui::Text(" ");

	ImGui::Text("MapObject List ");
	ImGui::BeginChild("Scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_None);
	ImGui::InputInt("ModelIndex", &m_iModelIndex, 0);

	ImGui::EndChild();
}

void CLevel_ImGui::Object_Build()
{
	const char* pText = "Build Tool";
	ImGui::Text(pText);
	ImGui::Text(" ");
	if (m_iModeSelect == IMGUI_CREATE)
	{
		const char* pModeText = "Create Mode";
		ImGui::Text(pModeText);
	}
	if (m_iModeSelect == IMGUI_SELECT)
	{
		const char* pModeText = "Select Mode";
		ImGui::Text(pModeText);
	}
	// 위치 크기 방향 수정창
	ImGui::Text("Build Data");
	ImGui::DragFloat3("Position", Position, 0.1f, -200.f, 3000.f);
	ImGui::DragFloat3("Scale", Scale, 0.1f, 0.f, 10000.f);
	ImGui::Text(" ");
	ImGui::Text("CollisionBox");
	ImGui::DragFloat3("Box_Position", CollisionBox_Pos, 0.1f, -90.f, 10000.f);
	ImGui::DragFloat3("Box_Scale", CollisionBox_Scale, 0.1f, 0.f, 10000.f);

	// 모델 선택창
	ImGui::Text(" ");
	ImGui::Text(" ");
	ImGui::Text(" ");

	ImGui::Text("Build List ");
	ImGui::BeginChild("Scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_None);
	ImGui::InputInt("ModelIndex", &m_iModelIndex, 0);
	ButtonImage_List(); // ImGui 선택 리스트 ( Environment 리스트 )
	ImGui::EndChild();
}

void CLevel_ImGui::Object_Item()
{
	const char* pText = "Coin Tool";
	ImGui::Text(pText);
	ImGui::Text(" ");
	if (m_iModeSelect == IMGUI_CREATE)
	{
		const char* pModeText = "Create Mode";
		ImGui::Text(pModeText);
	}
	if (m_iModeSelect == IMGUI_SELECT)
	{
		const char* pModeText = "Select Mode";
		ImGui::Text(pModeText);
	}
	// 위치 크기 방향 수정창
	ImGui::Text("Build Data");
	ImGui::DragFloat3("Position", Position, 0.1f, -200.f, 3000.f);
	ImGui::DragFloat3("Scale", Scale, 0.1f, 0.f, 10000.f);

	// 모델 선택창
	ImGui::Text(" ");
	ImGui::Text(" ");
	ImGui::Text(" ");

	ImGui::Text("Build List ");
	ImGui::BeginChild("Scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_None);
	ImGui::InputInt("ModelIndex", &m_iModelIndex, 0);
	ButtonImage_List(); // ImGui 선택 리스트 ( Environment 리스트 )
	ImGui::EndChild();

}


void CLevel_ImGui::ButtonImage_List()
{
	ImGui::BeginChild("Choose Environment");
	int iButton = 0;	

	switch (m_eImGui_Type)
	{
	case Client::CLevel_ImGui::IMGUI_OBJECT_NONANIM:
	{
		auto& SRVs = m_pEnviron->Get_SRV();
		for (auto iter = SRVs.begin(); iter != SRVs.end(); ++iter)
		{
			if (iButton % 4 != 0)
				ImGui::SameLine();
			string tag = "Environment" + to_string(iButton);
			if (ImGui::ImageButton(tag.c_str(), *iter, ImVec2(50, 50), ImVec2(0, 0)))
			{
				m_iModelIndex = iButton;
			}
			iButton++;
		}
		ImGui::EndChild();
	}
		break;
	case Client::CLevel_ImGui::IMGUI_OBJECT_ANIM:
		break;
	case Client::CLevel_ImGui::IMGUI_BUILD:
	{
		auto& SRVs = m_pBuild->Get_SRV();
		for (auto iter = SRVs.begin(); iter != SRVs.end(); ++iter)
		{
			if (iButton % 4 != 0)
				ImGui::SameLine();
			string tag = "Build" + to_string(iButton);
			if (ImGui::ImageButton(tag.c_str(), *iter, ImVec2(50, 50), ImVec2(0, 0)))
			{
				m_iModelIndex = iButton;
			}
			iButton++;
		}
		ImGui::EndChild();
	}
		break;
	case Client::CLevel_ImGui::IMGUI_ITEM:
	{
		auto& SRVs = m_pBuild->Get_SRV();
		for (auto iter = SRVs.begin(); iter != SRVs.end(); ++iter)
		{
			if (iButton % 4 != 0)
				ImGui::SameLine();
			string tag = "Build" + to_string(iButton);
			if (ImGui::ImageButton(tag.c_str(), *iter, ImVec2(50, 50), ImVec2(0, 0)))
			{
				m_iModelIndex = iButton;
			}
			iButton++;
		}
		ImGui::EndChild();
	}
	break;
	case Client::CLevel_ImGui::IMGUI_END:
		break;
	default:
		break;
	}


}

HRESULT CLevel_ImGui::Environment_Add()
{
	if (m_fPickingPos.x == 0 && m_fPickingPos.y == 0 && m_fPickingPos.z == 0)
		return S_OK;

	CEnvironment::ENVIRONMENT_DESC			Desc{};
	Desc.eID = LEVEL_IMGUI;
	Desc.fPosition = m_fPickingPos;
	Desc.iModelComponentIndex = m_iModelIndex;
	Desc.fScale = { 1.f,1.f ,1.f };
	Desc.iImGuiMode = IMGUI_OBJECT_NONANIM;
	Position[0] = m_fPickingPos.x;	Position[1] = m_fPickingPos.y;	Position[2] = m_fPickingPos.z;
	Scale[0] = Desc.fScale.x;		Scale[1] = Desc.fScale.y;		Scale[2] = Desc.fScale.z;
	


	CollisionBox_Pos[0] = m_fPickingPos.x;		CollisionBox_Pos[1] = m_fPickingPos.y;		CollisionBox_Pos[2] = m_fPickingPos.z;
	CollisionBox_Scale[0] = Desc.fScale.x;		CollisionBox_Scale[1] = Desc.fScale.y;		CollisionBox_Scale[2] = Desc.fScale.z;

	pGameObj = (m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_IMGUI, TEXT("Layer_Environment"),
		TEXT("Prototype_GameObject_Environment_ImGui"), &Desc));
	if (pGameObj != nullptr)
	{
		for (auto& pEnviron : m_vecEnvironment)
		{
			pEnviron->Set_PickingCheck(false);
		}
		m_vecEnvironment.push_back(dynamic_cast<CEnvironment*>(pGameObj));
		m_vecEnvironment.back()->Set_CollisionBox(CollisionBox_Scale[0], CollisionBox_Scale[1], CollisionBox_Scale[2], CollisionBox_Pos[0], CollisionBox_Pos[1], CollisionBox_Pos[2]);
		m_vecEnvironment.back()->Set_PickingCheck(true);
		m_iEnvironment_Count++;
		bAble_Select = false;

	}

	return S_OK;
}
HRESULT CLevel_ImGui::Environment_DataChange(_float fTimeDelta)
{
	if(m_iModeSelect == IMGUI_CREATE)   // Create 모드 일 때 가장 최근 설치 항목에 대한 수정 가능 기능
	{
		// 가장 최근 설치한 Environment 삭제하기
		if ((m_pGameInstance->Get_DIMouseState_Down(DIM_RB)) && (GetAsyncKeyState(VK_CONTROL) & 0x8000) && m_iEnvironment_Count > 0)
		{
			m_vecEnvironment.back()->Set_DeadEnviron();
			m_vecEnvironment.erase(m_vecEnvironment.end() - 1);
			--m_iEnvironment_Count;
			cout << "남은 Environment 개수 : " << m_iEnvironment_Count << endl;

			if (m_iEnvironment_Count > 0)
			{
				_float3 fPos, fScale;
				_vector vPos = m_vecEnvironment.back()->Get_Pos();
				fScale = m_vecEnvironment.back()->Get_Scale();
				m_vecEnvironment.back()->Set_PickingCheck(true);
				_float3 fCollisionBoxScale = m_vecEnvironment.back()->Get_CollisionBoxScale();
				_vector vecCollisionBoxPos = m_vecEnvironment.back()->Get_CollisionBoxPos();
				_float3 fEnvironPos{}, fCollisionBoxPos{};

				XMStoreFloat3(&fCollisionBoxPos, vecCollisionBoxPos);

				XMStoreFloat3(&fPos, vPos);
				Position[0] = fPos.x;				Position[1] = fPos.y;				Position[2] = fPos.z;
				Scale[0] = fScale.x;				Scale[1] = fScale.y;				Scale[2] = fScale.z;

				CollisionBox_Pos[0] = fCollisionBoxPos.x;				CollisionBox_Pos[1] = fCollisionBoxPos.y;				CollisionBox_Pos[2] = fCollisionBoxPos.z;
				CollisionBox_Scale[0] = fCollisionBoxScale.x;				CollisionBox_Scale[1] = fCollisionBoxScale.y;				CollisionBox_Scale[2] = fCollisionBoxScale.z;


			}
			else
			{
				Position[0] = 0.f;				Position[1] = 0.f;				Position[2] = 0.f;
				Scale[0] = 0.f;					Scale[1] = 0.f;					Scale[2] = 0.f;
				CollisionBox_Pos[0] = 0.f;				CollisionBox_Pos[1] = 0.f;				CollisionBox_Pos[2] = 0.f;
				CollisionBox_Scale[0] = 0.f;					CollisionBox_Scale[1] = 0.f;					CollisionBox_Scale[2] = 0.f;
			}
			return S_OK;
		}


		m_vecEnvironment.back()->MovePos(fTimeDelta, Position[0], Position[1], Position[2]);
		m_vecEnvironment.back()->Set_Scale(fTimeDelta, Scale[0], Scale[1], Scale[2]);
		m_vecEnvironment.back()->Set_CollisionBox(CollisionBox_Scale[0], CollisionBox_Scale[1], CollisionBox_Scale[2], CollisionBox_Pos[0], CollisionBox_Pos[1], CollisionBox_Pos[2]);

	
		XMVECTOR vTemp = {0.f, 0.f ,0.f ,1.f }; // 회전축
		_bool bGetKey = false;
		if ((GetAsyncKeyState(VK_LEFT) & 0x8000) && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
		{
			bGetKey = true;
			vTemp = { 0.f, 0.f, -1.f, 1.f };
		}
		else if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
		{
			bGetKey = true;
			vTemp = { 0.f, 0.f, 1.f, 1.f };
		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			bGetKey = true;
			vTemp = { 0.f, -1.f, 0.f, 1.f };
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			bGetKey = true;
			vTemp = { 0.f, 1.f, 0.f, 1.f };
		}
		else if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			bGetKey = true;
	
			vTemp = { 1.f, 0.f, 0.f, 1.f };
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			bGetKey = true;
			vTemp = { -1.f, 0.f, 0.f, 1.f };
		}
		else
		{
			bGetKey = false;
		}

		if(bGetKey == true)
			m_vecEnvironment.back()->Set_Turn(fTimeDelta, vTemp);
	
	}
	else if (m_iModeSelect == IMGUI_SELECT)
	{
		_uint iEnvironmentIndex = 0;
		for (auto& pEnviron : m_vecEnvironment)
		{

			if (true == pEnviron->Get_PickingCheck())
			{

				if ((m_pGameInstance->Get_DIMouseState_Down(DIM_RB)) && (GetAsyncKeyState(VK_CONTROL) & 0x8000) && m_iEnvironment_Count > 0)
				{
					pEnviron->Set_DeadEnviron();
					m_vecEnvironment.erase(m_vecEnvironment.begin() + iEnvironmentIndex);
					--m_iEnvironment_Count;
					cout << "남은 Environment 개수 : " << m_iEnvironment_Count << endl;

					break;
				}

				pEnviron->MovePos(fTimeDelta, Position[0], Position[1], Position[2]);
				pEnviron->Set_Scale(fTimeDelta, Scale[0], Scale[1], Scale[2]);
				pEnviron->Set_CollisionBox(CollisionBox_Scale[0], CollisionBox_Scale[1], CollisionBox_Scale[2], CollisionBox_Pos[0], CollisionBox_Pos[1], CollisionBox_Pos[2]);

				XMVECTOR vTemp = { 0.f, 0.f ,0.f ,1.f }; // 회전축
				_bool bGetKey = false;
				if ((GetAsyncKeyState(VK_LEFT) & 0x8000) && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
				{
					bGetKey = true;
					vTemp = { 0.f, 0.f, -1.f, 1.f };
				}
				else if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
				{
					bGetKey = true;
					vTemp = { 0.f, 0.f, 1.f, 1.f };
				}
				else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
				{
					bGetKey = true;
					vTemp = { 0.f, -1.f, 0.f, 1.f };
				}
				else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
				{
					bGetKey = true;
					vTemp = { 0.f, 1.f, 0.f, 1.f };
				}
				else if (GetAsyncKeyState(VK_UP) & 0x8000)
				{
					bGetKey = true;

					vTemp = { 1.f, 0.f, 0.f, 1.f };
				}
				else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
				{
					bGetKey = true;
					vTemp = { -1.f, 0.f, 0.f, 1.f };
				}

				else
					bGetKey = false;

				if (bGetKey == true)
					pEnviron->Set_Turn(fTimeDelta, vTemp);

				break;
			}
			iEnvironmentIndex++;
		}

	}
	return S_OK;
}
void CLevel_ImGui::Environment_Save()
{
	HANDLE hFile = CreateFile(L"../Bin/Data/Environment.dat", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Save Environment File Creation Failed", L"Error", MB_OK);
		return;
	}

	DWORD dwByte = 0;
	_float3 fPos;
	for (auto& environment : m_vecEnvironment)
	{
		if (environment)
		{
			
			
			LEVELID iLevel = environment->Get_Level();
			_int  iModelIndex = environment->Get_ModelIndex();
			_uint iImGuiMode = environment->Get_ImGuiMode();
			_vector vPos = environment->Get_Pos();
			XMStoreFloat3(&fPos, vPos);
			_float3 fScale = environment->Get_Scale();
			_float3 fCollisionScale = environment->Get_CollisionBoxScale();
			_vector vecCollisionPos = environment->Get_CollisionBoxPos();
			_vector	vRight{};
			_vector	vUp{};
			_vector	vLook{};

			environment->Get_Rotation(vRight, vUp, vLook);

			WriteFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr);
			WriteFile(hFile, &iModelIndex, sizeof(_int), &dwByte, nullptr);
			WriteFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
			WriteFile(hFile, &fScale, sizeof(_float3), &dwByte, nullptr);
			WriteFile(hFile, &fCollisionScale, sizeof(_float3), &dwByte, nullptr);
			WriteFile(hFile, &iImGuiMode, sizeof(_uint), &dwByte, nullptr);

			WriteFile(hFile, &vecCollisionPos, sizeof(_vector), &dwByte, nullptr);
			WriteFile(hFile, &vRight, sizeof(_vector), &dwByte, nullptr);
			WriteFile(hFile, &vUp, sizeof(_vector), &dwByte, nullptr);
			WriteFile(hFile, &vLook, sizeof(_vector), &dwByte, nullptr);

			m_vecModelIndex.push_back(iModelIndex);
		}
	}

	CloseHandle(hFile);
	MessageBox(NULL, L"Environment Saved Successfully", L"Success", MB_OK);

	sort(m_vecModelIndex.begin(), m_vecModelIndex.end());
	vector<_int>::iterator iter = unique(m_vecModelIndex.begin(), m_vecModelIndex.end());
	m_vecModelIndex.erase(iter, m_vecModelIndex.end());

	HANDLE hIndexFile = CreateFile(L"../Bin/Data/GamePlayLevel_Env_Index.dat", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hIndexFile)
	{
		MessageBox(NULL, L"Save GamePlayLevel_Env_Index File Creation Failed", L"Error", MB_OK);
		return;
	}

	dwByte = 0;

	for (auto& pIndex : m_vecModelIndex)
	{
		WriteFile(hIndexFile, &pIndex, sizeof(_int), &dwByte, nullptr);
	}

	CloseHandle(hIndexFile);
	MessageBox(NULL, L"GamePlayLevel_Env_Index Saved Successfully", L"Success", MB_OK);



}
void CLevel_ImGui::Environment_Load()
{
	for (auto& pEnviron : m_vecEnvironment)
	{
		pEnviron->Set_DeadEnviron();
	}
	m_vecEnvironment.clear();
	m_iEnvironment_Count = 0;
	HANDLE hFile = CreateFile(L"../Bin/Data/Environment.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Environment File Failed", L"Error", MB_OK);
		return;
	}
	DWORD dwByte = 0;
	LEVELID iLevel;
	_int  iModelIndex;
	_float3 fPos{}, fCollisionScale{}, fScale{}, fCollisionPos{};
	_vector	vRight{}, vUp{}, vLook{}, vecCollisionPos{};
	_uint iImGuiMode{};
	while (ReadFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr) && dwByte > 0)
	{
		
		ReadFile(hFile, &iModelIndex, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &fScale, sizeof(_float3), &dwByte, nullptr);

		ReadFile(hFile, &fCollisionScale, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &iImGuiMode, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &vecCollisionPos, sizeof(_vector), &dwByte, nullptr);

		ReadFile(hFile, &vRight, sizeof(_vector), &dwByte, nullptr);
		ReadFile(hFile, &vUp, sizeof(_vector), &dwByte, nullptr);
		ReadFile(hFile, &vLook, sizeof(_vector), &dwByte, nullptr);


		CEnvironment::ENVIRONMENT_DESC			Desc{};
		Desc.eID = iLevel;
		Desc.fPosition = fPos;
		Desc.iModelComponentIndex = iModelIndex;
		Desc.fScale = fScale;
		//cout << fScale.x << "     " << fScale.y << "            " << fScale.z << endl;
		pGameObj = (m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_IMGUI, TEXT("Layer_Environment"),
			TEXT("Prototype_GameObject_Environment_ImGui"), &Desc));
		if (pGameObj != nullptr)
		{

			Position[0] = fPos.x;			Position[1] = fPos.y;			Position[2] = fPos.z;
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Scale(0.f, fScale.x, fScale.y, fScale.z);
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Rotaion(vRight, vUp, vLook);
	
			Scale[0] = fScale.x;			Scale[1] = fScale.y;			Scale[2] = fScale.z;

			XMStoreFloat3(&fCollisionPos, vecCollisionPos);
			
			Desc.CollisionBoxScale.x = fCollisionScale.x;		Desc.CollisionBoxScale.y = fCollisionScale.y;		Desc.CollisionBoxScale.z = fCollisionScale.z;
			Desc.CollisionBoxPos.x = fCollisionPos.x;			Desc.CollisionBoxPos.y = fCollisionPos.y;			Desc.CollisionBoxPos.z = fCollisionPos.z;
			/*
						CollisionBox_Pos[0] = fPos.x;		CollisionBox_Pos[1] = fPos.y;		CollisionBox_Pos[2] = fPos.z;
						CollisionBox_Scale[0] = fCollisionBox_Scale.x;		CollisionBox_Scale[1] = fCollisionBox_Scale.y;		CollisionBox_Scale[2] = fCollisionBox_Scale.z;
			*/
			dynamic_cast<CEnvironment*>(pGameObj)->Set_CollisionBox(fCollisionScale.x, fCollisionScale.y, fCollisionScale.z, fCollisionPos.x, fCollisionPos.y, fCollisionPos.z);

		


			if(m_vecEnvironment.size() >0 )
				m_vecEnvironment.back()->Set_PickingCheck(false);
			m_vecEnvironment.push_back(dynamic_cast<CEnvironment*>(pGameObj));
			m_vecEnvironment.back()->Set_PickingCheck(true);
			m_iEnvironment_Count++;
		}
	}

	CloseHandle(hFile);
	MessageBox(NULL, L"Environment Loaded Successfully", L"Success", MB_OK);
}

HRESULT CLevel_ImGui::Environment_Select()
{
	if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && m_iModeSelect == IMGUI_SELECT && bAble_Select == true)
	{
		_float3 fMousePos = m_pGameInstance->Get_MousePos_NDC(g_hWnd, g_iWinSizeX, g_iWinSizeY);
		XMMATRIX invProj = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_PROJ);
		XMMATRIX invView = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW);
		XMVECTOR RayPos, RayDir;
		m_pGameInstance->Get_MouseRayDirection(fMousePos, invProj, invView, &RayPos, &RayDir);
		RayDir = XMVector3Normalize(RayDir);

		for (auto& pEnvironment : m_vecEnvironment)
		{
			XMFLOAT3 fBoxPos{}, fMinPoint{}, fMaxPoint{};
			_vector vBoxPos = pEnvironment->Get_CollisionBoxPos();
			XMStoreFloat3(&fBoxPos, vBoxPos);
			 XMFLOAT3 fBoxSize = pEnvironment->Get_CollisionBoxScale();
		
			m_pGameInstance->CreateBoundingBox(fBoxPos, fBoxSize, fMinPoint, fMaxPoint);
			_bool bPickCheck = false;
			float distance;
			if (m_pGameInstance->Picking_Box(RayPos, RayDir, fMinPoint, fMaxPoint, distance, pEnvironment->Get_BoundingBox()))
			{
				bPickCheck = true;
				pEnvironment->Set_PickingCheck(bPickCheck);
				_vector vecEnvironPos = pEnvironment->Get_Pos();
				_float3 fEnvironScale = pEnvironment->Get_Scale();

				_float3 fEnvironPos{};
				XMStoreFloat3(&fEnvironPos, vecEnvironPos);


				Position[0] = fEnvironPos.x;		Position[1] = fEnvironPos.y;		Position[2] = fEnvironPos.z;
				Scale[0] = fEnvironScale.x;			Scale[1] = fEnvironScale.y;			Scale[2] = fEnvironScale.z;

			
				CollisionBox_Pos[0] = fBoxPos.x;			CollisionBox_Pos[1] = fBoxPos.y;			CollisionBox_Pos[2] = fBoxPos.z;
				CollisionBox_Scale[0] = fBoxSize.x;		CollisionBox_Scale[1] = fBoxSize.y;		CollisionBox_Scale[2] = fBoxSize.z;

				bAble_Select = false;
				
			}
			pEnvironment->Set_PickingCheck(bPickCheck);

		}
	}
	
	return S_OK;
}

HRESULT CLevel_ImGui::Build_Add()
{
	if (m_fPickingPos.x == 0 && m_fPickingPos.y == 0 && m_fPickingPos.z == 0)
		return S_OK;

	CEnvironment::ENVIRONMENT_DESC			Desc{};
	Desc.eID = LEVEL_IMGUI;
	Desc.fPosition = m_fPickingPos;
	Desc.iModelComponentIndex = m_iModelIndex + ENVIRONMENT_EA;
	Desc.fScale = { 1.f,1.f ,1.f };
	Desc.iImGuiMode = IMGUI_BUILD;
	Position[0] = m_fPickingPos.x;	Position[1] = m_fPickingPos.y;	Position[2] = m_fPickingPos.z;
	Scale[0] = Desc.fScale.x;		Scale[1] = Desc.fScale.y;		Scale[2] = Desc.fScale.z;

	CollisionBox_Pos[0] = m_fPickingPos.x;		CollisionBox_Pos[1] = m_fPickingPos.y;		CollisionBox_Pos[2] = m_fPickingPos.z;
	CollisionBox_Scale[0] = Desc.fScale.x;		CollisionBox_Scale[1] = Desc.fScale.y;		CollisionBox_Scale[2] = Desc.fScale.z;

	pGameObj = (m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_IMGUI, TEXT("Layer_Environment"),
		TEXT("Prototype_GameObject_Environment_ImGui"), &Desc));
	if (pGameObj != nullptr)
	{
		for (auto& pBuild : m_vecBuild)
		{
			pBuild->Set_PickingCheck(false);
		}
		m_vecBuild.push_back(dynamic_cast<CEnvironment*>(pGameObj));
		m_vecBuild.back()->Set_PickingCheck(true);
		m_vecBuild.back()->Set_CollisionBox(CollisionBox_Scale[0], CollisionBox_Scale[1], CollisionBox_Scale[2], CollisionBox_Pos[0], CollisionBox_Pos[1], CollisionBox_Pos[2]);

		m_iBuild_Count++;
		bAble_Select = false;
	}

	return S_OK;
}

HRESULT CLevel_ImGui::Build_DataChange(_float fTimeDelta)
{

	if (m_iModeSelect == IMGUI_CREATE)   // Create 모드 일 때 가장 최근 설치 항목에 대한 수정 가능 기능
	{
		// 가장 최근 설치한 Build 삭제하기
		if ((m_pGameInstance->Get_DIMouseState_Down(DIM_RB) ) && (GetAsyncKeyState(VK_CONTROL) & 0x8000) && m_iBuild_Count > 0)
		{

			m_vecBuild.back()->Set_DeadEnviron();
			m_vecBuild.erase(m_vecBuild.end() - 1);
			--m_iBuild_Count;
			cout << "남은 Build 개수 : " << m_iBuild_Count << endl;

			bAble_Select = true;
			if (m_iBuild_Count > 0)
			{
				_float3 fPos, fScale;
				_vector vPos = m_vecBuild.back()->Get_Pos();
				fScale = m_vecBuild.back()->Get_Scale();
				m_vecBuild.back()->Set_PickingCheck(true);
				_float3 fCollisionBoxScale = m_vecBuild.back()->Get_CollisionBoxScale();
				_vector vecCollisionBoxPos = m_vecBuild.back()->Get_CollisionBoxPos();
				_float3 fEnvironPos{}, fCollisionBoxPos{};

				XMStoreFloat3(&fCollisionBoxPos, vecCollisionBoxPos);

				XMStoreFloat3(&fPos, vPos);
				Position[0] = fPos.x;				Position[1] = fPos.y;				Position[2] = fPos.z;
				Scale[0] = fScale.x;				Scale[1] = fScale.y;				Scale[2] = fScale.z;

				CollisionBox_Pos[0] = fCollisionBoxPos.x;				CollisionBox_Pos[1] = fCollisionBoxPos.y;				CollisionBox_Pos[2] = fCollisionBoxPos.z;
				CollisionBox_Scale[0] = fCollisionBoxScale.x;				CollisionBox_Scale[1] = fCollisionBoxScale.y;				CollisionBox_Scale[2] = fCollisionBoxScale.z;

			}
			else
			{
				Position[0] = 0.f;				Position[1] = 0.f;				Position[2] = 0.f;
				Scale[0] = 0.f;					Scale[1] = 0.f;					Scale[2] = 0.f;

				CollisionBox_Pos[0] = 0.f;				CollisionBox_Pos[1] = 0.f;				CollisionBox_Pos[2] = 0.f;
				CollisionBox_Scale[0] = 0.f;				CollisionBox_Scale[1] = 0.f;				CollisionBox_Scale[2] = 0.f;

			}
			return S_OK;
		}

		m_vecBuild.back()->MovePos(fTimeDelta, Position[0], Position[1], Position[2]);
		m_vecBuild.back()->Set_Scale(fTimeDelta, Scale[0], Scale[1], Scale[2]);
		m_vecBuild.back()->Set_CollisionBox(CollisionBox_Scale[0], CollisionBox_Scale[1], CollisionBox_Scale[2], CollisionBox_Pos[0], CollisionBox_Pos[1], CollisionBox_Pos[2]);



		XMVECTOR vTemp = { 0.f, 0.f ,0.f ,1.f }; // 회전축
		_bool bGetKey = false;
		if ((GetAsyncKeyState(VK_LEFT) & 0x8000) && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
		{
			bGetKey = true;
			vTemp = { 0.f, 0.f, -1.f, 1.f };
		}
		else if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
		{
			bGetKey = true;
			vTemp = { 0.f, 0.f, 1.f, 1.f };
		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			bGetKey = true;
			vTemp = { 0.f, -1.f, 0.f, 1.f };
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			bGetKey = true;
			vTemp = { 0.f, 1.f, 0.f, 1.f };
		}
		else if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			bGetKey = true;

			vTemp = { 1.f, 0.f, 0.f, 1.f };
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			bGetKey = true;
			vTemp = { -1.f, 0.f, 0.f, 1.f };
		}
		else
		{
			bGetKey = false;
		}

		if (bGetKey == true)
			m_vecBuild.back()->Set_Turn(fTimeDelta, vTemp);


	}
	else if (m_iModeSelect == IMGUI_SELECT)
	{
		_uint iBuildIndex = 0;
		for (auto& pBuild : m_vecBuild)
		{
			if (true == pBuild->Get_PickingCheck())
			{
				if ((m_pGameInstance->Get_DIMouseState_Down(DIM_RB)) && (GetAsyncKeyState(VK_CONTROL) & 0x8000) && m_iBuild_Count > 0)
				{
					pBuild->Set_DeadEnviron();
					m_vecBuild.erase(m_vecBuild.begin() + iBuildIndex);
					--m_iBuild_Count;
					cout << "남은 Environment 개수 : " << m_iBuild_Count << endl;

					break;
				}

				pBuild->MovePos(fTimeDelta, Position[0], Position[1], Position[2]);
				pBuild->Set_Scale(fTimeDelta, Scale[0], Scale[1], Scale[2]);
				pBuild->Set_CollisionBox(CollisionBox_Scale[0], CollisionBox_Scale[1], CollisionBox_Scale[2], CollisionBox_Pos[0], CollisionBox_Pos[1], CollisionBox_Pos[2]);
				XMVECTOR vTemp = { 0.f, 0.f ,0.f ,1.f }; // 회전축
				_bool bGetKey = false;
				if ((GetAsyncKeyState(VK_LEFT) & 0x8000) && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
				{
					bGetKey = true;
					vTemp = { 0.f, 0.f, -1.f, 1.f };
				}
				else if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
				{
					bGetKey = true;
					vTemp = { 0.f, 0.f, 1.f, 1.f };
				}
				else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
				{
					bGetKey = true;
					vTemp = { 0.f, -1.f, 0.f, 1.f };
				}
				else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
				{
					bGetKey = true;
					vTemp = { 0.f, 1.f, 0.f, 1.f };
				}
				else if (GetAsyncKeyState(VK_UP) & 0x8000)
				{
					bGetKey = true;

					vTemp = { 1.f, 0.f, 0.f, 1.f };
				}
				else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
				{
					bGetKey = true;
					vTemp = { -1.f, 0.f, 0.f, 1.f };
				}

				else
					bGetKey = false;

				if (bGetKey == true)
					pBuild->Set_Turn(fTimeDelta, vTemp);

				break;
			}
			iBuildIndex++;
		}

	}
	return S_OK;
}

void CLevel_ImGui::Build_Save()
{
	HANDLE hFile = CreateFile(L"../Bin/Data/Build.dat", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Save Build File Creation Failed", L"Error", MB_OK);
		return;
	}
	DWORD dwByte = 0;
	_float3 fPos;
	for (auto& pBuild : m_vecBuild)
	{
		if (pBuild)
		{
			LEVELID iLevel = pBuild->Get_Level();
			_int  iModelIndex = pBuild->Get_ModelIndex();
			_uint iImGuiMode = pBuild->Get_ImGuiMode();

			_vector vPos = pBuild->Get_Pos();
			XMStoreFloat3(&fPos, vPos);
			_float3 fScale = pBuild->Get_Scale();
			_float3 fCollisionScale = pBuild->Get_CollisionBoxScale();
			_vector vecCollisionPos = pBuild->Get_CollisionBoxPos();
			_vector	vRight{};
			_vector	vUp{};
			_vector	vLook{};

			pBuild->Get_Rotation(vRight, vUp, vLook);

			WriteFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr);
			WriteFile(hFile, &iModelIndex, sizeof(_int), &dwByte, nullptr);
			WriteFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
			WriteFile(hFile, &fScale, sizeof(_float3), &dwByte, nullptr);
			WriteFile(hFile, &fCollisionScale, sizeof(_float3), &dwByte, nullptr);
			WriteFile(hFile, &iImGuiMode, sizeof(_uint), &dwByte, nullptr);


			WriteFile(hFile, &vecCollisionPos, sizeof(_vector), &dwByte, nullptr);
			WriteFile(hFile, &vRight, sizeof(_vector), &dwByte, nullptr);
			WriteFile(hFile, &vUp, sizeof(_vector), &dwByte, nullptr);
			WriteFile(hFile, &vLook, sizeof(_vector), &dwByte, nullptr);

			m_vecBuildIndex.push_back(iModelIndex);
		}
	}
	CloseHandle(hFile);
	MessageBox(NULL, L"Build Saved Successfully", L"Success", MB_OK);

	sort(m_vecBuildIndex.begin(), m_vecBuildIndex.end());
	vector<_int>::iterator iter = unique(m_vecBuildIndex.begin(), m_vecBuildIndex.end());
	m_vecBuildIndex.erase(iter, m_vecBuildIndex.end());

	HANDLE hIndexFile = CreateFile(L"../Bin/Data/GamePlayLevel_Build_Index.dat", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hIndexFile)
	{
		MessageBox(NULL, L"Save GamePlayLevel_Build_Index File Creation Failed", L"Error", MB_OK);
		return;
	}

	dwByte = 0;

	for (auto& pIndex : m_vecBuildIndex)
	{
		pIndex -= ENVIRONMENT_EA;
		WriteFile(hIndexFile, &pIndex, sizeof(_int), &dwByte, nullptr);
		cout << pIndex << endl;
	}

	CloseHandle(hIndexFile);
	MessageBox(NULL, L"GamePlayLevel_Build_Index Saved Successfully", L"Success", MB_OK);


}

void CLevel_ImGui::Build_Load()
{
	for (auto& pBuild : m_vecBuild)
	{
		pBuild->Set_DeadEnviron();
	}
	m_vecBuild.clear();


	m_iBuild_Count = 0;
	HANDLE hFile = CreateFile(L"../Bin/Data/Build.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Build File Failed", L"Error", MB_OK);
		return;
	}
	DWORD dwByte = 0;
	LEVELID iLevel;
	_int  iModelIndex;
	_float3 fPos{}, fScale{}, fCollisionBoxPos{}, fCollisionBoxScale{};
	_vector	vRight{}, vUp{}, vLook{}, vecCollisionPos{};
	_uint iImGuiMode{};
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
		Desc.eID = iLevel;
		Desc.fPosition = fPos;
		Desc.iModelComponentIndex = iModelIndex;
		Desc.fScale = fScale;
		//cout << fScale.x << "     " << fScale.y << "            " << fScale.z << endl;
		pGameObj = (m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_IMGUI, TEXT("Layer_Environment"),
			TEXT("Prototype_GameObject_Environment_ImGui"), &Desc));
		if (pGameObj != nullptr)
		{
			Position[0] = fPos.x;			Position[1] = fPos.y;			Position[2] = fPos.z;
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Scale(0.f, fScale.x, fScale.y, fScale.z);
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Rotaion(vRight, vUp, vLook);
			Scale[0] = fScale.x;			Scale[1] = fScale.y;			Scale[2] = fScale.z;


			XMStoreFloat3(&fCollisionBoxPos, vecCollisionPos);
			Desc.CollisionBoxScale.x = fCollisionBoxScale.x;		Desc.CollisionBoxScale.y = fCollisionBoxScale.y;		Desc.CollisionBoxScale.z = fCollisionBoxScale.z;
			Desc.CollisionBoxPos.x = fCollisionBoxPos.x;			Desc.CollisionBoxPos.y = fCollisionBoxPos.y;			Desc.CollisionBoxPos.z = fCollisionBoxPos.z;

			dynamic_cast<CEnvironment*>(pGameObj)->Set_CollisionBox(fCollisionBoxScale.x, fCollisionBoxScale.y, fCollisionBoxScale.z, fCollisionBoxPos.x, fCollisionBoxPos.y, fCollisionBoxPos.z);


			if(m_iBuild_Count > 0)
				m_vecBuild.back()->Set_PickingCheck(false);
			m_vecBuild.push_back(dynamic_cast<CEnvironment*>(pGameObj));
			m_vecBuild.back()->Set_PickingCheck(true);
			m_iBuild_Count++;
		}
	}

	CloseHandle(hFile);
	MessageBox(NULL, L"Build Loaded Successfully", L"Success", MB_OK);
}

HRESULT CLevel_ImGui::Build_Select()
{
	if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && m_iModeSelect == IMGUI_SELECT && bAble_Select == true)
	{
		_float3 fMousePos = m_pGameInstance->Get_MousePos_NDC(g_hWnd, g_iWinSizeX, g_iWinSizeY);
		XMMATRIX invProj = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_PROJ);
		XMMATRIX invView = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW);
		XMVECTOR RayPos, RayDir;
		m_pGameInstance->Get_MouseRayDirection(fMousePos, invProj, invView, &RayPos, &RayDir);
		RayDir = XMVector3Normalize(RayDir);

		for (auto& pBuild : m_vecBuild)
		{
			_vector vBoxPos = pBuild->Get_CollisionBoxPos();
			XMFLOAT3 fBoxPos{};
			XMStoreFloat3(&fBoxPos, vBoxPos);
			XMFLOAT3 fBoxSize = pBuild->Get_CollisionBoxScale();

			XMFLOAT3 fMinPoint, fMaxPoint;
			m_pGameInstance->CreateBoundingBox(fBoxPos, fBoxSize, fMinPoint, fMaxPoint);
			_bool bPickCheck = false;
			float distance;
			if (m_pGameInstance->Picking_Box(RayPos, RayDir, fMinPoint, fMaxPoint, distance, pBuild->Get_BoundingBox()))
			{
				bPickCheck = true;
				pBuild->Set_PickingCheck(bPickCheck);
				_vector vecBuildPos = pBuild->Get_Pos();
				_float3 fBuildScale = pBuild->Get_Scale();
				_float3 fBuildPos{};
				XMStoreFloat3(&fBuildPos, vecBuildPos);


				Position[0] = fBuildPos.x;				Position[1] = fBuildPos.y;				Position[2] = fBuildPos.z;
				Scale[0] = fBuildScale.x;				Scale[1] = fBuildScale.y;				Scale[2] = fBuildScale.z;

				CollisionBox_Pos[0] = fBoxPos.x;				CollisionBox_Pos[1] = fBoxPos.y;				CollisionBox_Pos[2] = fBoxPos.z;
				CollisionBox_Scale[0] = fBoxSize.x;				CollisionBox_Scale[1] = fBoxSize.y;				CollisionBox_Scale[2] = fBoxSize.z;

				bAble_Select = false;

				
			}
			pBuild->Set_PickingCheck(bPickCheck);

		}
	}

	return S_OK;
}

HRESULT CLevel_ImGui::Item_Add()
{
	if (m_fPickingPos.x == 0 && m_fPickingPos.y == 0 && m_fPickingPos.z == 0)
		return S_OK;

	CCoin::COIN_DESC			Desc{};
	Desc.eID = LEVEL_IMGUI;
	m_fPickingPos.y = 3.f;
	Desc.fPosition = m_fPickingPos;
	Desc.fScale = { 15.f,15.f ,15.f };
	Position[0] = m_fPickingPos.x;	Position[1] = m_fPickingPos.y;	Position[2] = m_fPickingPos.z;
	Scale[0] = Desc.fScale.x;		Scale[1] = Desc.fScale.y;		Scale[2] = Desc.fScale.z;

	pGameObj = (m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_IMGUI, TEXT("Layer_Coin"),
		TEXT("Prototype_GameObject_Coin"), &Desc));
	if (pGameObj != nullptr)
	{
		m_vecCoin.push_back(dynamic_cast<CCoin*>(pGameObj));

		m_iCoin_Count++;
		bAble_Select = false;
	}
	return S_OK;
}

HRESULT CLevel_ImGui::Item_DataChange(_float fTimeDelta)
{
	if (m_iModeSelect == IMGUI_CREATE)   // Create 모드 일 때 가장 최근 설치 항목에 대한 수정 가능 기능
	{
		// 가장 최근 설치한 Environment 삭제하기
		if ((m_pGameInstance->Get_DIMouseState_Down(DIM_RB)) && (GetAsyncKeyState(VK_CONTROL) & 0x8000) && m_iCoin_Count > 0)
		{
			m_vecCoin.back()->Set_Dead();
			m_vecCoin.erase(m_vecCoin.end() - 1);
			--m_iCoin_Count;
			cout << "남은 Coin 개수 : " << m_iCoin_Count << endl;

			if (m_iCoin_Count > 0)
			{
				_float3 fPos, fScale;
				_vector vPos = m_vecCoin.back()->Get_Pos();
				fScale = m_vecCoin.back()->Get_Scale();
				_float3 fEnvironPos{};

				XMStoreFloat3(&fPos, vPos);
				Position[0] = fPos.x;				Position[1] = fPos.y;				Position[2] = fPos.z;
				Scale[0] = fScale.x;				Scale[1] = fScale.y;				Scale[2] = fScale.z;

			}
			else
			{
				Position[0] = 0.f;				Position[1] = 0.f;				Position[2] = 0.f;
				Scale[0] = 0.f;					Scale[1] = 0.f;					Scale[2] = 0.f;
			}
			return S_OK;
		}


		m_vecCoin.back()->MovePos( Position[0], Position[1], Position[2]);
		m_vecCoin.back()->Set_Scale(fTimeDelta, Scale[0], Scale[1], Scale[2]);

	}
	return S_OK;
}

void CLevel_ImGui::Item_Save()
{
	HANDLE hFile = CreateFile(L"../Bin/Data/Coin.dat", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Save Coin File Creation Failed", L"Error", MB_OK);
		return;
	}
	DWORD dwByte = 0;
	_float3 fPos;
	for (auto& pCoin : m_vecCoin)
	{
		if (pCoin)
		{
			LEVELID iLevel = pCoin->Get_Level();
			_vector vPos = pCoin->Get_Pos();
			XMStoreFloat3(&fPos, vPos);
			_float3 fScale = pCoin->Get_Scale();

			WriteFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr);
			WriteFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
			WriteFile(hFile, &fScale, sizeof(_float3), &dwByte, nullptr);
		}
	}
	CloseHandle(hFile);
	MessageBox(NULL, L"Coin Saved Successfully", L"Success", MB_OK);

}

void CLevel_ImGui::Item_Load()
{
	for (auto& pCoin : m_vecCoin)
	{
		pCoin->Set_Dead();
	}
	m_vecCoin.clear();


	m_iCoin_Count = 0;
	HANDLE hFile = CreateFile(L"../Bin/Data/Coin.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load Coin File Failed", L"Error", MB_OK);
		return;
	}
	DWORD dwByte = 0;
	LEVELID iLevel;
	_float3 fPos{}, fScale{};
	
	while (ReadFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr) && dwByte > 0)
	{
		ReadFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &fScale, sizeof(_float3), &dwByte, nullptr);

		CCoin::COIN_DESC			Desc{};
		Desc.eID = iLevel;
		Desc.fPosition = fPos;
		Desc.fScale = fScale;
		pGameObj = (m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_IMGUI, TEXT("Layer_Coin"),
			TEXT("Prototype_GameObject_Coin"), &Desc));
		if (pGameObj != nullptr)
		{

			Position[0] = fPos.x;			Position[1] = fPos.y;			Position[2] = fPos.z;
			dynamic_cast<CCoin*>(pGameObj)->Set_Scale(0.f, fScale.x, fScale.y, fScale.z);
			dynamic_cast<CCoin*>(pGameObj)->MovePos(fPos.x, fPos.y, fPos.z);
			Scale[0] = fScale.x;			Scale[1] = fScale.y;			Scale[2] = fScale.z;
	
			m_vecCoin.push_back(dynamic_cast<CCoin*>(pGameObj));
			m_iCoin_Count++;
		}
	}

	CloseHandle(hFile);
	MessageBox(NULL, L"Coin Loaded Successfully", L"Success", MB_OK);
}

HRESULT CLevel_ImGui::Item_Select()
{
	return S_OK;
}

CLevel_ImGui* CLevel_ImGui::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_ImGui* pInstance = new CLevel_ImGui(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_ImGui");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_ImGui::Free()
{
	__super::Free();
	Safe_Release(m_pSave);
	Safe_Release(m_pLoad);
	Safe_Release(m_pEnviron);
	Safe_Release(m_pBuild);

}

void CLevel_ImGui::Create_ImageButton()
{
	m_pSave = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Save.jpg"));
	m_pLoad = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Load.jpg"));

	m_pEnviron = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ImGui/Button/Environment%d.png"), ENVIRONMENT_EA);
	m_pBuild = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ImGui/Button/Build%d.png"), BUILD_EA);

	// 사진의 리소스뷰 가져오기
	my_Savetexture = *m_pSave->Get_SRV().begin();
	my_Loadtexture = *m_pLoad->Get_SRV().begin();
	
}



HRESULT CLevel_ImGui::Ready_Layer_Camera(const _tchar* pLayerTag)
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
	Desc.eLevel = LEVEL_IMGUI;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_IMGUI, pLayerTag,
		TEXT("Prototype_GameObject_Camera_Free_ImGui"), &Desc)))
		return E_FAIL;

	return S_OK;
}
HRESULT CLevel_ImGui::Ready_Lights()
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
HRESULT CLevel_ImGui::Ready_Layer_Monster(const _tchar* pLayerTag)
{
	//CMonster::MONSTER_DESC			Desc{};
	//Desc.eID = LEVEL_IMGUI;

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_IMGUI, pLayerTag,
	//	TEXT("Prototype_GameObject_Monster_ImGui"), &Desc)))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CLevel_ImGui::Ready_Layer_Player(const _tchar* pLayerTag)
{
	//CGameObject::GAMEOBJ_DESC Desc{};
	//CGameObject* pPlayer = m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_IMGUI, pLayerTag, TEXT("Prototype_GameObject_Player"), &Desc);
	//m_pPlayer = static_cast<CPlayer*>(pPlayer);
	return S_OK;
}

