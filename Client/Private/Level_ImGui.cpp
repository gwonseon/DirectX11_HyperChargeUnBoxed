#include "stdafx.h"
#include "..\Public\Level_ImGui.h"


#include "Camera_Free.h"
#include "Monster.h"
#include "Level_Loading.h"

CLevel_ImGui::CLevel_ImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_ImGui::Initialize()
{
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))			return E_FAIL;	// 카메라 생성
	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))			return E_FAIL;	// 지형 생성
	//if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))			return E_FAIL;	// 몬스터
	if (FAILED(Ready_Lights()))										return E_FAIL;	// 빛

	if (FAILED(m_pGameInstance->Close_Level(LEVEL_LOADING)))		return E_FAIL;	// 로딩 닫기

	pVIBuffer_Terrain = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_IMGUI, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));

	Create_ImageButton();
	
    return S_OK;
}

void CLevel_ImGui::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

	
	
	if (GetKeyState(VK_ADD) & 0x8000)
	{
		_float3 fScale = dynamic_cast<CEnvironment*>(pGameObj)->Get_Scale();
	}

	// 모드 변경
	if (GetAsyncKeyState(VK_F1) & 0x0001)  // 애니 없는 모델
	{
		m_eImGui_Type = IMGUI_OBJECT_NONANIM;
		m_fPickingPos = { 0.f,0.f,0.f };
	}
	if (GetAsyncKeyState(VK_F2) & 0x0001)  // 애니 있는 모델
	{
		m_eImGui_Type = IMGUI_OBJECT_ANIM;
		m_fPickingPos = { 0.f,0.f,0.f };
	}
	if (GetAsyncKeyState(VK_F3) & 0x0001) // 맵툴
	{
		m_eImGui_Type = IMGUI_MAPTOOL;
		m_fPickingPos = { 0.f,0.f,0.f };
	}


	// ESC 누르면 창 나가짐
	if (GetKeyState(VK_ESCAPE) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_IMGUI, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOGO))))
			return;
	}

	switch (m_eImGui_Type)
	{
	case Client::CLevel_ImGui::IMGUI_OBJECT_NONANIM:
		Object_NonAnim_Update(fTimeDelta);
		break;
	case Client::CLevel_ImGui::IMGUI_OBJECT_ANIM:
		break;
	case Client::CLevel_ImGui::IMGUI_MAPTOOL:
		break;
	case Client::CLevel_ImGui::IMGUI_END:
		break;
	default:
		break;
	}
}

HRESULT CLevel_ImGui::Render()
{
    __super::Render();
	ImGui::SetWindowPos("ParentWindow", ImVec2(0, 200));
	ImGui::SetNextWindowSize(ImVec2(400,600)); // 가로 400, 세로 300 크기로 설정
	ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(800, 600)); // 최소 크기 200x200, 최대 크기 800x600
	ImGui::Begin("ParentWindow");
	// ImGui 코드 작성칸
	switch (m_eImGui_Type)
	{
	case Client::CLevel_ImGui::IMGUI_OBJECT_NONANIM:
		Object_NonAnim();
		break;
	case Client::CLevel_ImGui::IMGUI_OBJECT_ANIM:
		Object_Anim();
		break;
	case Client::CLevel_ImGui::IMGUI_MAPTOOL:
		MapTool();
		break;
	case Client::CLevel_ImGui::IMGUI_END:
		break;
	default:
		break;
	}


	ImGui::End();
    

	ImGui::SetWindowPos("Save_Load", ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(300, 100)); // 가로 400, 세로 300 크기로 설정
	ImGui::SetNextWindowSizeConstraints(ImVec2(30, 30), ImVec2(150, 100)); // 최소 크기 200x200, 최대 크기 800x600
	ImGui::Begin("Save_Load");
	if (ImGui::ImageButton("Save", my_Savetexture, ImVec2(50, 50), ImVec2(0, 0)))
	{
		Save = true;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton("Load", my_Loadtexture, ImVec2(50, 50), ImVec2(0, 0)))
	{
		Environment_Load();
	}
	ImGui::End();



