
#pragma once
#include "Base.h"
#include <ThreadPool.h>



BEGIN(Engine)

class CThreadMgr final: public CBase
{
private:
	CThreadMgr();
	virtual ~CThreadMgr() = default;


public:
	HRESULT Initialize(CThreadPool* pThread);
	CThreadPool* Get_ptrThreadpool(){
		return m_pThreadpool;
	}
private:
	CThreadPool* m_pThreadpool = nullptr;

public:
	static CThreadMgr* Create(CThreadPool* pThread);
	virtual void Free() override;

};

END