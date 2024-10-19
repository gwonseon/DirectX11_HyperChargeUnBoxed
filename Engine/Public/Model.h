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
	const _float4x4* Get_BoneMatrix(const _char* pBoneName) const;
public:
	void Set_Animation(_uint iAnimIndex, _bool isLoop = false) {
		m_iCurrentAnimIndex = iAnimIndex;
		m_isLoop = isLoop;
		m_bLinearInterpolation = false; // 선형보간 하도록 false 로 바꿔줌
	}
	void Set_Animation_UpperBody(_uint iAnimIndex, _bool isLoop = false) {
		m_iCurrentAnimIndex_UpperBody = iAnimIndex;
		m_isLoop_UpperBody = isLoop;
		m_bLinearInterpolation_UpperBody = false; // 선형보간 하도록 false 로 바꿔줌
	}

	void Set_Animation_LowerBody(_uint iAnimIndex, _bool isLoop = false) {
		m_iCurrentAnimIndex_LowerBody = iAnimIndex;
		m_isLoop_LowerBody = isLoop;
		m_bLinearInterpolation_LowerBody = false; // 선형보간 하도록 false 로 바꿔줌
	}

	_uint Get_CurrentAnimationIndex() { return m_iCurrentAnimIndex; }
	_uint Get_CurrentAnimationIndex_UpperBody() { return m_iCurrentAnimIndex_UpperBody; }
	_uint Get_CurrentAnimationIndex_LowerBody() { return m_iCurrentAnimIndex_LowerBody; }

	void Set_SecondPreTransform(const _float4x4& mat) { m_PreTransformMatrix_Second = mat; }
	_float4x4* Get_SecondPreTransform() { return &m_PreTransformMatrix_Second; }
public:
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_Material_ShaderResource(class CShader* pShader, _uint iMeshIndex, aiTextureType eMaterialType, _uint iIndex, const _char* pConstantName);
	HRESULT Bind_Mesh_BoneMatrices(class CShader* pShader, _uint iMeshIndex, const _char* pConstantName);
	_bool Play_Animation(_float fTimeDelta, _bool Once);
	_bool Play_Animation_UpperBody(_float fTimeDelta, _float fRotation_Angle, _uint iUpperMotion, _bool& bShot);
	_bool Play_Animation_LowerBody(_float fTimeDelta);


	HRESULT Render(_uint iMeshIndex);


private:
	TYPE							m_eModelType = { TYPE_END };
	_float4x4						m_PreTransformMatrix = {};


	_float4x4						m_PreTransformMatrix_Second{};



	_uint							m_iNumMeshes = { 0 };
	vector<class CMesh*>			m_Meshes;

	_uint							m_iNumMaterials = { 0 };
	vector<class CMeshMaterial*>	m_Materials;

	vector<class CBone*>			m_Bones;

	_bool							m_isLoop = { false };
	_bool							m_isLoop_UpperBody = { false };
	_bool							m_isLoop_LowerBody = { false };

	_uint							m_iCurrentAnimIndex = {0};
	_uint							m_iCurrentAnimIndex_UpperBody = { 0 };
	_uint							m_iCurrentAnimIndex_LowerBody = { 0 };

	_uint							m_iNumAnimations = { 0 };


	vector<class CAnimation*>		m_Animations;


	_bool							m_bAnim_NoneLoop = false;
	_bool							m_bAnim_NoneLoop_UpperBody = false;
	_bool							m_bAnim_NoneLoop_LowerBody = false;

	_bool							isFinished{};
	_bool							isFinished_UpperBody{};
	_bool							isFinished_LowerBody{};
private:
	_uint							m_iPrevAnimIndex = {0};		// 보간을 위한 인덱스 저장
	_uint							m_iPrevAnimIndex_UpperBody = { 0 };		// 보간을 위한 인덱스 저장
	_uint							m_iPrevAnimIndex_LowerBody = { 0 };		// 보간을 위한 인덱스 저장


	_bool							m_bLinearInterpolation = {};
	_bool							m_bLinearInterpolation_UpperBody = {};
	_bool							m_bLinearInterpolation_LowerBody = {};

	KEYFRAME						PrevKeyFrame{};
	KEYFRAME						PrevKeyFrame_UpperBody{};
	KEYFRAME						PrevKeyFrame_LowerBody{};
	_uint	m_iPrevNumChannels{};
	_bool bAnimChange = false;

public:
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;


public: // Data파일 Read용
	static CModel* Create_ReadDataFile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const wstring pDataFilePath, _fmatrix PreTransformMatrix, _uint iIndex);
	HRESULT Initialize_Prototype_ReadDataFile(TYPE eModelType, const wstring pDataFile, _uint iIndex, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	HRESULT Ready_Meshes_ReadData_NonAnim(HANDLE hFileRead);
	HRESULT Ready_Materials_ReadData_NonAnim(HANDLE hFileRead);



	static CModel* Create_ReadDataFile_For_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const wstring pDataFilePath, _fmatrix PreTransformMatrix, _uint iIndex);
	HRESULT Initialize_Prototype_ReadDataFile_For_Anim(TYPE eModelType, const wstring pDataFile, _uint iIndex, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	HRESULT Ready_Bones( _int iParentIndex, HANDLE hFileRead);
	HRESULT Ready_Animations(HANDLE hFileRead);
	HRESULT Ready_Meshes_ReadData(HANDLE hFileRead);



private:
	DWORD			dwByte = 0;
};

END