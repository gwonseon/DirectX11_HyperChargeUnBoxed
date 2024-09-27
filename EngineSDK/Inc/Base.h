#pragma once

#include "Engine_Defines.h"

/* 모든 클래스의 부모가 되는 클래스다. */
/* 레퍼런스 카운트를 관리한다. (AddRef, Release) */

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	/* 레퍼런스 카운트를 증가시킨다. 증가시킨 레퍼런스 카운트를 리턴한다. */
	_uint AddRef();

	/* (레퍼런스 카운트를 감소시킨다. or 삭제한다.) 감소시키기 전의 레퍼런스 카운트를 리턴한다. */
	_uint Release();

private:
	_uint			m_iRefCnt = { 0 };

public:
	virtual void Free();
};

END