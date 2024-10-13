#pragma once

#include "Transform.h"

/* 프로토타입을 통해 객체를 생성한다. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		_uint			iData = {};
	}GAMEOBJ_DESC;
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	/* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
	virtual HRESULT Initialize_Prototype();

	/* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
	/* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한ㄴ다. */
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	 

public:
	bool IsValid() const { return !m_bDead; }
	bool Get_Dead() { return m_bDead; }
	void Set_Dead() { m_bDead = true; }


public:
	_float3	Get_PickingPos() { return m_fPickingPos; }

	class CComponent* Find_Component(const _wstring& strComponentTag);


protected:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	class CTransform* m_pTransformCom = { nullptr };

protected:
	map<const _wstring, class CComponent*>			m_Components;

protected:
	_uint							m_iData = {};
	_float3							m_fPickingPos{};
	_bool							m_bDead = false;
	_vector							m_vecPosition{};
protected:
	HRESULT Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;



};

END