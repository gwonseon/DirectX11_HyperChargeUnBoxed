#include "UIManager.h"
#include "GameInstance.h"



CUIManager::CUIManager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);

}

HRESULT CUIManager::Initialize()
{
    return S_OK;
}

void CUIManager::Update(_float fTimeDelta)
{
}

void CUIManager::CircleGauge_Interaction(CLayer* Item, CLayer* UI)
{
    if(Item != nullptr && UI != nullptr && Item->Get_GameObjectList_Size() > 0)
    {
        for (auto& pUI : UI->Get_GameObject_List())
        {
            for (auto& pItem : Item->Get_GameObject_List())
            {
                if (pItem->Get_Interaction() == true)
                {
                    pUI->Set_Interaction(true);
                    return;
                }
            }
            pUI->Set_Interaction(false);
        }
    }
  

}

CUIManager* CUIManager::Create()
{
    CUIManager* pInstance = new CUIManager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CUIManager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUIManager::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}
