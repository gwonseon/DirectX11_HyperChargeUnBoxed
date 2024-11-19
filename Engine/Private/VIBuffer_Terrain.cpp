#include "..\Public\VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }

{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype)
	: CVIBuffer{ Prototype }

{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapFilePath)
{
	_ulong			dwByte = { 0 };

	// CreateFile : 파일을 열거나 새로 만든다. pHeightMapFilePath의 경로에 있는 파일을 GENERIC_READ 읽기 전용으로 연다.
	// OPEN_EXISTING : 이미 존재하는 파일을 연다	if (0 == hFile)
	// 높이맵 파일 경로 받아온다
	HANDLE			hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile)
		return E_FAIL;


	// BMP 파일 헤더와 정보 헤더 읽기
	// 파일 타입, 크기, 데이터 시작 위치를 가지고 있다
	BITMAPFILEHEADER			fh{};
	// 이미지의 너비, 높이, 비트 깊이 등 BMP 이미지의 상세 정보를 가지고 있다.
	BITMAPINFOHEADER			ih{};

	// ReadFile : 파일로부터 데이터를 읽는다.
	ReadFile(hFile, &fh, sizeof(fh), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(ih), &dwByte, nullptr);

	// 이미지의 너비와 높이를 곱해 픽셀의 총 수를 계산한다.
	// 필요한 메모리를 동적으로 할당한다.
	_uint* pPixel = new _uint[ih.biWidth * ih.biHeight];
	m_iSizePixel = sizeof(_uint) * ih.biWidth * ih.biHeight;
	// ReadFile 함수를 이용해 파일에서 픽셀 데이터를 읽어서 앞에서 할당한 메모리를 위의 배열에 저장한다.
	ReadFile(hFile, pPixel, m_iSizePixel, &dwByte, nullptr);

	// CreateFile을 통해 생성된 핸들을 닫는다.
	// 더이상 파일에 대한 읽기 쓰기가 불가능
	// 닫지 않으면 시스템 리소스 누수가 발생할 수 있다. 이경우 프로그램이 더 많은 메모리와 리소스를 사용하게 되어 성능 저하나 시스템 불안정성이 발생할 수 있다.
	CloseHandle(hFile);

	m_iNumVerticesX = ih.biWidth * 10;// 버텍스 가로 개수 = 이미지의 가로 픽셀 수
	m_iNumVerticesZ = ih.biHeight * 10;	// 버텍스 세로 개수 = 이미지 세로 픽셀 수
	m_iVertexStride = sizeof(VTXNORTEX);// 사이즈는 구조체 사이즈
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;// 버텍스 개수는 가로 개수 X 세로 개수
	m_iIndexStride = sizeof(_uint);	// 인덱스 사이즈 = 4바이트 ( int 사이즈 )
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;// 인덱스 개수  = (버텍스 가로개수 - 1) X (버텍스 세로 개수 -1) X 2 X 3
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	// m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST; // 그리드
	m_fVertexPos = new _float3[m_iNumVertices];
#pragma region VERTEX_BUFFER
	// 정점 배열 메모리 동적 할당
	// VTXNORTEX 구조체가 포함하고 있는 데이터를 이용함	
	

	//size_t modify_i = 10; // 수정하려는 세로 위치
	//size_t modify_j = 15; // 수정하려는 가로 위치
	//size_t modify_index = modify_i * m_iNumVerticesX + modify_j;
	// pPixel[modify_index] = (pPixel[modify_index] & 0xffffff00) | (3000 & 0xff); // 새로운 높이를 20으로 설정

	// 2D 정점 좌표 설정 
	// i : 높이 j : 너비

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint			iIndex = i * m_iNumVerticesX + j;
			// 높이값 계산을 위해 0x000000ff 16진수 ARGB 에서 B 값을 받아온다.
			// 정점의 위치를 설정한다. 15로 나누는 것은 높이 스케일 조정하는것
		
			pVertices[iIndex].vPosition = _float3(j,0.f, i); // 15 나눈 값으로 높이 스케일 조정
		
			pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f); // 정점의 법선 벡터 초기화
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f)); // 텍스처 좌표 설정
			m_fVertexPos[iIndex] = pVertices[iIndex].vPosition;

		}
	}
