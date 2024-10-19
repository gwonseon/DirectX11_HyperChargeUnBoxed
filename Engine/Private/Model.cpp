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
{
	for (auto& pPrototypeAnimation : Prototype.m_Animations)
		m_Animations.push_back(pPrototypeAnimation->Clone());

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
			//cout << pBoneName << endl;
			if (!strcmp(pBone->Get_Name(), pBoneName))
				return true;

			++iBoneIndex;

			return false;
		});

	return iBoneIndex;
}

const _float4x4* CModel::Get_BoneMatrix(const _char* pBoneName) const
{
	return m_Bones[Get_BoneIndex(pBoneName)]->Get_CombinedTransformationFloat4x4Ptr();
}
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


	if (FAILED(Ready_Meshes_ReadData_NonAnim(hFileRead)))
		return E_FAIL;

	if (FAILED(Ready_Materials_ReadData_NonAnim(hFileRead)))
		return E_FAIL;

	CloseHandle(hFileRead);
	return S_OK;
}


CModel* CModel::Create_ReadDataFile_For_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const wstring pDataFilePath, _fmatrix PreTransformMatrix, _uint iIndex)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype_ReadDataFile_For_Anim(eModelType, pDataFilePath, iIndex, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CModel::Initialize_Prototype_ReadDataFile_For_Anim(TYPE eModelType, const wstring pDataFile, _uint iIndex, _fmatrix PreTransformMatrix)
{
	m_eModelType = eModelType;
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
	HANDLE hFileRead = CreateFile(pDataFile.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFileRead)
	{
		MessageBox(NULL, L" ModelData Exporter Failed", L"Error", MB_OK);
		return E_FAIL;
	}
	if (FAILED(Ready_Bones(-1, hFileRead)))
		return E_FAIL;

	if (FAILED(Ready_Meshes_ReadData(hFileRead)))
		return E_FAIL;

	if (FAILED(Ready_Materials_ReadData_NonAnim(hFileRead)))
		return E_FAIL;

	if (FAILED(Ready_Animations(hFileRead)))
		return E_FAIL;

	CloseHandle(hFileRead);
	return S_OK;
}

HRESULT CModel::Ready_Bones(_int iParentIndex, HANDLE hFileRead)
{
	CBone* pBone = CBone::Create(iParentIndex, hFileRead);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	_int iParentBoneIndex = m_Bones.size() - 1;
	_uint iNumChildren = 0;
	ReadFile(hFileRead, &iNumChildren, sizeof(_uint), &dwByte, nullptr);

	for (_int i = 0; i < iNumChildren; ++i)
	{
		Ready_Bones(iParentIndex, hFileRead);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations(HANDLE hFileRead)
{

	ReadFile(hFileRead, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);		// for Export 

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(this, hFileRead);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}
	return S_OK;
}

HRESULT CModel::Ready_Meshes_ReadData_NonAnim(HANDLE hFileRead)
{
	ReadFile(hFileRead, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create_NonAnim(m_pDevice, m_pContext, TYPE_NONANIM, this, XMLoadFloat4x4(&m_PreTransformMatrix), hFileRead);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Meshes_ReadData(HANDLE hFileRead)
{
	ReadFile(hFileRead, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{	
		CMesh* pMesh = CMesh::Create_NonAnim(m_pDevice, m_pContext, TYPE_ANIM, this, XMLoadFloat4x4(&m_PreTransformMatrix), hFileRead);
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

HRESULT CModel::Initialize(void* pArg)
{
	XMStoreFloat4x4(&m_PreTransformMatrix_Second, XMMatrixIdentity());

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

_bool CModel::Play_Animation(_float fTimeDelta, _bool Once = false)
{

	if (m_bAnim_NoneLoop == true) // 마지막 동작을 한 번 더 하는 문제를 해결하기 위해 루프가 끝났을 때를 기억해 초기화만 해준다
	{
		m_Animations[m_iCurrentAnimIndex]->CurrentPosition_Init(m_Bones);
		m_bAnim_NoneLoop = false;
	}
	else
	{
		if (m_iCurrentAnimIndex != m_iPrevAnimIndex)
		{
			// 이전 애니메이션 인덱스가 유효한지 확인
			
			
			if(m_Animations[m_iCurrentAnimIndex]->Get_PrevKeyFrame() != nullptr )
			{
				m_Animations[m_iCurrentAnimIndex]->CurrentPosition_Init(m_Bones);

				PrevKeyFrame = *m_Animations[m_iPrevAnimIndex]->Get_PrevKeyFrame();
				const vector<string> strName = m_Animations[m_iPrevAnimIndex]->Get_ChannelNames();
				m_bLinearInterpolation = m_Animations[m_iCurrentAnimIndex]->Update_LinearInterPolation(&PrevKeyFrame, m_Bones, strName, fTimeDelta);
				for (auto& pBone : m_Bones)
				{
					pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix), fTimeDelta);
				}
			}

			if (m_Animations[m_iCurrentAnimIndex]->Get_PrevKeyFrame() == nullptr)
			{
				m_iPrevAnimIndex = m_iCurrentAnimIndex;
			}
			else if (m_Animations[m_iPrevAnimIndex]->Get_PrevKeyFrame() == nullptr)
			{
				m_iPrevAnimIndex = m_iCurrentAnimIndex;
			}
			if (m_bLinearInterpolation == true)
			{
				m_iPrevAnimIndex = m_iCurrentAnimIndex;
			}
		}
		if(m_iPrevAnimIndex == m_iCurrentAnimIndex)
		{
			// 모델의 뼈의 행렬(TransformationMatrix)을 현재 애니메이션에 맞는 상태로 갱신해준다.
			isFinished = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(m_Bones, m_isLoop, fTimeDelta);

			// 모든 뼈들의 CombinedTransformationMatrix를 갱신한다.
			for (auto& pBone : m_Bones)
			{
				pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix), fTimeDelta);
			}

			m_bAnim_NoneLoop = isFinished; // 애니메이션의 종료 여부를 설정
			m_iPrevAnimIndex = m_iCurrentAnimIndex;
		}
	}

	return isFinished;

}

_bool CModel::Play_Animation_UpperBody(_float fTimeDelta, _float fRotation_Angle, _uint iUpperMotion, _bool& bShot)
{
	if (m_bAnim_NoneLoop_UpperBody == true) // 마지막 동작을 한 번 더 하는 문제를 해결하기 위해 루프가 끝났을 때를 기억해 초기화만 해준다
	{
		m_Animations[m_iCurrentAnimIndex_UpperBody]->CurrentPosition_UpperBody_Init(m_Bones);
		m_bAnim_NoneLoop_UpperBody = false;
	}
	else
	{
		if (m_iCurrentAnimIndex_UpperBody != m_iPrevAnimIndex_UpperBody) // 애니메이션이 달라졌을 때 들어옴
		{
			// 이전 애니메이션 인덱스가 유효한지 확인
			if (m_Animations[m_iPrevAnimIndex_UpperBody]->Get_PrevKeyFrame_UpperBody() != nullptr )
			{
			
				PrevKeyFrame_UpperBody = *m_Animations[m_iPrevAnimIndex_UpperBody]->Get_PrevKeyFrame_UpperBody();
				const vector<string> strName = m_Animations[m_iPrevAnimIndex_UpperBody]->Get_ChannelNames();
				m_bLinearInterpolation_UpperBody = m_Animations[m_iCurrentAnimIndex_UpperBody]->Update_LinearInterPolation_Player(&PrevKeyFrame_UpperBody, m_Bones, strName, fTimeDelta, true);
			
				for (auto& pBone : m_Bones)
				{
					pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix), fRotation_Angle, iUpperMotion);
				}
				
			}
			if (m_Animations[m_iCurrentAnimIndex_UpperBody]->Get_PrevKeyFrame_UpperBody() == nullptr)
			{
				m_iPrevAnimIndex_UpperBody = m_iCurrentAnimIndex_UpperBody;
			}
			else if (m_Animations[m_iPrevAnimIndex_UpperBody]->Get_PrevKeyFrame_UpperBody() == nullptr)
			{
				m_iPrevAnimIndex_UpperBody = m_iCurrentAnimIndex_UpperBody;
			}
			if (m_bLinearInterpolation_UpperBody == true)
			{
				m_iPrevAnimIndex_UpperBody = m_iCurrentAnimIndex_UpperBody;
			}
		}
		if (m_iPrevAnimIndex_UpperBody == m_iCurrentAnimIndex_UpperBody)
		{
			// 모델의 뼈의 행렬(TransformationMatrix)을 현재 애니메이션에 맞는 상태로 갱신해준다.
			isFinished_UpperBody = m_Animations[m_iCurrentAnimIndex_UpperBody]->Update_TransformationMatrix_Player(m_Bones, m_isLoop_UpperBody, fTimeDelta, true, iUpperMotion, bShot);
			// 모든 뼈들의 CombinedTransformationMatrix를 갱신한다.
		
			for (auto& pBone : m_Bones)
			{
				pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix), fRotation_Angle, iUpperMotion);
			}
			m_bAnim_NoneLoop_UpperBody = isFinished_UpperBody; // 애니메이션의 종료 여부를 설정
			m_iPrevAnimIndex_UpperBody = m_iCurrentAnimIndex_UpperBody;
		}
	 }

	return isFinished_UpperBody;
}

