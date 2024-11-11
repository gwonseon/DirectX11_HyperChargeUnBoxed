#pragma once
#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Grass final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_Grass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Grass(const CVIBuffer_Grass& Prototype);
	virtual ~CVIBuffer_Grass() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring pDataFilePath, _fmatrix PreTransformMatrix, _uint iIndex,  vector<_float3> fPos, const CVIBuffer_Instancing::INSTANCING_DESC* pDesc);
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Bind_ShaderResource(CShader* pShader, _uint iMeshIndex, aiTextureType eMaterialType, _uint iIndex, const _char* pConstantName);


private:
	_float4x4						m_PreTransformMatrix = {};
	_uint			m_iNumMeshes = { 0 };
	_uint			m_iNumMaterials = { 0 };
	_uint			m_iMaterialIndex = { 0 };
	_uint			m_iFaceNum = 0;
	DWORD			dwByte = 0;

	vector<ID3D11ShaderResourceView*>		m_Materials[aiTextureType_UNKNOWN];



public:
	static CVIBuffer_Grass* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring pDataFilePath, _fmatrix PreTransformMatrix, _uint iIndex,vector<_float3> fPos , const CVIBuffer_Instancing::INSTANCING_DESC* pDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END