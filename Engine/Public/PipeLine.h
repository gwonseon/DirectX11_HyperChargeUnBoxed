#pragma once
#include "Base.h"



class CPipeLine final : public CBase
{
public:
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END};

private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	// Getter
	const _float4x4* Get_TransformFloat4x4(TRANSFORMSTATE eState)
	{
		return &m_TransformationMatrix[eState];
	}


	_matrix Get_TransformMatrix(TRANSFORMSTATE eState)
	{
		return XMLoadFloat4x4(&m_TransformationMatrix[eState]);
	}
	_matrix Get_TransformMatrixInverse(TRANSFORMSTATE eState)
	{
		return XMLoadFloat4x4(&m_TransformationMatrixInverse[eState]);
	}

	const _float4* Get_CamPosition()
	{
		return &m_vCamPosition;
	}

public:
	// Setter
	void Set_TransformMatrix(TRANSFORMSTATE eState, _fmatrix TransformMatrix)
	{
		XMStoreFloat4x4(&m_TransformationMatrix[eState], TransformMatrix);
	}

public:
	HRESULT Update();
private:
	_float4x4	m_TransformationMatrix[D3DTS_END];
	_float4x4	m_TransformationMatrixInverse[D3DTS_END];
	_float4		m_vCamPosition;

public:
	static CPipeLine* Create();
	virtual void Free() override;

};

