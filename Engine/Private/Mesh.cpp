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

		XMStoreFloat4x4(&BoneMatrices[iNumBones++],	ResultMatrix);
	}

	return pShader->Bind_Matrices(pConstantName, BoneMatrices, 512);
}

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
	char* buffer = new char[iLen + 1]; // +1 for null terminator
	ReadFile(hFileRead, buffer, iLen * sizeof(_char), &dwByte, nullptr);
	buffer[iLen] = '\0';
	strcpy_s(m_szName, iLen + 1, buffer); // +1 to include the null terminator
	// 메모리 해제
	delete[] buffer;

	// cout을 통해 문자열 출력
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
	if(eModelType == CModel::TYPE_NONANIM)
	{
		ReadFile(hFileRead, &iFaceSize, sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iFaceSize; i++)
		{
			ReadFile(hFileRead, &iIndiciesNum, sizeof(_uint), &dwByte, nullptr);
			pIndices[iNumIndices++] = iIndiciesNum;

		}
	}
	else
	{
		for (_uint i = 0; i < m_iFaceNum; i++)
		{
			ReadFile(hFileRead, &pIndices[iNumIndices], sizeof(_uint), &dwByte, nullptr);  // for Export 
//			cout << pIndices[iNumIndices] << endl;
			iNumIndices++;
			ReadFile(hFileRead, &pIndices[iNumIndices], sizeof(_uint), &dwByte, nullptr);  // for Export 
//			cout << pIndices[iNumIndices] << endl;
			iNumIndices++;
			ReadFile(hFileRead, &pIndices[iNumIndices], sizeof(_uint), &dwByte, nullptr);  // for Export 
//			cout << pIndices[iNumIndices] << endl;
			iNumIndices++;

		
		}
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
	m_iVertexStride = sizeof(VTXANIMMESH);
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
	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{

		ReadFile(hFileRead, &pVertices[i].vPosition, sizeof(_float3), &dwByte, nullptr);  // for Export 
		ReadFile(hFileRead, &pVertices[i].vNormal, sizeof(_float3), &dwByte, nullptr);  // for Export 
		ReadFile(hFileRead, &pVertices[i].vTexcoord, sizeof(_float2), &dwByte, nullptr);  // for Export 
		ReadFile(hFileRead, &pVertices[i].vTangent, sizeof(_float3), &dwByte, nullptr);  // for Export 

	

	}


	ReadFile(hFileRead, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);  // for Export 

	for (size_t i = 0; i < m_iNumBones; i++)
	{
	
		_uint iBoneIndex{};
		ReadFile(hFileRead, &iBoneIndex, sizeof(_uint), &dwByte, nullptr);  // for Export 

		m_Bones.push_back(iBoneIndex);

		_float4x4		OffsetMatrix;

		ReadFile(hFileRead, &OffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);  // for Export 


		//cout << OffsetMatrix._11 << "    " << OffsetMatrix._12 << "    " << OffsetMatrix._13 << "     " << OffsetMatrix._14 << endl;
		//cout << OffsetMatrix._21 << "    " << OffsetMatrix._22 << "    " << OffsetMatrix._23 << "     " << OffsetMatrix._24 << endl;
		//cout << OffsetMatrix._31 << "    " << OffsetMatrix._32 << "    " << OffsetMatrix._33 << "     " << OffsetMatrix._34 << endl;
		//cout << OffsetMatrix._41 << "    " << OffsetMatrix._42 << "    " << OffsetMatrix._43 << "     " << OffsetMatrix._44 << endl;
		//cout << "---------------------------------------------------------------------------------------------------------" << endl;
		//
		m_OffsetMatrices.push_back(OffsetMatrix);
		_uint iNumWeight{};
		ReadFile(hFileRead, &iNumWeight, sizeof(_uint), &dwByte, nullptr);  // for Export 

		/* 이 메시에 영향을 주는 i번째 뼈는 pAIBone->mNumWeights만큼의 정점에게 영향을 준다.  */
		for (size_t j = 0; j < iNumWeight; j++)
		{
			_float fBlendWeightX{};
			_float fBlendWeightY{};
			_float fBlendWeightZ{};
			_float fBlendWeightW{};
			_uint iWeightIndex{};

			_float fWeight{};
			ReadFile(hFileRead, &fBlendWeightX, sizeof(_float), &dwByte, nullptr);  // for Export 
			ReadFile(hFileRead, &fBlendWeightY, sizeof(_float), &dwByte, nullptr);  // for Export 
			ReadFile(hFileRead, &fBlendWeightZ, sizeof(_float), &dwByte, nullptr);  // for Export 
			ReadFile(hFileRead, &fBlendWeightW, sizeof(_float), &dwByte, nullptr);  // for Export 
			ReadFile(hFileRead, &iWeightIndex, sizeof(_uint), &dwByte, nullptr);  // for Export 

			ReadFile(hFileRead, &fWeight, sizeof(_float), &dwByte, nullptr);  // for Export 

			if (0 == fBlendWeightX)
			{
				pVertices[iWeightIndex].vBlendIndex.x = i;
				pVertices[iWeightIndex].vBlendWeight.x = fWeight;
			}

			else if (0 == fBlendWeightY)
			{
				pVertices[iWeightIndex].vBlendIndex.y = i;
				pVertices[iWeightIndex].vBlendWeight.y = fWeight;
	//			cout << pVertices[iWeightIndex].vBlendWeight.y << endl;

			}

			else if (0 == fBlendWeightZ)
			{
				pVertices[iWeightIndex].vBlendIndex.z = i;
				pVertices[iWeightIndex].vBlendWeight.z = fWeight;
			}

			else if (0 == fBlendWeightW)
			{
				pVertices[iWeightIndex].vBlendIndex.w = i;
				pVertices[iWeightIndex].vBlendWeight.w = fWeight;
			}

		
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		m_Bones.push_back(pModel->Get_BoneIndex(m_szName));

		_float4x4		OffsetMatrix;

		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.push_back(OffsetMatrix);
	}

	m_InitialDesc.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

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
