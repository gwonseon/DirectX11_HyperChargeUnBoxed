#include "stdafx.h"
#include "..\Public\Monster_Path.h"


#include "Camera_Free.h"
#include "Monster.h"
#include "Level_Loading.h"

CMonster_Path::CMonster_Path(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
	: CLevel{pDevice,pContext}
{}

HRESULT CMonster_Path::Initialize()
{
	m_eTargetID = LEVEL_GAMEPLAY;

	ShowCursor(true);
	if(FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))			return E_FAIL;	// 카메라 생성
	if(FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))			return E_FAIL;	// 지형 생성
	if(FAILED(Ready_Lights()))										return E_FAIL;	// 빛
	if(FAILED(m_pGameInstance->Close_Level(LEVEL_LOADING)))		return E_FAIL;	// 로딩 닫기
	Load_Map();

	pVIBuffer_Terrain = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_MONSTERSPAWN,TEXT("Layer_Terrain"),TEXT("Com_VIBuffer")));

	// 저장 로드 버튼(이미지 버튼)
	Create_ImageButton();
	m_iLevel = m_eTargetID;  // 찍을 때 디폴트 값(원하는 레벨)
	m_iCellIndex = 0;
	m_iRound = 0;

	return S_OK;
}

void CMonster_Path::Update(_float fTimeDelta)
{

	if(m_bOnce == false)
	{	// 네비의 중간 콜리전 박스 포인터를 담은 벡터
		m_vecCollisionCenter = m_pTerrain->Get_Collision_Center();
		m_bOnce = true;
	}
	__super::Update(fTimeDelta);
	Picking_Create(); // 클릭한 위치에 생성

	if(m_pGameInstance->Get_DIKeyState_Down(DIK_RETURN))
	{
		m_bAdd = true;
	}
	if(m_pGameInstance->Get_DIKeyState_Down(DIK_F1))	// 1라운드 저장
	{
		Save_FirstRound();
	}
	if(m_pGameInstance->Get_DIKeyState_Down(DIK_F2))	// 2라운드 저장
	{
		Save_SecondRound();
	}
	if(m_pGameInstance->Get_DIKeyState_Down(DIK_F3))	// 3라운드 저장
	{
		Save_ThirdRound();
	}
	// 데이터 추가
	if(m_bAdd == true)
	{
		Add_Data();
		m_bAdd = false;
	}

	// 불러오기
	if(m_bLoad == true)
	{
		Load();
		m_bLoad = false;
	}

}

HRESULT CMonster_Path::Render()
{
	__super::Render();

	ImGui::SetNextWindowSize(ImVec2(400,600)); // 가로 400, 세로 600 크기로 설정
	ImGui::SetNextWindowSizeConstraints(ImVec2(200,200),ImVec2(800,600)); // 최소 크기 200x200, 최대 크기 800x600
	ImGui::Begin("ParentWindow",nullptr,ImGuiWindowFlags_None); // 창 이동 가능

	const char* pText = "Monster Spawn";
	ImGui::Text("0Round is BreakTime");
	ImGui::Text("F1 F2 F3 -> Save");  // 각 라운드별 저장

	ImGui::Text(pText);
	ImGui::Text(" ");

	ImGui::Text("Level Data");
	// GameplayLevel은 3
	ImGui::InputInt("Level",&m_iLevel);

	ImGui::Text("Round Data");
	ImGui::Text("Round 0은 안됨");
	ImGui::InputInt("Round",&m_iRound);

	ImGui::Text("Position Data");
	ImGui::DragFloat3("Position",Position,0.1f,-200.f,3000.f);
	ImGui::Text("Cell Index: %d",m_iCellIndex);
	ImGui::Text("Model Index: %d",m_iModelIndex);
	ButtonImage_List();
	ImGui::End();

	return S_OK;
}

HRESULT CMonster_Path::Ready_Layer_Terrain(const _tchar* pLayerTag)
{
	CTerrain::TERRAIN_DESC pDesc{};
	pDesc.eID = LEVEL_MONSTERSPAWN;
	pDesc.eTargetID = m_eTargetID;
	m_pTerrain = static_cast<CTerrain*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_MONSTERSPAWN,pLayerTag,TEXT("Prototype_GameObject_Terrain"),&pDesc));

	return S_OK;
}

