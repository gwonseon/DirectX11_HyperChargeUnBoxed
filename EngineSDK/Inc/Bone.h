#pragma once

#include "Base.h"



/* assimp에서 제공하는 뼈의 정보는 세가지 타입으로 표현한다. */
/* aiNode, aiBone, aiNodeAnim */

/* 뼈 하나의 상태 정보를 가진다. */
/* */

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_Name() const {
		return m_szName;
	}
	_matrix Get_CombinedTransformationMatrix() {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}
	const _float4x4* Get_CombinedTransformationFloat4x4Ptr() const {
		return &m_CombinedTransformationMatrix;
	}
	void Set_TransformationMatrix(_fmatrix TransformationMatrix)
	{
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}
	_float4x4 Get_TransformationMatrix() {	return m_TransformationMatrix;	}


public:
	HRESULT Initialize(_uint iParentBoneIndex, HANDLE hFileRead);
	void Update_CombinedTransformationMatrix(const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix);
	void Update_CombinedTransformationMatrix(const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix, _float fRotation_Angle);

private:
	_char				m_szName[MAX_PATH] = {};

	/* 부모 기준으로 표현된 나만의 상태를 표현하기위한 행렬. */
	_float4x4			m_TransformationMatrix = {};

	/* 자식행렬 * 부모행렬 */
	/* m_CombinedTransformationMatrix =
	m_TransformationMatrix * Parent`s m_CombinedTransformationMatrix */
	_float4x4			m_CombinedTransformationMatrix = {};

	_int				m_iParentBoneIndex = { -1 };

	// CBone*				m_pParent = { nullptr };


public:
	static CBone* Create(_uint iParentBoneIndex, HANDLE hFileRead);
	CBone* Clone();
	virtual void Free() override;



public:


private:
	DWORD			dwByte = 0;

};

END