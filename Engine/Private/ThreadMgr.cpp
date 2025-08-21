#include "ThreadMgr.h"

CThreadMgr::CThreadMgr()
{}

HRESULT CThreadMgr::Initialize(CThreadPool* pThread)
{
	m_pThreadpool = pThread;

    return S_OK;
}

CThreadMgr * CThreadMgr::Create(CThreadPool* pThread)
{
	CThreadMgr* pInstance = new CThreadMgr();

	if(FAILED(pInstance->Initialize(pThread)))
	{
		MSG_BOX("Failed to Created : CThreadMgr");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CThreadMgr::Free()
{
	Safe_Delete(m_pThreadpool);
	__super::Free();
}
