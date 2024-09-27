#pragma once

// 정점 버퍼와 인덱스 버퍼를 가지는 모든 클래스들의 부모 클래스
// 추상 클래스라 Create 함수는 따로 없다.
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& Prototype);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();


	void	Chang_Topology();
public:
	HRESULT Bind_Buffers(); // 그리기 위해 필요한 값들을 장치에 올린다.
protected:
	ID3D11Buffer*					m_pVB = { nullptr };		// 정점을 저장하는 버퍼, 정점 데이터를 GPU메모리에 저장, 엑세스 할 수 있게 해준다
	ID3D11Buffer*					m_pIB = { nullptr };		// 인덱스를 저장하는 버퍼, 인덱스는 정점 버퍼 내의 정점들을 참조하여 효율적으로 렌더링하게 한다.

	D3D11_BUFFER_DESC				m_BufferDesc = {};			// 버퍼 생성시 사용하는 구조체, ( 버퍼크기, 사용용도(정점버퍼, 인덱스버퍼 등), 메모리 접근법(CPU,GPU) 등을 정의
	D3D11_SUBRESOURCE_DATA			m_InitialDesc = {};			// 버퍼 초기 데이터 넘기기 위해 사용하는 구조체, GPU 메모리에 처음으로 데이터 복사 가능


	_uint							m_iNumVertexBuffers = {};	// 사용되는 정점 버퍼의 개수
	_uint							m_iVertexStride = {};		// 정점 하나의 크기를 나타낸다.한 칸의 크기를 Stride 라고 부른다.
	_uint							m_iNumVertices = {};		// 정점 버퍼 하나에 포함된 정점의 총 개수
	_uint							m_iIndexStride = {};		// 인덱스 하나의 크기, 일반적으로 2byte 혹은 4byte의 크기를 갖는다.
	_uint							m_iNumIndices = {};			// 인덱스 버퍼에 포함된 인덱스의 총 개수

	DXGI_FORMAT						m_eIndexFormat = {};		// 인덱스 버퍼에서 사용하는 데이터 형식
	D3D_PRIMITIVE_TOPOLOGY			m_ePrimitiveTopology = {};	// 정점들이 렌더링 할 기본 도형의 종류를 정의


public:
	const _float3* Get_VtxPos() const { return m_fVertexPos; }
	_uint	Get_VtxCountX() { return m_iNumVerticesX; }
	_uint	Get_VtxCountZ() { return m_iNumVerticesZ; }
	_uint					m_iNumVerticesX = {};
	_uint					m_iNumVerticesZ = {};
	_float3*				m_fVertexPos = { nullptr };


protected:
	HRESULT Create_Buffer(ID3D11Buffer** ppOut);

public:
	virtual CComponent* Clone(void* pArg) = 0; // 순수 가상 함수 , 추상 클래스가 되어버렷~!!!!!!!
	virtual void Free() override;



};

END