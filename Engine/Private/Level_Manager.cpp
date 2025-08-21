#include "..\Public\Level_Manager.h"
#include "GameInstance.h"

#include "Level.h"

CLevel_Manager::CLevel_Manager()
	: m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Open_Level(_uint iCurrentLevelID,CLevel* pNewLevel)
{
	if(nullptr == pNewLevel)
		return E_FAIL;

	/* 최초레벨할당할때 지우면 큰일난다. m_iCurrentLevelID : 의도치 않은 값으로 초기화되어있기때문에 의도치않은 레벨자원을 지우잖어. */
	if(nullptr != m_pCurrentLevel)
		m_pGameInstance->Clear(m_iCurrentLevelID);
	Safe_Release(m_pCurrentLevel);
	m_pCurrentLevel = pNewLevel;
	m_iCurrentLevelID = iCurrentLevelID;

	return S_OK;
}

HRESULT CLevel_Manager::Close_Level(_uint iLevelID)
{
	m_pGameInstance->Clear(iLevelID);
	return S_OK;
}

void CLevel_Manager::Update(_float fTimeDelta)
{
	m_pCurrentLevel->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	return m_pCurrentLevel->Render();

}

CLevel_Manager* CLevel_Manager::Create()
{
	return new CLevel_Manager();
}

void CLevel_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pCurrentLevel);
	Safe_Release(m_pGameInstance);
}