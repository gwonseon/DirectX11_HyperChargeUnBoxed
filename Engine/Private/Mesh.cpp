#include "..\Public\Mesh.h"
#include "Bone.h"
#include "Model.h"
#include "Shader.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer{ Prototype }
{
}

//HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, class CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
//{
//	strcpy_s(m_szName, pAIMesh->mName.data);
//	m_iMaterialIndex = pAIMesh->mMaterialIndex;
//	m_iNumVertices = pAIMesh->mNumVertices;
//	m_iIndexStride = sizeof(_uint);
//	m_iNumIndices = pAIMesh->mNumFaces * 3;
//	m_iNumVertexBuffers = 1;
//	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
//	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//
//
//#pragma region VERTEX_BUFFER
//
//	HRESULT	hr = eModelType == CModel::TYPE_NONANIM ?
//		Ready_VIBuffer_For_NonAnim(pAIMesh, PreTransformMatrix) :
//		Ready_VIBuffer_For_Anim(pAIMesh, pModel);
//
//	if (FAILED(hr))
//		return E_FAIL;
//
//#pragma endregion
//
//#pragma region INDEX_BUFFER
//
//	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
//
//	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
//	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	m_BufferDesc.CPUAccessFlags = 0;
//	m_BufferDesc.MiscFlags = 0;
//	m_BufferDesc.StructureByteStride = 0;
//
//	ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
//	_uint* pIndices = new _uint[m_iNumIndices];
//
//	_uint		iNumIndices = { 0 };
//
//	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
//	{
//		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
//		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
//		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
//	}
//
//	m_InitialDesc.pSysMem = pIndices;
//
//	if (FAILED(__super::Create_Buffer(&m_pIB)))
//		return E_FAIL;
//
//#pragma endregion
//
//
//	Safe_Delete_Array(pIndices);
//
//	return S_OK;
//}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const vector<class CBone*>& Bones, const _char* pConstantName)
{
	_float4x4			BoneMatrices[512];

	_uint		iNumBones = { 0 };

	for (auto& iBoneIndex : m_Bones)
	{
		_matrix			CombinedMatrix = Bones[iBoneIndex]->Get_CombinedTransformationMatrix();
		_matrix			OffsetMatrix = XMLoadFloat4x4(&m_OffsetMatrices[iNumBones]);

		_matrix			ResultMatrix = OffsetMatrix * CombinedMatrix;

		XMStoreFloat4x4(&BoneMatrices[iNumBones++],
			ResultMatrix);
	}

	return pShader->Bind_Matrices(pConstantName, BoneMatrices, 512);
}
//
//HRESULT CMesh::Ready_VIBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
//{
//	m_iVertexStride = sizeof(VTXMESH);
//	/* dx9 : 정점버퍼를 할당하고 -> 락언락해서 정점버퍼에 초기값을 채운다. */
//	/* dx9 : 정점버퍼에 초기값을 채우면서 정점버퍼를 할당한다*/
//	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
//
//	/* 할당하고자하는 메모리공간의 크기(Byte)*/
//	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
//
//	/* 버퍼의 속성 (정적, 동적) */
//	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	m_BufferDesc.CPUAccessFlags = 0;
//	m_BufferDesc.MiscFlags = 0;
//	m_BufferDesc.StructureByteStride = m_iVertexStride;
//
//	ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
//	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
//
//	for (size_t i = 0; i < m_iNumVertices; i++)
//	{
//		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
//		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));
//
//		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
//		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));
//
//		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
//		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
//	}
//
//	m_InitialDesc.pSysMem = pVertices;
//
//	if (FAILED(__super::Create_Buffer(&m_pVB)))
//		return E_FAIL;
//	Safe_Delete_Array(pVertices);
//
//	return S_OK;
//}
//
//HRESULT CMesh::Ready_VIBuffer_For_Anim(const aiMesh* pAIMesh, class CModel* pModel)
//{
//	m_iVertexStride = sizeof(VTXANIMMESH);
//	/* dx9 : 정점버퍼를 할당하고 -> 락언락해서 정점버퍼에 초기값을 채운다. */
//	/* dx9 : 정점버퍼에 초기값을 채우면서 정점버퍼를 할당한다*/
//	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
//
//	/* 할당하고자하는 메모리공간의 크기(Byte)*/
//	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
//
//	/* 버퍼의 속성 (정적, 동적) */
//	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	m_BufferDesc.CPUAccessFlags = 0;
//	m_BufferDesc.MiscFlags = 0;
//	m_BufferDesc.StructureByteStride = m_iVertexStride;
//
//	ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
//	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
//	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);
//
//	for (size_t i = 0; i < m_iNumVertices; i++)
//	{
//		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
//		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
//		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
//		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
//	}
//
//	m_iNumBones = pAIMesh->mNumBones;
//
//	for (size_t i = 0; i < m_iNumBones; i++)
//	{
//		aiBone* pAIBone = pAIMesh->mBones[i];
//
//		m_Bones.push_back(pModel->Get_BoneIndex(pAIBone->mName.data));
//
//		_float4x4		OffsetMatrix;
//
//		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
//		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
//
//		m_OffsetMatrices.push_back(OffsetMatrix);
//
//		/* 이 메시에 영향을 주는 i번째 뼈는 pAIBone->mNumWeights만큼의 정점에게 영향을 준다.  */
//		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
//		{
//			/* 이 메시에 영향을 주는 i번째 뼈의 j번째 영향을 주는 정점의 인덱스가 pAIBone->mWeights[j].mVertexId */
//
//			if (0 == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.x)
//			{
//				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.x = i;
//				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.x = pAIBone->mWeights[j].mWeight;
//			}
//
//			else if (0 == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.y)
//			{
//				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.y = i;
//				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.y = pAIBone->mWeights[j].mWeight;
//			}
//
//			else if (0 == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.z)
//			{
//				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.z = i;
//				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.z = pAIBone->mWeights[j].mWeight;
//			}
//
//			else if (0 == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.w)
//			{
//				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.w = i;
//				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.w = pAIBone->mWeights[j].mWeight;
//			}
//		}
//	}
//
//	if (0 == m_iNumBones)
//	{
//		m_iNumBones = 1;
//
//		m_Bones.push_back(pModel->Get_BoneIndex(m_szName));
//
//		_float4x4		OffsetMatrix;
//
//		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
//
//		m_OffsetMatrices.push_back(OffsetMatrix);
//	}
//
//	m_InitialDesc.pSysMem = pVertices;
//
//	if (FAILED(__super::Create_Buffer(&m_pVB)))
//		return E_FAIL;
//
//	Safe_Delete_Array(pVertices);
//
//	return S_OK;
//}

