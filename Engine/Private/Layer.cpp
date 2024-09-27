#include "..\Public/Layer.h"
#include "GameObject.h"


CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
    if (nullptr == pGameObject)
        return E_FAIL;
    m_GameObjects.push_back(pGameObject);
    return S_OK;

}

CComponent* CLayer::Get_Component(const _tchar* pComponentTag, _uint iIndex)
{
    auto	iter = m_GameObjects.begin();
    
    for (_uint i = 0; i < iIndex; ++i)
        ++iter;

    return (*iter)->Find_Component(pComponentTag);
}

CGameObject* CLayer::Get_Object(_uint iIndex)
{
    if (m_GameObjects.size() <= iIndex)
        return nullptr;

    auto	iter = m_GameObjects.begin();

    for (size_t i = 0; i < iIndex; ++i)
        ++iter;

    return *iter;
}



void CLayer::Priority_Update(_float fTimeDelta)
{
    for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
    {
        if (nullptr != (*iter))
        {

            (*iter)->Priority_Update(fTimeDelta);
            if ((*iter)->Get_Dead() == true)
            {
                (*iter)->Release();
                (iter) = m_GameObjects.erase((iter));
            }
            else
            {
                ++iter;
            }
        }
        else
            ++iter;
    }

}

void CLayer::Update(_float fTimeDelta)
{
    for (auto& pGameObject : m_GameObjects)
    {
        if (nullptr != pGameObject)
            pGameObject->Update(fTimeDelta);
    }
}

void CLayer::Late_Update(_float fTimeDelta)
{
    for (auto& pGameObject : m_GameObjects)
    {
        if (nullptr != pGameObject)
            pGameObject->Late_Update(fTimeDelta);
    }
}

void CLayer::GameObject_Clear()
{
    for (auto& pGameObject : m_GameObjects)
    {
        Safe_Release(pGameObject);
    }
    m_GameObjects.clear();
}






CLayer* CLayer::Create()
{
    return new CLayer();
}

void CLayer::Free()
{
    __super::Free();
    for (auto& pGameObject : m_GameObjects)
        Safe_Release(pGameObject);
    m_GameObjects.clear();
}
