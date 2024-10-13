#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(class CModel* pModel, HANDLE hFileRead);
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _float fCurrentPosition);
	void Update_TransformationMatrix_UpperBody(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _float fCurrentPosition);
	void Update_TransformationMatrix_LowerBody(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _float fCurrentPosition);


public:
	_bool Update_LinearInterPolation( const vector<class CBone*>& Bones, vector<string> szName, _float fTimeDelta);
	_bool Update_LinearInterPolation_UpperBody(const vector<class CBone*>& Bones, vector<string> szName, _float fTimeDelta);
	_bool Update_LinearInterPolation_LowerBody(const vector<class CBone*>& Bones, vector<string> szName, _float fTimeDelta);

	void Set_InterPolationCurrentTime_Init() { m_fInterPolation_CurrentTime = 0.f; }
public:
	KEYFRAME&		Get_LastKeyFrame() { return LastKeyFrame; }

	_char* Get_szName() { return m_szName; }
	string Get_strName() { return m_strName; }

private:
	_char								m_szName[MAX_PATH];

	_uint								m_iNumKeyFrames = {};
	vector<KEYFRAME>					m_KeyFrames;

	_uint								m_iBoneIndex = {};

	KEYFRAME		LastKeyFrame;
	KEYFRAME		LastKeyFrame_UpperBody;
	KEYFRAME		LastKeyFrame_LowerBody;
	_float			m_fInterPolation_CurrentTime{};
	_float			m_fInterPolation_TargetTime{};
	_bool			m_bInitOnce = false;
	_bool			m_bInitOnce_UpperBody = false;
	_bool			m_bInitOnce_LowerBody = false;
	_float			fTemp = 0.f;
	_float			fTemp_UpperBody = 0.f;
	_float			fTemp_LowerBody = 0.f;
	string			m_strName = {};
public:
	static CChannel* Create(class CModel* pModel, HANDLE hFileRead);
	virtual void Free() override;

private:
	DWORD			dwByte = 0;
};

END