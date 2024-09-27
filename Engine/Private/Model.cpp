#include "..\Public\Model.h"
#include "Bone.h"
#include "Mesh.h"
#include "Shader.h"
#include "Animation.h"
#include "MeshMaterial.h"


CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CModel::CModel(const CModel& Prototype)
	: CComponent{ Prototype }
	, m_eModelType{ Prototype.m_eModelType }
	, m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
	, m_iNumMeshes{ Prototype.m_iNumMeshes }
	, m_Meshes{ Prototype.m_Meshes }
	, m_iNumMaterials{ Prototype.m_iNumMaterials }
	, m_Materials{ Prototype.m_Materials }
	, m_iNumAnimations{ Prototype.m_iNumAnimations }
	, m_Animations{ Prototype.m_Animations }
{
	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);

	for (auto& pPrototypeBone : Prototype.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());

	for (auto& pMaterials : m_Materials)
		Safe_AddRef(pMaterials);

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);
}

_uint CModel::Get_BoneIndex(const _char* pBoneName) const
{
	_uint	iBoneIndex = { 0 };
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](class CBone* pBone)->_bool
		{
			if (!strcmp(pBone->Get_Name(), pBoneName))
				return true;

			++iBoneIndex;

			return false;
		});

	return iBoneIndex;
}

//HRESULT CModel::Initialize_Prototype_For_Export(TYPE eModelType, const _char* pModelFilePath, _uint iIndex, _fmatrix PreTransformMatrix)
//{
//	m_eModelType = eModelType;
//
//	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
//
//	_uint		iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };
//
//	if (TYPE_NONANIM == m_eModelType)
//		iFlag |= aiProcess_PreTransformVertices;
//
//	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
//
//	if (nullptr == m_pAIScene)
//		return E_FAIL;
//
//	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
//		return E_FAIL;
//
//	if (FAILED(Ready_Meshes()))
//		return E_FAIL;
//
//	if (FAILED(Ready_Materials(pModelFilePath)))
//		return E_FAIL;
//
//	return S_OK;
//
//}

HRESULT CModel::Initialize_Prototype_ReadDataFile(TYPE eModelType, const wstring pDataFile, _uint iIndex, _fmatrix PreTransformMatrix)
{
	m_eModelType = eModelType;

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);


	HANDLE hFileRead = CreateFile(pDataFile.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFileRead)
	{
		MessageBox(NULL, L" ModelData Exporter Failed", L"Error", MB_OK);
		return E_FAIL;
	}

//	Read_DataFile(pDataFile,hFileRead);
	if (FAILED(Ready_Meshes_ReadData_NonAnim(hFileRead)))
		return E_FAIL;

	if (FAILED(Ready_Materials_ReadData_NonAnim(hFileRead)))
		return E_FAIL;

	m_Materials;
	m_Meshes;

	CloseHandle(hFileRead);
	return S_OK;
}

