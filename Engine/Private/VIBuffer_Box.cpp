
#include "..\Public\VIBuffer_Box.h"

CVIBuffer_Box::CVIBuffer_Box(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
	: CVIBuffer{pDevice,pContext}
{}

CVIBuffer_Box::CVIBuffer_Box(const CVIBuffer_Box& Prototype)
	: CVIBuffer{Prototype}
{}

HRESULT CVIBuffer_Box::Initialize_Prototype()
{
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumVertices = 8;  // 정점 수 8개
	m_iIndexStride = sizeof(_ushort);
	m_iNumIndices = 36;  // 인덱스 수 36개
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	m_fVertexPos = new _float3[m_iNumVertices];

	ZeroMemory(&m_BufferDesc,sizeof m_BufferDesc);

	/* 할당하고자하는 메모리공간의 크기(Byte)*/
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;

	/* 버퍼의 속성 (정적, 동적) */
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialDesc,sizeof m_InitialDesc);
	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];

	pVertices[0].vPosition = {-1.f,1.f,-1.f};
	pVertices[0].vTexcoord = _float2(0.0f,0.0f);
	m_fVertexPos[0] = pVertices[0].vPosition;

	pVertices[1].vPosition = {1.f,1.f,-1.f};
	pVertices[1].vTexcoord = _float2(1.0f,0.0f);
	m_fVertexPos[1] = pVertices[0].vPosition;


	pVertices[2].vPosition = {1.f,-1.f,-1.f};
	pVertices[2].vTexcoord = _float2(1.0f,1.0f);
	m_fVertexPos[2] = pVertices[0].vPosition;

	pVertices[3].vPosition = {-1.f,-1.f,-1.f};
	pVertices[3].vTexcoord = _float2(0.0f,1.0f);
	m_fVertexPos[3] = pVertices[0].vPosition;

	pVertices[4].vPosition = {-1.f,1.f,1.f};
	pVertices[4].vTexcoord = _float2(0.0f,0.0f);
	m_fVertexPos[4] = pVertices[0].vPosition;

	pVertices[5].vPosition = {1.f,1.f,1.f};
	pVertices[5].vTexcoord = _float2(1.0f,0.0f);
	m_fVertexPos[5] = pVertices[0].vPosition;

	pVertices[6].vPosition = {1.f,-1.f,1.f};
	pVertices[6].vTexcoord = _float2(1.0f,1.0f);
	m_fVertexPos[6] = pVertices[0].vPosition;

	pVertices[7].vPosition = {-1.f,-1.f,1.f};
	pVertices[7].vTexcoord = _float2(0.0f,1.0f);
	m_fVertexPos[7] = pVertices[0].vPosition;

	m_InitialDesc.pSysMem = pVertices;

	if(FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;




	ZeroMemory(&m_BufferDesc,sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_InitialDesc,sizeof m_InitialDesc);
	_ushort* pIndices = new _ushort[m_iNumIndices];
	pIndices[0]= 1;
	pIndices[1]= 5;
	pIndices[2]= 6;

	// X+
	pIndices[3]= 1;
	pIndices[4]= 6;
	pIndices[5]= 2;

	// X-
	pIndices[6]= 4;
	pIndices[7]= 0;
	pIndices[8]= 3;

	// X-
	pIndices[9]= 4;
	pIndices[10]= 3;
	pIndices[11]= 7;

	// Y+
	pIndices[12]= 4;
	pIndices[13]= 5;
	pIndices[14]= 1;

	// Y+
	pIndices[15]= 4;
	pIndices[16]= 1;
	pIndices[17]= 0;

	// Y-
	pIndices[18]= 3;
	pIndices[19]= 2;
	pIndices[20]= 6;

	// Y-
	pIndices[21]= 3;
	pIndices[22]= 6;
	pIndices[23]= 7;

	// Z+
	pIndices[24]= 7;
	pIndices[25]= 6;
	pIndices[26]= 5;

	// Z+
	pIndices[27]= 7;
	pIndices[28]= 5;
	pIndices[29]= 4;

	// Z-
	pIndices[30] = 0;
	pIndices[31] = 1;
	pIndices[32] = 2;

	// Z-
	pIndices[33] = 0;
	pIndices[34] = 2;
	pIndices[35] = 3;
	m_InitialDesc.pSysMem = pIndices;

	if(FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;


	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Box::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Box* CVIBuffer_Box::Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
{
	CVIBuffer_Box* pInstance = new CVIBuffer_Box(pDevice,pContext);

	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Box");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Box::Clone(void* pArg)
{
	CVIBuffer_Box* pInstance = new CVIBuffer_Box(*this);

	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Box");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Box::Free()
{
	__super::Free();
}