_bool CModel::Play_Animation_LowerBody(_float fTimeDelta)
{
	if (m_bAnim_NoneLoop_LowerBody == true) // 마지막 동작을 한 번 더 하는 문제를 해결하기 위해 루프가 끝났을 때를 기억해 초기화만 해준다
	{
		m_Animations[m_iCurrentAnimIndex_LowerBody]->CurrentPosition_LowerBody_Init(m_Bones);
		m_bAnim_NoneLoop_LowerBody = false;
	}

	else
	{
		if (m_iCurrentAnimIndex_LowerBody != m_iPrevAnimIndex_LowerBody)
		{
			// 이전 애니메이션 인덱스가 유효한지 확인

			
			if (m_Animations[m_iPrevAnimIndex_LowerBody]->Get_PrevKeyFrame() != nullptr)
			{
				PrevKeyFrame = *m_Animations[m_iPrevAnimIndex_LowerBody]->Get_PrevKeyFrame();
				const vector<string> strName = m_Animations[m_iPrevAnimIndex_LowerBody]->Get_ChannelNames();
				m_bLinearInterpolation_LowerBody = m_Animations[m_iCurrentAnimIndex_LowerBody]->Update_LinearInterPolation_Player(&PrevKeyFrame, m_Bones, strName, fTimeDelta,false);
				for (auto& pBone : m_Bones)
				{
					pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
				}
			}

			if (m_Animations[m_iCurrentAnimIndex_LowerBody]->Get_PrevKeyFrame() == nullptr)
			{
				m_iPrevAnimIndex_LowerBody = m_iCurrentAnimIndex_LowerBody;
			}
			else if (m_Animations[m_iPrevAnimIndex_LowerBody]->Get_PrevKeyFrame() == nullptr)
			{
				m_iPrevAnimIndex_LowerBody = m_iCurrentAnimIndex_LowerBody;
			}
			if (m_bLinearInterpolation_LowerBody == true)
			{
				m_iPrevAnimIndex_LowerBody = m_iCurrentAnimIndex_LowerBody;
			}
		}
		if (m_iPrevAnimIndex_LowerBody == m_iCurrentAnimIndex_LowerBody)
		{
			_bool Temp{}; // 상체에 필요한 매개변수 때문에 만든 빈 불값
			// 모델의 뼈의 행렬(TransformationMatrix)을 현재 애니메이션에 맞는 상태로 갱신해준다.
			isFinished_LowerBody = m_Animations[m_iCurrentAnimIndex_LowerBody]->Update_TransformationMatrix_Player(m_Bones, m_isLoop_LowerBody, fTimeDelta,false,true, Temp);

			// 모든 뼈들의 CombinedTransformationMatrix를 갱신한다.
			for (auto& pBone : m_Bones)
			{
				pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
			}

			m_bAnim_NoneLoop_LowerBody = isFinished_LowerBody; // 애니메이션의 종료 여부를 설정
			m_iPrevAnimIndex_LowerBody = m_iCurrentAnimIndex_LowerBody;
		}
		

			m_bAnim_NoneLoop_LowerBody = isFinished_LowerBody; // 애니메이션의 종료 여부를 설정
			m_iPrevAnimIndex_LowerBody = m_iCurrentAnimIndex_LowerBody;



		}
	
	return isFinished_LowerBody;
}

