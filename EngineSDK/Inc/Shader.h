#pragma once

#include "Component.h"


// 셰이더를 객체화하여 컴포넌트로 만들기 위한 클래스
// 외부 셰이더 파일을 받아와 객체화 한다는 느낌
// 셰이더를 빌드하는 기능이 D3D11 에 없다. 헤더와 라이브러리를 추가해야한다.
BEGIN(Engine)

class ENGINE_DLL CShader final: public CComponent
{
private:
	CShader(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CShader(const CShader& Prototype);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath,const D3D11_INPUT_ELEMENT_DESC* pElements,_uint iNumElements);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Begin(_uint iPassIndex);
	HRESULT Bind_Matrix(const _char* pConstantName,const _float4x4* pMatrix); // 매개변수로 받은 이름을 가진 매트릭스에 대해서
	HRESULT Bind_SRV(const _char* pConstantName,ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_SRVs(const _char* pConstantName,ID3D11ShaderResourceView** ppSRVs,_uint iNumSRVs);
	HRESULT Bind_RawValue(const _char* pConstantName,const void* pData,_uint iLength);
	HRESULT Bind_Matrices(const _char* pConstantName,const _float4x4* pMatrix,_uint iNumMatrices);




private:
	ID3DX11Effect* m_pEffect = {nullptr}; // 쉐이더를 관리하는 인터페이스
	vector<ID3D11InputLayout*>		m_InputLayouts; // GPU로 전달되는 정점 데이터의 형식과 구조를 설명하여 쉐이더의 입력과 정점 버퍼 데이터를 연결하는 역할
	// 셰이더에 pass 가 여러 개 있다. 따라서 Input 구조체도 pass마다 바뀌게 될 수 있다.
	// 셰이더와 정점 데이터를 연결하는 역할을 하는 InputLayOut도 여러 개가 존재해야 한다.
	// 따라서 D3D11InputLayOut* 를 벡터 컨테이너에 담아서 관리한다.


	_uint							m_iNumPasses = {};
public:
	static CShader* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext,const _tchar* pShaderFilePath,const D3D11_INPUT_ELEMENT_DESC* pElements,_uint iNumElements);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END