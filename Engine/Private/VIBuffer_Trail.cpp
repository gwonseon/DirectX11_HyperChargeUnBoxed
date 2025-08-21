#include "..\Public\VIBuffer_Trail.h"


CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice,pContext)
	,m_MaxSegments(0)
	,m_FadeSpeed(1.0f)
{}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& Prototype)
	: CVIBuffer(Prototype)
	,m_MaxSegments(Prototype.m_MaxSegments)
	,m_FadeSpeed(Prototype.m_FadeSpeed)
	,m_Buffer(Prototype.m_Buffer)
{}



HRESULT CVIBuffer_Trail::Initialize_Prototype()
{
	// 버퍼 설정
	m_iVertexStride      = sizeof(TRAILVERTEX_DESC);
	m_iNumVertices       = m_MaxSegments ;
	m_iNumVertexBuffers  = 1;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	// 동적 버퍼 생성
	ZeroMemory(&m_BufferDesc,sizeof m_BufferDesc);
	m_BufferDesc.Usage        = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.BindFlags    = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.ByteWidth    = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.MiscFlags      = 0;
	m_BufferDesc.StructureByteStride = 0;
	ZeroMemory(&m_InitialDesc,sizeof m_InitialDesc);
	// 초기 데이터는 nullptr (Map 시 전체 덮어쓰기)
	m_InitialDesc.pSysMem = nullptr;

	//if(FAILED(Create_Buffer(&m_pVB)))
	//	return E_FAIL;

	HRESULT hr = Create_Buffer_Dynamic(&m_pVB);
	if(FAILED(hr)) {
		cout << ("Trail VB CreateBuffer failed: 0x%08X",hr);
	}
	// 순환 버퍼 크기만큼 벡터 준비
	m_Buffer.assign(m_MaxSegments,{_float3{0,0,0},1.0f});
	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void* pArg)
{
	return S_OK;
}

void CVIBuffer_Trail::Update_TrailPos(const _float3& tipPos,_float fDeltaTime)
{
	// 순환 버퍼 이동 & age 증가
	for(int i = m_MaxSegments - 1; i > 0; --i) {
		m_Buffer[i]     = m_Buffer[i - 1];
		m_Buffer[i].fAlpha = min(1.0f,m_Buffer[i].fAlpha + fDeltaTime * m_FadeSpeed);
	}
	// 최신 위치 삽입
	m_Buffer[0].vPosition = tipPos;
	m_Buffer[0].fAlpha      = 0.0f;

	// GPU에 복사 (Map/Unmap)
	D3D11_MAPPED_SUBRESOURCE mapped;
	m_pContext->Map(m_pVB,0,D3D11_MAP_WRITE_DISCARD,0,&mapped);
	memcpy(mapped.pData,m_Buffer.data(),m_iVertexStride * m_iNumVertices);
	m_pContext->Unmap(m_pVB,0);
}

HRESULT CVIBuffer_Trail::Render()
{
	// Bind Buffers
	Bind_Buffers();

	// Draw non-indexed
	m_pContext->Draw(m_iNumVertices,0);
	return S_OK;
}

CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext,
	_uint maxSegments,float fadeSpeed )
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice,pContext);
	pInstance->m_MaxSegments = maxSegments;
	pInstance->m_FadeSpeed = fadeSpeed;
	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CVIBuffer_Trail::Clone(void* pArg)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);
	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Trail::Free()
{

	__super::Free();
}