//_bool CModel::Play_Animation(_float fTimeDelta, _bool Once = false)
//{
//	if (m_bAnim_NoneLoop == true) // 마지막 동작을 한 번 더 하는 문제를해결하기 위해 루프가 끝났을 때를 기억해 초기화만 해준다
//	{
//		m_Animations[m_iCurrentAnimIndex]->CurrentPosition_Init(m_Bones);
//		m_bAnim_NoneLoop = false;
//	}
//	else
//	{
//	
//		if (m_iPrevAnimIndex != m_iCurrentAnimIndex)
//		{
//			bAnimChange = true;
//		}
//		/* 모델의 뼈의 행렬(TransformationMatrix)을 현재 애니메이션에 맞는 상태로 갱신해준다. */
//		isFinished = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(m_Bones, m_isLoop, bAnimChange,fTimeDelta);
//		
//		/* 모든 뼈들의 CombinedTransformationMatrix를 갱신한다. */
//		for (auto& pBone : m_Bones)
//		{
//			pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
//		}
//		m_bAnim_NoneLoop = isFinished;
//		m_iPrevAnimIndex = m_iCurrentAnimIndex;
//		
//		
//	}
//
//
//
//	return isFinished;
//}


//_bool CModel::Play_Animation(_float fTimeDelta)
//{
//	
//	/* 모델의 뼈의 행렬(TransformationMatrix)을 현재 애니메이션에 맞는 상태로 갱신해준다. */
//	_bool		isFinished = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(m_Bones, m_isLoop, fTimeDelta);
//
//	/* 모든 뼈들의 CombinedTransformationMatrix를 갱신한다. */
//	for (auto& pBone : m_Bones)
//	{
//		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
//	}
//
//
//	return isFinished;
//}
HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}



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
