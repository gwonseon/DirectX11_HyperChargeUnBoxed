#pragma once

#include "Base.h"

/* 어떤 뼈들을 움직여야하는가? */
/* 해당 뼈들은 시간에 따라 어떤 상태변화를 가지는가? */

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
//	HRESULT Initialize(class CModel* pModel, const aiAnimation* pAIAnimation);
	_bool Update_TransformationMatrix(const vector<class CBone*>& Bones, _float* pCurrentPosition, _bool isLoop, _float fTimeDelta);

private:
	_char					m_szName[MAX_PATH] = {};
	_float					m_fDuration = { 0.f };
	_float					m_fTickPerSecond = { 0.f };


	_uint					m_iNumChannels = {};
	vector<class CChannel*>	m_Channels;

public:
//	static CAnimation* Create(class CModel* pModel, const aiAnimation* pAIAnimation);
	virtual void Free() override;
};

END