#pragma endregion

#pragma region INDEX_BUFFER
	// 인덱스 배열 메모리 동적 할당 및 초기화
	_uint* pIndices = new _uint[m_iNumIndices];
	_uint			iNumIndices = { 0 };
	
	// 삼각형 리스트 구성 및 법선 벡터 계산
	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint			iIndex = i * m_iNumVerticesX + j;

			_uint			iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			// 첫 번째 삼각형에 대한 법선 벡터의 계산
			// vSour, vDest는 삼각형의 두 변을 나타낸다.
			// XMVector3Cross : 두 벡터의 외적을 이용해 법선 벡터(vNormal) 을 구한다.
			// XMVector3Normalize : 법선 벡터를 정규화하여 크기 1로 만든다.
			_vector			vSour, vDest, vNormal;

			vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			// 계산된 법선 벡터를 삼각형의 세 정점에 더해준다.
			// 여러 삼각형이 하나의 정점을 공유하는 경우 법선 벡터를 더해 평균적인 법선 벡터가 구해진다.
			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);

			// 두 번째 삼각형에 대한 계산
			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
		}
	}
	// 법선 벡터의 최종 정규화
	// 법선 벡터의 크기를 1로 맞춰준다.
	for (size_t i = 0; i < m_iNumVertices; i++)
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

#pragma endregion
	/* dx9 : 정점버퍼를 할당하고 -> 락언락해서 정점버퍼에 초기값을 채운다. */
	/* dx9 : 정점버퍼에 초기값을 채우면서 정점버퍼를 할당한다*/
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);// 0초기화

	/* 할당하고자하는 메모리공간의 크기(Byte)*/
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices; // 버텍스 사이즈 X 버텍스 개수

	/* 버퍼의 속성 (정적, 동적) */
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; // D3D11_USAGE_DEFAULT : GPU에서 읽기와 쓰기를 한다. 이를 통해 성능과 메모리 효율성을 최적화한다.
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;// 정점 버퍼로 사용될 것임을 지정한다. GPU가 이 버퍼를 정점 데이터를 읽는데 사용한다.
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;// CPU가 이 버퍼에 직접 접근할 수 없다.
	m_BufferDesc.MiscFlags = 0;// 특별한 추가 속성이 없다.
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);// m_InitialDesc 초기화한다
	m_InitialDesc.pSysMem = pVertices;// pVertices에 저장된 정점 데이터의 포인터를 할당하여 GPU가 이 데이터로 정점 버퍼를 초기화하도록 한다.

	if (FAILED(__super::Create_Buffer(&m_pVB)))// GPU에 실제로 버퍼를 생성한다
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);// m_BufferDesc 초기화

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;// 인덱스 버퍼의 크기를 바이트 단위로 지정한다. 인덱스 크기 X 인덱스 개수
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;// GPU를 기본적인 사용 방식으로 동작하게 한다.
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;// GPU가 이 버퍼를 인덱스 데이터를 읽는데 사용하도록 한다.
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;// CPU 접근 놉
	m_BufferDesc.MiscFlags = 0;// 추가 속성 놉
	m_BufferDesc.StructureByteStride = 0;// 인덱스는 구조체스트라이드 필요없음

	ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);// m_InitialDesc 초기화
	m_InitialDesc.pSysMem = pIndices;// pSysMem  에 인덱스 데이터를 가리키는 포인터 할당


	if (FAILED(__super::Create_Buffer(&m_pIB)))// GPU에 인덱스버퍼 생성
		return E_FAIL;

	// 할당된 메모리 해제
	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);
	Safe_Delete_Array(pPixel);

	//XMMatrixLookAtLH();
	//XMMatrixPerspectiveFovLH();

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

_bool CVIBuffer_Terrain::Terrain_Picking(_float3 fRayDir, _float3 CameraPos, _float3& ResultPos)
{
	return false;
}


CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CVIBuffer_Terrain::Free()
{
	__super::Free();
	

}
