#pragma once

#include "Model.h"
#include "VIBuffer.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
//	virtual HRESULT Initialize_Prototype(CModel::TYPE eModelType, class CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const vector<class CBone*>& Bones, const _char* pConstantName);

private:
	_char					m_szName[MAX_PATH] = "";
	_uint					m_iMaterialIndex = { 0 };

	/* 이 메시의 정점들에게 영향을 주는 뼈들의 갯수 */
	_uint					m_iNumBones = { 0 };

	/* 모델클래스에 선언된 전체 뼈들 중에서 몇번째 뼈가 정점에게 영향을 주는가? */
	vector<_uint>			m_Bones;
	// vector<class CBone*>	m_Bones;

	vector<_float4x4>		m_OffsetMatrices;

private:
	//HRESULT Ready_VIBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	//HRESULT Ready_VIBuffer_For_Anim(const aiMesh* pAIMesh, class CModel* pModel);
	HRESULT Ready_VIBuffer_For_NonAnim_DataRead(HANDLE hFileRead, _fmatrix PreTransformMatrix);
	HRESULT Ready_VIBuffer_For_Anim_DataRead(HANDLE hFileRead, class CModel* pModel);

	// ReadData
public:
	static CMesh* Create_NonAnim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, class CModel* pModel,  _fmatrix PreTransformMatrix, HANDLE hFileRead);
	virtual HRESULT Initialize_Prototype_NonAnim(CModel::TYPE eModelType, class CModel* pModel, _fmatrix PreTransformMatrix, HANDLE hFileRead);

	
private:
	DWORD			dwByte = 0;
	_uint			m_iFaceNum = 0;
public:
//	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, class CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END