HRESULT CModel::Read_DataFile(wstring hDataFile, HANDLE hFileRead)
{
	// 변수
	_float3			fVerticesPos{}, fVerticesNor{}, fVerticesTangent{};
	_float2			fVerticesTex{};
	_uint			iVerticesNum{}, iFaceNum{}, iMaterialIndex{}, iIndiciesNum{}, iMeshSize{}, m_iNumMesh_Read{}, iLen{};
	string			strMeshName;
	vector<_uint>	vecIndices;

	_char			szMeshName{};

	
	

//	ReadFile(hFileRead, &m_iNumMesh_Read, sizeof(_uint), &dwByte, nullptr);

	for (int j = 0; j < m_iNumMesh_Read; j++)
	{

//		ReadFile(hFileRead, &iLen, sizeof(_uint), &dwByte, nullptr);
		for (_uint k = 0; k < iLen; k++)
		{
//			ReadFile(hFileRead, &szMeshName, sizeof(_char), &dwByte, nullptr);
			//				cout << szMeshName;
		}
		//			cout << endl;

//		ReadFile(hFileRead, &iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		//		cout << " iMaterialIndex : " << iMaterialIndex << endl;
//		ReadFile(hFileRead, &iVerticesNum, sizeof(_uint), &dwByte, nullptr);
		//		cout << " iVerticesNum : " << iVerticesNum << endl;
//		ReadFile(hFileRead, &iFaceNum, sizeof(_uint), &dwByte, nullptr);
		//		cout << " iFaceNum : " << iFaceNum << endl;
//		ReadFile(hFileRead, &iMeshSize, sizeof(_uint), &dwByte, nullptr);
		//		cout << " iMeshSize : " << iMeshSize << endl;
		for (int i = 0; i < iMeshSize; i++)
		{
//			ReadFile(hFileRead, &fVerticesPos, sizeof(_float3), &dwByte, nullptr);
//			ReadFile(hFileRead, &fVerticesNor, sizeof(_float3), &dwByte, nullptr);
//			ReadFile(hFileRead, &fVerticesTex, sizeof(_float2), &dwByte, nullptr);
//			ReadFile(hFileRead, &fVerticesTangent, sizeof(_float3), &dwByte, nullptr);

		}
		_uint vecSize = vecIndices.size();
//		ReadFile(hFileRead, &vecSize, sizeof(_uint), &dwByte, nullptr);
		//			cout << "vecSize : " << vecSize << endl;

		for (_uint i = 0; i < vecSize; i++)
		{
//			ReadFile(hFileRead, &iIndiciesNum, sizeof(_uint), &dwByte, nullptr);
			//				cout << "pIndiciesNum : " << iIndiciesNum << endl;
		}
	}

	_uint iNumMaterial{}, iNumTexture{}, iExtLen{}, iFullPathLen{};
	_char szExt{}, szFullPath{};
//	ReadFile(hFileRead, &iNumMaterial, sizeof(_uint), &dwByte, nullptr);

	//		cout << "m_iNumMaterials : " << iNumMaterial << endl;
	for (_uint i = 0; i < iNumMaterial; i++)
	{
		ReadFile(hFileRead, &iNumTexture, sizeof(_uint), &dwByte, nullptr);
		cout << "iNumTexture : " << iNumTexture << endl;
		for (_uint j = 0; j < iNumTexture; j++)
		{
			ReadFile(hFileRead, &iExtLen, sizeof(_uint), &dwByte, nullptr);
			cout << "iExtLen : " << iExtLen << endl;
			for (_uint k = 0; k < iExtLen; k++)
			{
				ReadFile(hFileRead, &szExt, sizeof(_char), &dwByte, nullptr);
				cout << szExt;
			}
			cout << endl;
			ReadFile(hFileRead, &iFullPathLen, sizeof(_uint), &dwByte, nullptr);
			for (_uint k = 0; k < iFullPathLen; k++)
			{
				ReadFile(hFileRead, &szFullPath, sizeof(_char), &dwByte, nullptr);
				cout << szFullPath;
			}
			cout << endl;
		}
	}
	
	return S_OK;
}

