#include "..\Public\MeshMaterial.h"
#include "Shader.h"

CMeshMaterial::CMeshMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

//HRESULT CMeshMaterial::Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial)
//{
//	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
//	{
//		_uint		iNumTexture = pAIMaterial->GetTextureCount(aiTextureType(i));
//
//		for (size_t j = 0; j < iNumTexture; j++)
//		{
//			ID3D11ShaderResourceView* pSRV = { nullptr };
//			aiString						strTextureFilePath = {};
//
//
//			if (FAILED(pAIMaterial->GetTexture(aiTextureType(i), j, &strTextureFilePath)))
//				return E_FAIL;
//
//			_char			szFullPath[MAX_PATH] = {};
//			_char			szDrive[MAX_PATH] = {};
//			_char			szDirectory[MAX_PATH] = {};
//			_char			szFileName[MAX_PATH] = {};
//			_char			szExt[MAX_PATH] = {};
//
//			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
//			_splitpath_s(strTextureFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
//
//			strcpy_s(szFullPath, szDrive);
//			strcat_s(szFullPath, szDirectory);
//			strcat_s(szFullPath, szFileName);
//			strcat_s(szFullPath, szExt);
//
//			_tchar		szPerfectPath[MAX_PATH] = {};
//			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szPerfectPath, MAX_PATH);
//
//
//			if (false == strcmp(szExt, ".dds"))
//			{
//				if (FAILED(CreateDDSTextureFromFile(m_pDevice, szPerfectPath, nullptr, &pSRV)))
//					return E_FAIL;
//			}
//			else
//			{
//				if (FAILED(CreateWICTextureFromFile(m_pDevice, szPerfectPath, nullptr, &pSRV)))
//					return E_FAIL;
//			}
//
//			m_Materials[i].push_back(pSRV);
//		}
//	}
//
//	return S_OK;
//}

HRESULT CMeshMaterial::Bind_ShaderResource(CShader* pShader, aiTextureType eTextureType, _uint iIndex, const _char* pConstantName)
{
	if (iIndex >= m_Materials[eTextureType].size())
		return E_FAIL;
	return pShader->Bind_SRV(pConstantName, m_Materials[eTextureType][iIndex]);
}

HRESULT CMeshMaterial::Initialize_ReadData(HANDLE hFileRead)
{
	// 이거 18로 수정
	for (size_t i = 0; i < aiTextureType_UNKNOWN; i++)
	{
		_uint		iNumTexture = 0;
		ReadFile(hFileRead, &iNumTexture, sizeof(_uint), &dwByte, nullptr);
		for (_uint j = 0; j < iNumTexture; j++)
		{
			ID3D11ShaderResourceView* pSRV = { nullptr };
			_char			szFullPath[MAX_PATH] = {};
			_char			szExt[MAX_PATH] = {};
			_uint			iExtLen{}, iFullPathLen{};

			ReadFile(hFileRead, &iExtLen, sizeof(_uint), &dwByte, nullptr);
			for (_uint k = 0; k < iExtLen; k++)
			{
				ReadFile(hFileRead, &szExt[k], sizeof(_char), &dwByte, nullptr);
			}
			ReadFile(hFileRead, &iFullPathLen, sizeof(_uint), &dwByte, nullptr);
			for (_uint k = 0; k < iFullPathLen; k++)
			{
				ReadFile(hFileRead, &szFullPath[k], sizeof(_char), &dwByte, nullptr);
			}
	

			_tchar		szPerfectPath[MAX_PATH] = {};
			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szPerfectPath, MAX_PATH);


			if (false == strcmp(szExt, ".dds"))
			{
				if (FAILED(CreateDDSTextureFromFile(m_pDevice, szPerfectPath, nullptr, &pSRV)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(CreateWICTextureFromFile(m_pDevice, szPerfectPath, nullptr, &pSRV)))
					return E_FAIL;
			}

			m_Materials[i].push_back(pSRV);
			m_Materials[i].size();
		}
	}

	return S_OK;
}

CMeshMaterial* CMeshMaterial::Create_ReadData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,  HANDLE hFileRead)
{
	CMeshMaterial* pInstance = new CMeshMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize_ReadData( hFileRead)))
	{
		MSG_BOX("Failed to Created : CMeshMaterial");
		Safe_Release(pInstance);
	}

	return pInstance;
}

//CMeshMaterial* CMeshMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial)
//{
//	CMeshMaterial* pInstance = new CMeshMaterial(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize(pModelFilePath, pAIMaterial)))
//	{
//		MSG_BOX("Failed to Created : CMeshMaterial");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}

void CMeshMaterial::Free()
{
	__super::Free();

	for (auto& Textures : m_Materials)
	{
		for (auto& pSRV : Textures)
			Safe_Release(pSRV);
		Textures.clear();
	}

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);


}
