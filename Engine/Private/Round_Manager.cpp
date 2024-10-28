#include "Round_Manager.h"
#include "GameInstance.h"

CRound_Manager::CRound_Manager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CRound_Manager::Initialize()
{
    return S_OK;
}

void CRound_Manager::Update(_float fTimeDelta, _uint& iCurrentRound ,_bool& bBuildMode, CLayer* Monster_Near, CLayer* Monster_Far, _bool& bRoundStart, _float& SkipTimer)
{
    SkipTimer = m_fBreakTimeSkip_Timer;
    if (iCurrentRound == BREAKTIME_ROUND && bBuildMode == true && bRoundStart == false) // 쉬는 시간
    {
     //   cout << "쉬는 시간 시작 " << m_fBreakTime_Timer << endl;
        m_fBreakTime_Timer += fTimeDelta; // 쉬는 시간 타이머 
        if (m_fBreakTime_Timer >= 60.f)
        {
            // 시간 되면 라운드 넘어감
            ++m_iCurrent_Round;
            m_fBreakTime_Timer = 0.f;
            iCurrentRound = m_iCurrent_Round;
            bRoundStart = true; // 라운드 시작
            bBuildMode = false; // 빌드 모드 아님 ( 총)
          //  cout << "이번 라운드 : " << m_iCurrent_Round << endl;
        }
    }
    else if (iCurrentRound == BREAKTIME_ROUND && bBuildMode == false && bRoundStart == false)
    {
        // cout << "스킵 시작 " << endl;
        
        m_fBreakTimeSkip_Timer -= fTimeDelta; // 쉬는 시간 스킵 시작
        
        if (m_fBreakTimeSkip_Timer <= 0.f) // 쉬는 시간 스킵
        {
            ++m_iCurrent_Round;
            m_fBreakTimeSkip_Timer = 5.f;
            iCurrentRound = m_iCurrent_Round; 
            bBuildMode = false; // 빌드 모드 아님 ( 총)
            bRoundStart = true; // 라운드 시작

            //cout << "이번 라운드(스킵함) : " << m_iCurrent_Round << endl;
        }
    }
    else
    {
        if (Monster_Far == nullptr || Monster_Near == nullptr)
            return;
        m_fBreakTimeSkip_Timer = 5.f; // 쉬는 시간 스킵 타이머 초기화
        m_fBreakTime_Timer = 0.f;       // 쉬는 시간 타이머 초기화
        _int iMonster_Near =   Monster_Near->Get_GameObjectList_Size();
        _int iMonster_Far = Monster_Far->Get_GameObjectList_Size();
        m_iMonster_Count = iMonster_Near + iMonster_Far;
        
        // 몬스터 수가 0이하면 쉬는 시간 
        if (m_iMonster_Count <= 0)
        {
            cout << "쉬는 시간 몬스터 다 잡음 : " << m_iMonster_Count << endl;
            iCurrentRound = 0;     // 0번 라운드가 쉬는 시간
            bBuildMode = true;   // 빌드 모드
            bRoundStart = false; // 라운드 끝남
        }
    }
}


CRound_Manager* CRound_Manager::Create()
{
    CRound_Manager* pInstance = new CRound_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CRound_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRound_Manager::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);

}