HRESULT CModel::Ready_Meshes_ReadData_NonAnim(HANDLE hFileRead)
{
	ReadFile(hFileRead, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create_NonAnim(m_pDevice, m_pContext, TYPE_NONANIM, this,XMLoadFloat4x4(&m_PreTransformMatrix), hFileRead);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials_ReadData_NonAnim(HANDLE hFileRead)
{
	ReadFile(hFileRead, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		CMeshMaterial* pMeshMaterial = CMeshMaterial::Create_ReadData(m_pDevice, m_pContext,  hFileRead);

		m_Materials.push_back(pMeshMaterial);
	}
	return S_OK;
}

//HRESULT CModel::Initialize_Prototype(TYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
//{
//	m_eModelType = eModelType;
//
//	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
//
//	_uint		iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };
//
//	if (TYPE_NONANIM == m_eModelType)
//		iFlag |= aiProcess_PreTransformVertices;
//
//	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
//
//	if (nullptr == m_pAIScene)
//		return E_FAIL;
//
//	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
//		return E_FAIL;
//
//	if (FAILED(Ready_Meshes()))
//		return E_FAIL;
//
//	if (FAILED(Ready_Materials(pModelFilePath)))
//		return E_FAIL;
//
//	if (FAILED(Ready_Animations()))
//		return E_FAIL;
//
//	return S_OK;
//}

HRESULT CModel::Initialize(void* pArg)
{

	return S_OK;
}

HRESULT CModel::Bind_Material_ShaderResource(CShader* pShader, _uint iMeshIndex, aiTextureType eMaterialType, _uint iIndex, const _char* pConstantName)
{
	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	return m_Materials[iMaterialIndex]->Bind_ShaderResource(pShader, eMaterialType, iIndex, pConstantName);
}

HRESULT CModel::Bind_Mesh_BoneMatrices(CShader* pShader, _uint iMeshIndex, const _char* pConstantName)
{
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, m_Bones, pConstantName);
}

_bool CModel::Play_Animation(_float fTimeDelta)
{
	/* 모델의 뼈의 행렬(TransformationMatrix)을 현재 애니메이션에 맞는 상태로 갱신해준다. */
	_bool		isFinished = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(m_Bones, &m_fCurrentPosition, m_isLoop, fTimeDelta);

	/* 모든 뼈들의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}


	return isFinished;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

//HRESULT CModel::Ready_Meshes()
//{
//	m_iNumMeshes = m_pAIScene->mNumMeshes;
//
//	for (size_t i = 0; i < m_iNumMeshes; i++)
//	{
//		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, this, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix));
//		if (nullptr == pMesh)
//			return E_FAIL;
//
//		m_Meshes.push_back(pMesh);
//	}
//
//	return S_OK;
//}
//
//HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
//{
//	m_iNumMaterials = m_pAIScene->mNumMaterials;
//
//	for (size_t i = 0; i < m_iNumMaterials; i++)
//	{
//		CMeshMaterial* pMeshMaterial = CMeshMaterial::Create(m_pDevice, m_pContext, pModelFilePath, m_pAIScene->mMaterials[i]);
//
//		m_Materials.push_back(pMeshMaterial);
//	}
//	return S_OK;
//}

//HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
//{
//	CBone* pBone = CBone::Create(pAINode, iParentIndex);
//	if (nullptr == pBone)
//		return E_FAIL;
//
//	m_Bones.push_back(pBone);
//
//	_int iParentBoneIndex = m_Bones.size() - 1;
//
//	for (_int i = 0; i < pAINode->mNumChildren; ++i)
//	{
//		Ready_Bones(pAINode->mChildren[i], iParentBoneIndex);
//	}
//
//	return S_OK;
//}

//HRESULT CModel::Ready_Animations()
//{
//	/* 애니메이션 정보 : 이 애님을 표현하기위해서 어떤 뼈들을 움직여야하는가? */
//	/* 그 뼈들의 상태가 시간에 따라서 어떻게 변화하는가? */
//	m_iNumAnimations = m_pAIScene->mNumAnimations;
//	m_ChannelCurrentKeyFrameIndex.resize(m_iNumAnimations);
//	for (size_t i = 0; i < m_iNumAnimations; i++)
//	{
//		CAnimation* pAnimation = CAnimation::Create(this, m_pAIScene->mAnimations[i]);
//		if (nullptr == pAnimation)
//			return E_FAIL;
//
//		m_ChannelCurrentKeyFrameIndex[i].resize(m_pAIScene->mAnimations[i]->mNumChannels);
//
//		m_Animations.push_back(pAnimation);
//	}
//
//
//
//
//
//	return S_OK;
//}

CModel* CModel::Create_ReadDataFile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType,const wstring pDataFilePath, _fmatrix PreTransformMatrix, _uint iIndex)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype_ReadDataFile(eModelType, pDataFilePath, iIndex, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

//CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
//{
//	CModel* pInstance = new CModel(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PreTransformMatrix)))
//	{
//		MSG_BOX("Failed to Created : CModel");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	for (auto& pMaterial : m_Materials)
		Safe_Release(pMaterial);

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);


}