#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("ImGui레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_ImGui::Ready_Layer_Terrain(const _tchar* pLayerTag)
{

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_IMGUI, pLayerTag, TEXT("Prototype_GameObject_Terrain_ImGui"))))
		return E_FAIL;
	return S_OK;
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
	Desc.fSpeedPerSec = 20.f;
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
	CMonster::MONSTER_DESC			Desc{};
	Desc.eID = LEVEL_IMGUI;
	
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_IMGUI, pLayerTag,
		TEXT("Prototype_GameObject_Monster_ImGui"),&Desc)))
		return E_FAIL;


	return S_OK;
}


HRESULT CLevel_ImGui::Picking_Create()
{
	if ((GetAsyncKeyState(VK_LBUTTON) & 0x0001) && (bAble_Select == true))
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
			Environment_Add();

	
	}
	return S_OK;
}

HRESULT CLevel_ImGui::Picking_Select()
{
	return S_OK;
}

void CLevel_ImGui::Object_NonAnim_Update(_float fTimeDelta)
{	/*
	if (m_iEnvironment_Count > 0)
	{
		for (auto iter = m_vecEnvironment.begin(); iter != m_vecEnvironment.end();)
		{
			if (nullptr != (*iter))
			{
				if ((*iter)->Get_Dead() == true)
				{
					iter = m_vecEnvironment.erase(iter);
					m_iEnvironment_Count--;
				}
				else
					++iter;
			}
			else
			{
				++iter;
			}
		}
	}
	*/
	
	if (m_iEnvironment_Count > 0)
		Environment_DataChange(fTimeDelta);
	Picking_Create();
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		if (GetAsyncKeyState('S') & 0x8000)
		{
			Environment_Save();
		}
	}

	if(GetAsyncKeyState(VK_TAB))
	{
		Environment_Load();
	}

	if (Save == true)
	{
		Environment_Save();
		Save = false;
	}
}

void CLevel_ImGui::Object_Anim_Update(_float fTimeDelta)
{
}

void CLevel_ImGui::MapTool_Update(_float fTimeDelta)
{
}

