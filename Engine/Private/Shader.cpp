#include "..\Public\Shader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CShader::CShader(const CShader& Prototype)
	: CComponent{ Prototype }
	, m_pEffect{ Prototype.m_pEffect }
	, m_InputLayouts{ Prototype.m_InputLayouts }
	, m_iNumPasses {Prototype.m_iNumPasses}
{

	Safe_AddRef(m_pEffect);

	for (auto& pInputLayout : m_InputLayouts)
		Safe_AddRef(pInputLayout);
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	_uint		iHlslFlag = { 0 };

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif 	

	// 1. 컴파일할 이펙트 파일의 경로를 가리킨다,
	// 2. 쉐이더 파일의 매크로 사용 여부. 
	// 3. D3D_COMPILE_STANDARD_FILE_INCLUDE 이 속성을 사용해야 나중에 헤더 포함 가능
	// 4. 쉐이더 컴파일 시에 사용할 플래그로 디버깅정보, 최적화 수준 설정 등등이 가능하다
	// 5. 디버그 모드일 경우 디버그 플래그 설정 
	// 6. 최적화 삭제를 위에서 지정해두었다.
	// 7. 컴파일된 이펙트 객체의 주소를 저장할 변수의 주소
	// 8. 컴파일 오류 메시지를 받을 수 있는 Blob 객체 포인터의 주소로 함수의 실행 결과 정보를 전달하는 메모리 공간
	if (FAILED(D3DX11CompileEffectFromFile(
			pShaderFilePath, 
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, 
			iHlslFlag, 
			0, 
			m_pDevice, 
			&m_pEffect,
			nullptr)))
		return E_FAIL;


	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC		TechniqueDesc{};

	pTechnique->GetDesc(&TechniqueDesc);

	m_iNumPasses = TechniqueDesc.Passes;

	for (size_t i = 0; i < m_iNumPasses; i++)
	{
		ID3D11InputLayout* pInputLayout = { nullptr };

		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);
		if (nullptr == pPass)
			return E_FAIL;

		D3DX11_PASS_DESC		PassDesc{};

		pPass->GetDesc(&PassDesc);

		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
			return E_FAIL;

		m_InputLayouts.push_back(pInputLayout);
	}

	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (iPassIndex >= m_iNumPasses)
		return E_FAIL;

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);
	// IASetInputLayout 는 내가 그리려고 하는 정점들을 내가 만든 쉐이더에서 잘 입력 받아올 수 있는 가에 대한 검증을 해주는 함수이다.
// CreateInputLayOut() 을 통해 레이아웃을 생성하고 호출하면 된다. initial_prototype에서 Create함

	ID3DX11EffectPass* pPass = m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPassIndex);
	if (nullptr == pPass)
		return E_FAIL;

	/* 쉐이더는 전역변수를 클라이언트로부터 받아올 수 있다. */
	/* Apply함수를 호출하기 전에 받아와야할 모든 값들을 받아와야하낟. */
	/* Apply함수는 쉐이더에 전달할 모든 변수를 다 던지고 호출해야한다. */
	pPass->Apply(0, m_pContext);

	return S_OK;
}

// 셰이더를 호출하기 전에 전역 변수를 세팅해주는 역할, 행렬을 바인딩하는 함수
HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	if (nullptr == m_pEffect)
		return E_FAIL;
	// 셰이더파일안에 정의되어 있는 지정한 이름의 전역변수에 대한 핸들을 얻어온다.
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;


	//AsMatrix :  Effect 파일의 변수를 행렬로 치환해준다.
	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	// 행렬데이터를 효과 변수에 설정해준다.
	return  pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(pMatrix));
}


HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices)
{
	if (nullptr == m_pEffect)
		return E_FAIL;


	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrixArray(reinterpret_cast<const _float*>(pMatrices), 0, iNumMatrices);
}



// 셰이더를 호출하기 전에 전역 변수를 세팅해주는 역할, SRC ( 텍스처 ) 를 바인딩하는 함수
HRESULT CShader::Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{

	if (nullptr == m_pEffect)
		return E_FAIL;
	// 셰이더 파일 안에 정의되어 있는 지정한 이름의 전역변수에 대한 핸들을 얻어온다
	ID3DX11EffectVariable* pVariable= m_pEffect->GetVariableByName(pConstantName);

	if (nullptr == pVariable)
		return E_FAIL;

	// AsShaderResource : 효과 파일으 ㅣ변수를 셰이더 리소스로 캐스팅하기 위해 사용되는 메서드
	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	// 셰이더 리소스 변수를 설정,  셰이더 리소스 뷰를 셰이더에 바인딩한다.
	return pSRVariable->SetResource(pSRV);

}

HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iLength);
}



CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElements, iNumElements)))
	{
		MSG_BOX("Failed to Created : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);

	for (auto& pInputLayout : m_InputLayouts)
		Safe_Release(pInputLayout);
}
