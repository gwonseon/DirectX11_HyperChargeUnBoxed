#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_uint Get_BoneIndex(const _char* pBoneName) const;

public:
	void Set_Animation(_uint iAnimIndex, _bool isLoop = false) {
		m_iCurrentAnimIndex = iAnimIndex;
		m_isLoop = isLoop;
	}

public:
//	HRESULT Initialize_Prototype_For_Export(TYPE eModelType, const _char* pModelFilePath, _uint iIndex, _fmatrix PreTransformMatrix = XMMatrixIdentity());

//	virtual HRESULT Initialize_Prototype(TYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_Material_ShaderResource(class CShader* pShader, _uint iMeshIndex, aiTextureType eMaterialType, _uint iIndex, const _char* pConstantName);
	HRESULT Bind_Mesh_BoneMatrices(class CShader* pShader, _uint iMeshIndex, const _char* pConstantName);
	_bool Play_Animation(_float fTimeDelta);
	HRESULT Render(_uint iMeshIndex);

private:


private:
	TYPE							m_eModelType = { TYPE_END };
	_float4x4						m_PreTransformMatrix = {};

	_uint							m_iNumMeshes = { 0 };
	vector<class CMesh*>			m_Meshes;

	_uint							m_iNumMaterials = { 0 };
	vector<class CMeshMaterial*>	m_Materials;

	vector<class CBone*>			m_Bones;

	_bool							m_isLoop = { false };
	_uint							m_iCurrentAnimIndex = {};
	_uint							m_iNumAnimations = { 0 };
	_float							m_fCurrentPosition = { 0.f };
	vector<vector<_uint>>			m_ChannelCurrentKeyFrameIndex;
	vector<class CAnimation*>		m_Animations;

public:
	//HRESULT Ready_Meshes();
	//HRESULT Ready_Materials(const _char* pModelFilePath);
	//HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	//HRESULT Ready_Animations();


public:
//	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);

	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;


public: // DataÆÄÀÏ Read¿ë
	static CModel* Create_ReadDataFile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const wstring pDataFilePath, _fmatrix PreTransformMatrix, _uint iIndex);
	HRESULT Initialize_Prototype_ReadDataFile(TYPE eModelType, const wstring pDataFile, _uint iIndex, _fmatrix PreTransformMatrix = XMMatrixIdentity());

	HRESULT Ready_Meshes_ReadData_NonAnim(HANDLE hFileRead);
	HRESULT Ready_Materials_ReadData_NonAnim(HANDLE hFileRead);

	HRESULT Read_DataFile(wstring pDataFile, HANDLE hFileRead);


private:
	DWORD			dwByte = 0;
};

END