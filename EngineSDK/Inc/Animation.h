#pragma once

#include "Base.h"

/* 어떤 뼈들을 움직여야하는가? */
/* 해당 뼈들은 시간에 따라 어떤 상태변화를 가지는가? */

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(class CModel* pModel, HANDLE hFileRead);
	_bool Update_TransformationMatrix(const vector<class CBone*>& Bones, _bool isLoop,  _float fTimeDelta, _bool bPlay = true);
	_bool Update_TransformationMatrix_Player(const vector<class CBone*>& Bones, _bool isLoop, _float fTimeDelta, _bool bUpper, _uint iUpperMotion, _bool& bShot);

private:
	_char					m_szName[MAX_PATH] = {};
	_float					m_fDuration = { 0.f };
	_float					m_fTickPerSecond = { 0.f };
	_float					m_fCurrentPosition = { 0.f };
	_float					m_fCurrentPosition_UpperBody = { 0.f };
	_float					m_fCurrentPosition_LowerBody = { 0.f };
	_float m_fLerptime{ 0 };

	_uint					m_iNumChannels = {};
	vector<class CChannel*>	m_Channels;
	vector<_uint>			m_iChannelKeyFrameIndices;



public:
	KEYFRAME*& Get_PrevKeyFrame() { return LastKeyFrame; }
	KEYFRAME*& Get_PrevKeyFrame_UpperBody() { return LastKeyFrame_UpperBody; }
	KEYFRAME*& Get_PrevKeyFrame_LowerBody() { return LastKeyFrame_LowerBody; }

	_bool	  Update_LinearInterPolation(KEYFRAME* _PrevKeyFrame, const vector<class CBone*>& Bones, vector<string> szName, _float fTimeDelta);
	_bool	  Update_LinearInterPolation_Player(KEYFRAME* _PrevKeyFrame, const vector<class CBone*>& Bones, vector<string> szName, _float fTimeDelta, _bool bUpper);

	//_char** Get_szName() { return m_szPrevChannelName; }
	_uint	Get_PrevNumChannel() { return m_iPrevNumChannels; }
	const vector<string>& Get_ChannelNames() const {
		return m_vecName;
	}

private:
	KEYFRAME* LastKeyFrame{};
	KEYFRAME* LastKeyFrame_UpperBody{};
	KEYFRAME* LastKeyFrame_LowerBody{};
	//_char** m_szPrevChannelName ;
	_bool m_bLinearInterpolation = false;
	_bool m_bLinearInterpolation_UpperBody = false;
	_bool m_bLinearInterpolation_LowerBody = false;
	vector<string> m_vecName{};
	vector<string> m_vecName_UpperBody{};
	vector<string> m_vecName_LowerBody{};


	//string	m_strName;
	_uint	m_iPrevNumChannels{};
public:
	static CAnimation* Create(class CModel* pModel, HANDLE hFileRead);
	virtual void Free() override;
	CAnimation* Clone();
	
//	// 총 딜레이
//private:
//	_float					m_fRiflrDelay = 0.1f;
//	_float					m_fShotGunDelay = 1.f;
//	_float					m_fPulseCannonDelay = 2.f;
//	_float					m_fTeleportDelay = 2.f;
//	_float					m_fLocketLauncherDelay = 2.f;
//	_float					m_fCurrentDelay = 0.f;



public:
	void CurrentPosition_Init(const vector<class CBone*>& Bones );
	void CurrentPosition_UpperBody_Init(const vector<class CBone*>& Bones);
	void CurrentPosition_LowerBody_Init(const vector<class CBone*>& Bones);

	void CurrentPosition_Init() { m_fCurrentPosition = 0.f; };

private:
	DWORD			dwByte = 0;
	_bool							m_bLimitOnce = false;
};

END