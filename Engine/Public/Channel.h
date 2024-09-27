#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
//	HRESULT Initialize(class CModel* pModel, const aiNodeAnim* pAIChannel);
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentPosition);

private:
	_char								m_szName[MAX_PATH];

	_uint								m_iNumKeyFrames = {};
	vector<KEYFRAME>					m_KeyFrames;

	_uint								m_iBoneIndex = {};

	/* 현재 재생되고 있는 위치기준으로 왼쪽에 존재하는 키프레임의 인덱스*/
	_uint								m_iCurrentKeyFrameIndex = {};

public:
//	static CChannel* Create(class CModel* pModel, const aiNodeAnim* pAIChannel);
	virtual void Free() override;
};

END