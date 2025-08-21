#pragma once

#include "Base.h"


/* 1. 원형객체를 보관한다. */
/* 2. 원형객체를 찾아서 복제하여 사본객체를 레이어별로 구분하여 보관한다. */
/* 3. 보관하고 있는 사본 객체들의 반복적인 갱신 작업도 수행해준다. */
/* 4. 보관하고 있는 사본 객체들의 렌더함수를 호출한다. (X) */

BEGIN(Engine)

class CObject_Manager final: public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent* Get_Component(_uint iLevelIndex,const _wstring& strLayerTag,const _wstring& strComponentTag,_uint iIndex,_uint iPartObjID = 0);


public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(const _wstring& strPrototypeTag,class CGameObject* pPrototype);
	HRESULT Add_GameObject_ToLayer(_uint iLevelIndex,const _wstring& strLayerTag,const _wstring& strPrototypeTag,void* pArg);
	class CGameObject* Add_GameObject_ToLayer_ReturnObject(_uint iLevelIndex,const _wstring& strLayerTag,const _wstring& strPrototypeTag,void* pArg);
	class CGameObject* Clone_Prototype(const _wstring& strPrototypeTag,void* pArg);


	class CGameObject* Get_Prototype(_uint iLevelIndex,const _tchar* pLayerTag,const _wstring& strPrototypeTag);
	//	class CComponent* Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	void Clear(_uint iLevelIndex);
private:
	map<const _wstring,class CGameObject*>				m_Prototypes;

	/* 원형을 복제한 사본객체를 레벨별로 그룹지어 저장한다. */
	_uint												m_iNumLevels = {0};
	map<const _wstring,class CLayer*>*					m_pLayers = {nullptr};

public:
	void Set_KatanaState(_bool bKatana)	{
		m_bPlayer_Katana = bKatana;
	}
	_bool Get_KatanaState()				{
		return m_bPlayer_Katana;
	}

	void Set_PlayerPos(_float4 fPos) {
		m_fPlayerPos = fPos;
	}
	_float4 Get_PlayerPos() {
		return m_fPlayerPos;
	}

private:
	_bool  m_bPlayer_Katana{};
	_float4 m_fPlayerPos{};


public:
	class CGameObject* Find_Prototype(const _wstring& strPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex,const _wstring& strLayerTag);


public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END