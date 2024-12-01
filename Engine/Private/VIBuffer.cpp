#include "..\Public\VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
	, m_fVertexPos()
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& Prototype)
	: CComponent{ Prototype }
	, m_pVB{ Prototype.m_pVB }
	, m_pIB{ Prototype.m_pIB }
	, m_iNumVertexBuffers{ Prototype.m_iNumVertexBuffers }
	, m_iVertexStride{ Prototype.m_iVertexStride }
	, m_iNumVertices{ Prototype.m_iNumVertices }
	, m_iIndexStride{ Prototype.m_iIndexStride }
	, m_iNumIndices{ Prototype.m_iNumIndices }
	, m_eIndexFormat{ Prototype.m_eIndexFormat }
	, m_ePrimitiveTopology{ Prototype.m_ePrimitiveTopology }
	, m_iNumVerticesX{ Prototype.m_iNumVerticesX }
	, m_iNumVerticesZ{ Prototype.m_iNumVerticesZ }
	, m_fVertexPos{ Prototype.m_fVertexPos }
	, m_pVertexPositions{ Prototype.m_pVertexPositions }
{
	Safe_AddRef(m_pIB);
	Safe_AddRef(m_pVB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	return S_OK;
}

void CVIBuffer::Update(_float fTimeDelta)
{
}

HRESULT CVIBuffer::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

void CVIBuffer::Chang_Topology()
{
	
		if (m_ePrimitiveTopology != D3D_PRIMITIVE_TOPOLOGY_LINELIST)
			m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		else
			m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
}

HRESULT CVIBuffer::Bind_Buffers()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
	};

	_uint				iVertexStrides[] = {
		m_iVertexStride,
	};

	_uint				iOffsets[] = {
		0,
	};


	/* 정점버퍼들을 장치에 바인딩한다. */
	/* 복수의 정점버퍼를 동시에 장치에 바인딩하는 것이 가능하다 .*/
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);

	/* 인덱스 버퍼를 장치에 바인딩한다. */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

//HRESULT CVIBuffer::Bind_ShaderResouce(CShader* pShader, _uint iMeshIndex, aiTextureType eMaterialType, _uint iIndex, const _char* pConstantName)
//{
//	return E_NOTIMPL;
//}

HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer** ppOut)
{
	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_InitialDesc, ppOut);

	return S_OK;
}


void CVIBuffer::Free()
{
	__super::Free();
	if (false == m_isCloned)
		Safe_Delete_Array(m_pVertexPositions);

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);
}