HRESULT CMonster_Path::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC			Desc{};
	Desc.vEye = _float4(0.f,10.f,-5.f,1.f);
	Desc.vAt = _float4(0.f,0.f,0.f,1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNearZ = 0.1f;
	Desc.fFar = 500.f;
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fMouseSensor = 0.05f;
	Desc.eLevel = LEVEL_MONSTERSPAWN;
	if(FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_MONSTERSPAWN,pLayerTag,
		TEXT("Prototype_GameObject_Camera_Free"),&Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Path::Ready_Lights()
{
	LIGHT_DESC	LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f,-1.f,1.f,0.f);
	LightDesc.vDiffuse = _float4(1.f,1.f,1.f,1.f);
	LightDesc.vAmbient = _float4(1.f,1.f,1.f,1.f);
	LightDesc.vSpecular = _float4(1.f,1.f,1.f,1.f);

	if(FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}



void CMonster_Path::Save_FirstRound()
{
	_ulong		dwByte = {0};
	_wstring strLast = TEXT(".dat");
	_wstring strPath{};
	switch(m_eTargetID)
	{
	case Client::LEVEL_GAMEPLAY:
	strPath = TEXT("../Bin/Data/Gameplay_Monster");
	break;
	case Client::LEVEL_YARD:
	strPath = TEXT("../Bin/Data/Yard_Monster");
	break;
	default:
	break;
	}

	_wstring Path_Result = strPath + to_wstring(1) + strLast;
	HANDLE		hFile = CreateFile(Path_Result.c_str(),GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if(0 == hFile)
		return;


	for(auto& pMonster : m_vecMonsterSpawn[m_eTargetID][1])
	{
		_float3 fPos = pMonster.fPos;
		_uint	iModelIdx = pMonster.iModel_Idx;
		_uint	iCellIdx = pMonster.iCellIdx;
		WriteFile(hFile,&iModelIdx,sizeof(_uint),&dwByte,nullptr);
		WriteFile(hFile,&fPos,sizeof(_float3),&dwByte,nullptr);
		WriteFile(hFile,&iCellIdx,sizeof(_uint),&dwByte,nullptr);
	}
	CloseHandle(hFile);
	MessageBox(NULL,L"Monster1Round Saved Successfully",L"Success",MB_OK);
}

void CMonster_Path::Save_SecondRound()
{
	_ulong		dwByte = {0};
	_wstring strLast = TEXT(".dat");
	_wstring strPath{};
	switch(m_eTargetID)
	{
	case Client::LEVEL_GAMEPLAY:
	strPath = TEXT("../Bin/Data/Gameplay_Monster");
	break;
	case Client::LEVEL_YARD:
	strPath = TEXT("../Bin/Data/Yard_Monster");
	break;
	default:
	break;
	}
	_wstring Path_Result = strPath + to_wstring(2) + strLast;
	HANDLE		hFile = CreateFile(Path_Result.c_str(),GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if(0 == hFile)
		return;
	for(auto& pMonster : m_vecMonsterSpawn[m_eTargetID][2])
	{
		_float3 fPos = pMonster.fPos;
		_uint	iModelIdx = pMonster.iModel_Idx;
		_uint	iCellIdx = pMonster.iCellIdx;
		WriteFile(hFile,&iModelIdx,sizeof(_uint),&dwByte,nullptr);
		WriteFile(hFile,&fPos,sizeof(_float3),&dwByte,nullptr);
		WriteFile(hFile,&iCellIdx,sizeof(_uint),&dwByte,nullptr);
	}
	CloseHandle(hFile);
	MessageBox(NULL,L"Monster2Round Saved Successfully",L"Success",MB_OK);
}

void CMonster_Path::Save_ThirdRound()
{
	_ulong		dwByte = {0};
	_wstring strLast = TEXT(".dat");
	_wstring strPath{};
	switch(m_eTargetID)
	{
	case Client::LEVEL_GAMEPLAY:
	strPath = TEXT("../Bin/Data/Gameplay_Monster");
	break;
	case Client::LEVEL_YARD:
	strPath = TEXT("../Bin/Data/Yard_Monster");
	break;
	default:
	break;
	}
	_wstring Path_Result = strPath + to_wstring(3) + strLast;
	HANDLE		hFile = CreateFile(Path_Result.c_str(),GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if(0 == hFile)
		return;
	for(auto& pMonster : m_vecMonsterSpawn[m_eTargetID][3])
	{
		_float3 fPos = pMonster.fPos;
		_uint	iModelIdx = pMonster.iModel_Idx;
		_uint	iCellIdx = pMonster.iCellIdx;
		WriteFile(hFile,&iModelIdx,sizeof(_uint),&dwByte,nullptr);
		WriteFile(hFile,&fPos,sizeof(_float3),&dwByte,nullptr);
		WriteFile(hFile,&iCellIdx,sizeof(_uint),&dwByte,nullptr);
	}
	CloseHandle(hFile);
	MessageBox(NULL,L"Monster3Round Saved Successfully",L"Success",MB_OK);
}

void CMonster_Path::Load()
{}

void CMonster_Path::Add_Data()
{
	if(m_iRound != 0)
	{
		MONSTER_SPAWN_DESC pDesc{};
		pDesc.fPos = {Position[0],Position[1],Position[2]};
		pDesc.iModel_Idx = m_iModelIndex;
		pDesc.iCellIdx = m_iCellIndex;
		// 레벨과 라운드로 데이터 파일 이름을 구분할 것임
		pDesc.iLevel = m_iLevel;
		pDesc.iRound = m_iRound;
		// 0은 쉬는 시간 , 123 이 실제 라운드
		cout << "추가 : " << m_iModelIndex << ",    Cell 번호 :" << m_iCellIndex << "     레벨 : " << m_iLevel << "  라운드 : " << m_iRound << endl;
		m_vecMonsterSpawn[m_iLevel][m_iRound].push_back(pDesc);
	} else
		cout << "0라운드다 " << endl;
}

void CMonster_Path::Picking_Create()
{
	if((m_pGameInstance->Get_DIMouseState_Down(DIM_LB)))
	{
		_float3 fMousePos = m_pGameInstance->Get_MousePos_NDC(g_hWnd,g_iWinSizeX,g_iWinSizeY);
		XMMATRIX invProj = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_PROJ);
		XMMATRIX invView = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW);
		XMVECTOR RayPos,RayDir;

		m_pGameInstance->Get_MouseRayDirection(fMousePos,invProj,invView,&RayPos,&RayDir);

		RayDir = XMVector3Normalize(RayDir);
		_bool bPickCheck = false;
		for(auto& pCollisionBox : m_vecCollisionCenter)
		{
			XMFLOAT3 fBoxPos{},fMinPoint{},fMaxPoint{};
			fBoxPos = pCollisionBox->Get_PickingPos();
			m_pGameInstance->CreateBoundingBox(fBoxPos,{0.3f,0.3f,0.3f},fMinPoint,fMaxPoint);

			float distance;
			if(m_pGameInstance->Picking_Box(RayPos,RayDir,fMinPoint,fMaxPoint,distance,pCollisionBox->Get_BoundingBox()))
			{
				bPickCheck = true;
				m_fPickingPos = fBoxPos;
				m_iCellIndex = pCollisionBox->Get_CellIdx();
				Position[0] = m_fPickingPos.x;		Position[1] = m_fPickingPos.y;		Position[2] = m_fPickingPos.z;
				return;
			}
		}


		// 박스 충돌이 안되면 터레인 피킹한 위치로 반환
		const _float3* VtxPos = pVIBuffer_Terrain->Get_VtxPos();  // _float3 배열의 시작 주소 반환
		_uint VtxCountX = pVIBuffer_Terrain->Get_VtxCountX();
		_uint VtxCountZ = pVIBuffer_Terrain->Get_VtxCountZ();


		m_fPickingPos = m_pGameInstance->Picking_Terrain(RayPos,RayDir,VtxPos,VtxCountX,VtxCountZ);
		if(m_fPickingPos.x == 0 && m_fPickingPos.y == 0 && m_fPickingPos.z == 0)
			return;

		Position[0] = m_fPickingPos.x;		Position[1] = m_fPickingPos.y;		Position[2] = m_fPickingPos.z;

	}
}

void CMonster_Path::Load_Map()
{
	HANDLE hFile{};
	switch(m_eTargetID)
	{
	case Client::LEVEL_GAMEPLAY:
	{
		hFile = CreateFile(L"../Bin/Data/Environment.dat",GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(INVALID_HANDLE_VALUE == hFile)
		{
			MessageBox(NULL,L"Load Environment File Failed",L"Error",MB_OK);
			return;
		}
		break;
	}
	case Client::LEVEL_YARD:
	{
		hFile = CreateFile(L"../Bin/Data/Environment_Yard.dat",GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(INVALID_HANDLE_VALUE == hFile)
		{
			MessageBox(NULL,L"Load Environment_Yard File Failed",L"Error",MB_OK);
			return;
		}
		break;
	}
	default:
	break;
	}

	DWORD dwByte = 0;
	LEVELID iLevel;
	_uint iImGuiMode{};
	_int  iModelIndex{};
	_float3 fPos{},fCollisionBoxScale{},fScale{};
	_vector	vRight{},vUp{},vLook{},vecCollisionPos{};
	while(ReadFile(hFile,&iLevel,sizeof(LEVELID),&dwByte,nullptr) && dwByte > 0)
	{

		ReadFile(hFile,&iModelIndex,sizeof(_int),&dwByte,nullptr);
		ReadFile(hFile,&fPos,sizeof(_float3),&dwByte,nullptr);
		ReadFile(hFile,&fScale,sizeof(_float3),&dwByte,nullptr);

		ReadFile(hFile,&fCollisionBoxScale,sizeof(_float3),&dwByte,nullptr);
		ReadFile(hFile,&iImGuiMode,sizeof(_uint),&dwByte,nullptr);
		ReadFile(hFile,&vecCollisionPos,sizeof(_vector),&dwByte,nullptr);

		ReadFile(hFile,&vRight,sizeof(_vector),&dwByte,nullptr);
		ReadFile(hFile,&vUp,sizeof(_vector),&dwByte,nullptr);
		ReadFile(hFile,&vLook,sizeof(_vector),&dwByte,nullptr);

		CEnvironment::ENVIRONMENT_DESC			Desc{};
		Desc.eID = LEVEL_MONSTERSPAWN;
		Desc.fPosition = fPos;
		Desc.iModelComponentIndex = iModelIndex;
		Desc.fScale = fScale;
		//cout << fScale.x << "     " << fScale.y << "            " << fScale.z << endl;
		CGameObject* pGameObj = (m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_MONSTERSPAWN,TEXT("Layer_Environment"),
			TEXT("Prototype_GameObject_Environment_ImGui"),&Desc));
		if(pGameObj != nullptr)
		{
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Scale(0.f,fScale.x,fScale.y,fScale.z);
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Rotaion(vRight,vUp,vLook);
		}
	}
	CloseHandle(hFile);



	switch(m_eTargetID)
	{
	case Client::LEVEL_GAMEPLAY:
	{
		hFile = CreateFile(L"../Bin/Data/Build.dat",GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(INVALID_HANDLE_VALUE == hFile)
		{
			MessageBox(NULL,L"Load Build File Failed",L"Error",MB_OK);
			return;
		}
		break;
	}
	case Client::LEVEL_YARD:
	{
		hFile = CreateFile(L"../Bin/Data/Build_Yard.dat",GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(INVALID_HANDLE_VALUE == hFile)
		{
			MessageBox(NULL,L"Load Build_Yard File Failed",L"Error",MB_OK);
			return;
		}
		break;
	}
	default:
	break;
	}
	while(ReadFile(hFile,&iLevel,sizeof(LEVELID),&dwByte,nullptr) && dwByte > 0)
	{

		ReadFile(hFile,&iModelIndex,sizeof(_int),&dwByte,nullptr);
		ReadFile(hFile,&fPos,sizeof(_float3),&dwByte,nullptr);
		ReadFile(hFile,&fScale,sizeof(_float3),&dwByte,nullptr);

		ReadFile(hFile,&fCollisionBoxScale,sizeof(_float3),&dwByte,nullptr);
		ReadFile(hFile,&iImGuiMode,sizeof(_uint),&dwByte,nullptr);
		ReadFile(hFile,&vecCollisionPos,sizeof(_vector),&dwByte,nullptr);

		ReadFile(hFile,&vRight,sizeof(_vector),&dwByte,nullptr);
		ReadFile(hFile,&vUp,sizeof(_vector),&dwByte,nullptr);
		ReadFile(hFile,&vLook,sizeof(_vector),&dwByte,nullptr);

		CEnvironment::ENVIRONMENT_DESC			Desc{};
		Desc.eID = LEVEL_MONSTERSPAWN;
		Desc.fPosition = fPos;
		Desc.iModelComponentIndex = iModelIndex;
		Desc.fScale = fScale;
		//cout << fScale.x << "     " << fScale.y << "            " << fScale.z << endl;
		CGameObject* pGameObj = (m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_MONSTERSPAWN,TEXT("Layer_Environment"),
			TEXT("Prototype_GameObject_Environment_ImGui"),&Desc));
		if(pGameObj != nullptr)
		{
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Scale(0.f,fScale.x,fScale.y,fScale.z);
			dynamic_cast<CEnvironment*>(pGameObj)->Set_Rotaion(vRight,vUp,vLook);
		}
	}

	CloseHandle(hFile);
}

void CMonster_Path::Create_ImageButton()
{
	m_pSave = CTexture::Create(m_pDevice,m_pContext,TEXT("../Bin/Resources/Textures/Save.jpg"));
	m_pLoad = CTexture::Create(m_pDevice,m_pContext,TEXT("../Bin/Resources/Textures/Load.jpg"));
	m_pMonster = CTexture::Create(m_pDevice,m_pContext,TEXT("../Bin/Resources/Textures/ImGui/Button/Monster%d.png"),MONSTER_EA);
	// 사진의 리소스뷰 가져오기
	my_Savetexture = *m_pSave->Get_SRV().begin();
	my_Loadtexture = *m_pLoad->Get_SRV().begin();
}

void CMonster_Path::ButtonImage_List()
{
	ImGui::BeginChild("Choose Monster");
	int iButton = 0;
	auto& SRVs = m_pMonster->Get_SRV();
	for(auto iter = SRVs.begin(); iter != SRVs.end(); ++iter)
	{
		if(iButton % 4 != 0)
			ImGui::SameLine();
		string tag = "Monster" + to_string(iButton);
		if(ImGui::ImageButton(tag.c_str(),*iter,ImVec2(50,50),ImVec2(0,0)))
		{
			switch(iButton)
			{
			case 0:
			m_iModelIndex = ANIM_TANK;
			break;
			case 1:
			m_iModelIndex = ANIM_HELICOPTER;
			break;
			case 2:
			m_iModelIndex = ANIM_ALIEN;
			break;
			case 3:
			m_iModelIndex = ANIM_PONY;
			break;
			case 4:
			m_iModelIndex = ANIM_RIFLEMAN;
			break;
			case 5:
			m_iModelIndex = ANIM_BLIMP;
			break;
			default:
			break;
			}
		}
		iButton++;
	}
	ImGui::EndChild();
}

CMonster_Path* CMonster_Path::Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
{
	CMonster_Path* pInstance = new CMonster_Path(pDevice,pContext);

	if(FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMonster_Path");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Path::Free()
{
	__super::Free();
	Safe_Release(m_pSave);
	Safe_Release(m_pLoad);
	Safe_Release(m_pMonster);

}