CMesh* CMesh::Create_NonAnim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, CModel* pModel, _fmatrix PreTransformMatrix, HANDLE hFileRead)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype_NonAnim(eModelType, pModel, PreTransformMatrix, hFileRead)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CMesh::Initialize_Prototype_NonAnim(CModel::TYPE eModelType, CModel* pModel, _fmatrix PreTransformMatrix, HANDLE hFileRead)
{
	_uint iLen{};
	ReadFile(hFileRead, &iLen, sizeof(_uint), &dwByte, nullptr);
	for (_uint k = 0; k < iLen; k++)
	{
		ReadFile(hFileRead, &m_szName[k], sizeof(_char), &dwByte, nullptr);
		
	}
	cout << m_szName << endl;
	ReadFile(hFileRead, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFileRead, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);

	m_iIndexStride = sizeof(_uint);
	ReadFile(hFileRead, &m_iFaceNum, sizeof(_uint), &dwByte, nullptr);

	m_iNumIndices = m_iFaceNum * 3;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER

	HRESULT	hr = eModelType == CModel::TYPE_NONANIM ?
		Ready_VIBuffer_For_NonAnim_DataRead(hFileRead ,PreTransformMatrix) :
		Ready_VIBuffer_For_Anim_DataRead(hFileRead,pModel);

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
	_uint* pIndices = new _uint[m_iNumIndices];

	_uint		iNumIndices = { 0 };

	_uint iFaceSize = 0;
	_uint iIndiciesNum = 0;
	ReadFile(hFileRead, &iFaceSize, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iFaceSize; i++)
	{
		ReadFile(hFileRead, &iIndiciesNum, sizeof(_uint), &dwByte, nullptr);
		pIndices[iNumIndices++] = iIndiciesNum;
	}

	m_InitialDesc.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

#pragma endregion
	Safe_Delete_Array(pIndices);
	return S_OK;
}

HRESULT CMesh::Ready_VIBuffer_For_NonAnim_DataRead(HANDLE hFileRead, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);
	/* dx9 : 정점버퍼를 할당하고 -> 락언락해서 정점버퍼에 초기값을 채운다. */
	/* dx9 : 정점버퍼에 초기값을 채우면서 정점버퍼를 할당한다*/
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 할당하고자하는 메모리공간의 크기(Byte)*/
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;

	/* 버퍼의 속성 (정적, 동적) */
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	_uint iVertice = 0;
	ReadFile(hFileRead, &iVertice, sizeof(_uint), &dwByte, nullptr);
	_float3			fVerticesPos{}, fVerticesNor{}, fVerticesTangent{};
	_float2			fVerticesTex{};

	for (_uint i = 0; i < iVertice; i++)
	{
		ReadFile(hFileRead, &fVerticesPos, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFileRead, &fVerticesNor, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFileRead, &fVerticesTex, sizeof(_float2), &dwByte, nullptr);
		ReadFile(hFileRead, &fVerticesTangent, sizeof(_float3), &dwByte, nullptr);

		pVertices[i].vPosition = fVerticesPos;
		pVertices[i].vNormal = fVerticesNor;
		pVertices[i].vTexcoord = fVerticesTex;
		pVertices[i].vTangent = fVerticesTangent;

		//if (i < 3)
		//{
		//	cout << "vPosition.x   : " << pVertices[i].vPosition.x << "vPosition.y   : " << pVertices[i].vPosition.y << "vPosition.z   : " << pVertices[i].vPosition.z << endl;
		//
		//}
	}

	m_InitialDesc.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VIBuffer_For_Anim_DataRead(HANDLE hFileRead, CModel* pModel)
{
	return E_NOTIMPL;
}


//CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, class CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
//{
//	CMesh* pInstance = new CMesh(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize_Prototype(eModelType, pModel, pAIMesh, PreTransformMatrix)))
//	{
//		MSG_BOX("Failed to Created : CMesh");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}

CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CMesh::Free()
{
	__super::Free();

}
