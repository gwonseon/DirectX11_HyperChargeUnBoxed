#pragma once
#include "Base.h"

BEGIN(Engine)


class CTimer final: public CBase
{
private:
	CTimer(void);
	virtual ~CTimer(void) = default;

public:
	_float Get_TimeDelta(void) const
	{
		return m_fTimeDelta;
	}

public:
	HRESULT Ready_Timer(void);
	void Update_Timer(void);

private:
	LARGE_INTEGER			m_FrameTime = {}; // 매 프레임 시간을 받는다
	LARGE_INTEGER			m_FixTime = {}; // 1초 경과 여부를 파악하기 위한 기준 시간
	LARGE_INTEGER			m_LastTime = {}; // 이전 Update 때의 시간
	LARGE_INTEGER			m_CpuTick = {}; // 1초 경과 판단 기준

private:
	_float					m_fTimeDelta = {};

public:
	static CTimer* Create(void);
	virtual void Free(void) override;

};

END