void CLevel_ImGui::Object_NonAnim()
{
	const char* pText = "NonAnim Object Tool";
	ImGui::Text(pText);
	ImGui::Text(" ");

	// 위치 크기 방향 수정창
	ImGui::Text("Object Data");
	ImGui::DragFloat3("Position", Position, 0.1f, -90.f, 90.f);
	ImGui::DragFloat3("Scale", Scale, 0.1f, -90.f, 90.f);
	// 모델 선택창
	ImGui::Text(" ");
	ImGui::Text(" ");
	ImGui::Text(" ");
//	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Text("Environment List ");
	ImGui::BeginChild("Scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_None);
	ImGui::InputInt("ModelIndex", &m_iModelIndex, 0);
	Environment_List(); // ImGui 선택 리스트 ( Environment 리스트 )
	ImGui::EndChild();
//	ImGui::PopStyleVar();



}

void CLevel_ImGui::Object_Anim()
{
	const char* pText = "Anim Object Tool";
	ImGui::Text(pText);


}

void CLevel_ImGui::MapTool()
{
	const char* pText = "MapTool";
	ImGui::Text(pText);

}

void CLevel_ImGui::Environment_List()
{

	ImGui::BeginChild("Choose Environment");
	if (ImGui::ImageButton("Chair0", SRV_m_pChair0, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 0;
	ImGui::SameLine();
	if (ImGui::ImageButton("Chair1", SRV_m_pChair1, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 1;
	ImGui::SameLine();
	if (ImGui::ImageButton("Chair2", SRV_m_pChair2, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 2;
	ImGui::SameLine();
	if (ImGui::ImageButton("Chair3", SRV_m_pChair3, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 3;
	ImGui::SameLine();
	if (ImGui::ImageButton("Chair4", SRV_m_pChair4, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 4;
	if (ImGui::ImageButton("Chair5", SRV_m_pChair5, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 5;
	ImGui::SameLine();
	if (ImGui::ImageButton("AidKit", SRV_AidKit, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 6;
	ImGui::SameLine();
	if (ImGui::ImageButton("Card0", SRV_Card0, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 7;
	ImGui::SameLine();
	if (ImGui::ImageButton("WasteBin", SRV_WasteBin, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 8;
	ImGui::SameLine();
	if (ImGui::ImageButton("Desk0", SRV_Desk0, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 9;
	if (ImGui::ImageButton("Desk1", SRV_Desk1, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 10;
	ImGui::SameLine();
	if (ImGui::ImageButton("Desk2", SRV_Desk2, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 11;
	ImGui::SameLine();
	if (ImGui::ImageButton("Desk3", SRV_Desk3, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 12;
	ImGui::SameLine();
	if (ImGui::ImageButton("KeyPad", SRV_KeyPad, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 13;
	ImGui::SameLine();
	if (ImGui::ImageButton("Vent1", SRV_Vent1, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 14;
	if (ImGui::ImageButton("Sprinkler", SRV_Sprinkler, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 15;
	ImGui::SameLine();
	if (ImGui::ImageButton("Trim", SRV_Trim0, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 16;
	ImGui::SameLine();
	if (ImGui::ImageButton("Vent0", SRV_Vent0, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 17;
	ImGui::SameLine();
	if (ImGui::ImageButton("card1", SRV_Card1, ImVec2(50, 50), ImVec2(0, 0)))
		m_iModelIndex = 18;
	ImGui::SameLine();

	ImGui::EndChild();
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
	Position[0] = m_fPickingPos.x;
	Position[1] = m_fPickingPos.y;
	Position[2] = m_fPickingPos.z;
	Scale[0] = Desc.fScale.x;
	Scale[1] = Desc.fScale.y;
	Scale[2] = Desc.fScale.z;
	pGameObj = (m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_IMGUI, TEXT("Layer_Environment"),
		TEXT("Prototype_GameObject_Environment_ImGui"), &Desc));
	if (pGameObj != nullptr)
	{
		
		m_vecEnvironment.push_back(dynamic_cast<CEnvironment*>(pGameObj));
		m_iEnvironment_Count++;
		bAble_Select = false;
	}

	return S_OK;
}

HRESULT CLevel_ImGui::Environment_DataChange(_float fTimeDelta)
{
	// 가장 최근 설치한 Environment 삭제하기
	if ((GetAsyncKeyState(VK_RBUTTON) & 0x0001) && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
			m_vecEnvironment.back()->Set_Dead();
			m_vecEnvironment.erase(m_vecEnvironment.end() - 1);
			--m_iEnvironment_Count;
			bAble_Select = true;
			if (m_iEnvironment_Count > 0)
			{
				_float3 fPos, fScale;
				_vector vPos = m_vecEnvironment.back()->Get_Pos();
				fScale = m_vecEnvironment.back()->Get_Scale();
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

	m_vecEnvironment.back()->MovePos(fTimeDelta,Position[0], Position[1], Position[2]);
	m_vecEnvironment.back()->Set_Scale(fTimeDelta, Scale[0], Scale[1], Scale[2]);
	
	
	XMVECTOR vTemp = {0.f, 0.f ,0.f ,1.f };
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

	if(bGetKey == true)
		m_vecEnvironment.back()->Set_Turn(fTimeDelta, vTemp);

	if ((GetAsyncKeyState(VK_RETURN) & 0x8000) && (bAble_Select == false))
	{
		bAble_Select = true;
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
			_vector vPos = environment->Get_Pos();
			XMStoreFloat3(&fPos, vPos);
			_float3 fScale = environment->Get_Scale();

			_vector	vRight{};
			_vector	vUp{};
			_vector	vLook{};

			environment->Get_Rotation(vRight, vUp, vLook);
			//_float3 a, b, c;
			//XMStoreFloat3(&a, vRight);
			//cout << a.x << "     " << a.y << "       " << a.z << endl;

			//XMStoreFloat3(&b, vUp);
			//cout << b.x << "     " << b.y << "       " << b.z << endl;

			//XMStoreFloat3(&c, vLook);
			//cout << c.x << "     " << c.y << "       " << c.z << endl;


			WriteFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr);
			WriteFile(hFile, &iModelIndex, sizeof(_int), &dwByte, nullptr);
			WriteFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
			WriteFile(hFile, &fScale, sizeof(_float3), &dwByte, nullptr);

			WriteFile(hFile, &vRight, sizeof(_vector), &dwByte, nullptr);
			WriteFile(hFile, &vUp, sizeof(_vector), &dwByte, nullptr);
			WriteFile(hFile, &vLook, sizeof(_vector), &dwByte, nullptr);


		}
	}

	CloseHandle(hFile);
	MessageBox(NULL, L"Environment Saved Successfully", L"Success", MB_OK);


}

void CLevel_ImGui::Environment_Load()
{
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
		

			m_vecEnvironment.push_back(dynamic_cast<CEnvironment*>(pGameObj));
			m_iEnvironment_Count++;
		}
	}



	CloseHandle(hFile);
	MessageBox(NULL, L"Environment Loaded Successfully", L"Success", MB_OK);
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
	Button_Free();


}

void CLevel_ImGui::Create_ImageButton()
{
	// 저장 로드 버튼
	m_pSave = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Save.jpg"));
	m_pLoad = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Load.jpg"));

	m_pChair0	= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/Chair0.png"));
	m_pChair1	= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/Chair1.png"));
	m_pChair2	= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/Chair2.png"));
	m_pChair3	= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/Chair3.png"));
	m_pChair4	= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/Chair4.png"));
	m_pChair5	= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/Chair5.png"));
	AidKit		= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/AidKit.png"));
	Card0		= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/Card0.png"));
	Card1		= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/Card1.png"));
	Desk0		= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/Desk0.png"));
	Desk1		= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/Desk1.png"));
	Desk2		= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/Desk2.png"));
	Desk3		= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/Desk3.png"));
	KeyPad		= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/KeyPad.png"));
	Sprinkler	= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/Sprinkler.png"));
	Trim0		= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/Trim0.png"));
	Vent0		= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/Vent0.png"));
	Vent1		= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/Vent1.png"));
	WasteBin	= CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Environment/WasteBin.png"));

	// 사진의 리소스뷰 가져오기
	my_Savetexture = *m_pSave->Get_SRV().begin();
	my_Loadtexture = *m_pLoad->Get_SRV().begin();


	SRV_m_pChair0			= *m_pChair0->Get_SRV().begin();
	SRV_m_pChair1			= *m_pChair1->Get_SRV().begin();
	SRV_m_pChair2			= *m_pChair2->Get_SRV().begin();
	SRV_m_pChair3			= *m_pChair3->Get_SRV().begin();
	SRV_m_pChair4			= *m_pChair4->Get_SRV().begin();
	SRV_m_pChair5			= *m_pChair5->Get_SRV().begin();
	SRV_AidKit				= *AidKit->Get_SRV().begin();
	SRV_Card0				= *Card0->Get_SRV().begin();
	SRV_Card1				= *Card1->Get_SRV().begin();
	SRV_Desk0				= *Desk0->Get_SRV().begin();
	SRV_Desk1				= *Desk1->Get_SRV().begin();
	SRV_Desk2				= *Desk2->Get_SRV().begin();
	SRV_Desk3				= *Desk3->Get_SRV().begin();
	SRV_KeyPad				= *KeyPad->Get_SRV().begin();
	SRV_Sprinkler			= *Sprinkler->Get_SRV().begin();
	SRV_Trim0				= *Trim0->Get_SRV().begin();
	SRV_Vent0				= *Vent0->Get_SRV().begin();
	SRV_Vent1				= *Vent1->Get_SRV().begin();
	SRV_WasteBin			= *WasteBin->Get_SRV().begin();
}

void CLevel_ImGui::Button_Free()
{
	Safe_Release(	m_pSave		);
	Safe_Release(	m_pLoad		);
	Safe_Release(	m_pChair0	);
	Safe_Release(	m_pChair1	);
	Safe_Release(	m_pChair2	);
	Safe_Release(	m_pChair3	);
	Safe_Release(	m_pChair4	);
	Safe_Release(	m_pChair5	);
	Safe_Release(	AidKit		);
	Safe_Release(	Card0		);
	Safe_Release(	Card1		);
	Safe_Release(	Desk0		);
	Safe_Release(	Desk1		);
	Safe_Release(	Desk2		);
	Safe_Release(	Desk3		);
	Safe_Release(	KeyPad		);
	Safe_Release(	Sprinkler	);
	Safe_Release(	Trim0		);
	Safe_Release(	Vent0		);
	Safe_Release(	Vent1		);
	Safe_Release(	WasteBin);
}