#pragma once

#include "Base.h"

BEGIN(Engine)



class CMeshMaterial final : public CBase
{
private:
	CMeshMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMeshMaterial() = default;

public:
//	HRESULT Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial);
	HRESULT Bind_ShaderResource(class CShader* pShader, aiTextureType eTextureType, _uint iIndex, const _char* pConstantName);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	vector<ID3D11ShaderResourceView*>		m_Materials[aiTextureType_UNKNOWN];




public:
	HRESULT Initialize_ReadData(HANDLE hFileRead);
	static CMeshMaterial* Create_ReadData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,  HANDLE hFileRead);

private:
	DWORD			dwByte = 0;





public:
//	static CMeshMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial);
	virtual void Free() override;
};

END