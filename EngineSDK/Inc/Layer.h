#pragma once
#include "Base.h"

// CGameObject*를 원소로 갖는 list 컨테이너를 m_GameObjects로 갖는다.
// Add하고 Update들 다 있다/


BEGIN(Engine)
class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_Component(const _wstring& strComponentTag, _uint iIndex, _uint iPartObjID = 0);

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
//	class CComponent* Get_Component(const _tchar* pComponentTag, _uint iIndex = 0);
	class CGameObject* Get_Object(_uint iIndex = 0);

	void GameObject_Clear();
	void Set_Dead();


	// 충돌
public:
	list<class CGameObject*> Get_GameObject_List() { return m_GameObjects; }



private:
	list<class CGameObject*> m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;


};
END
