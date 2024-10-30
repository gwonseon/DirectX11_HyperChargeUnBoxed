#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Player.h"

BEGIN(Client)

class CPlayer_Build : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		LEVELID eID = {};
		CPlayer* pPlayer = { nullptr };
		_uint iModel_Idx{};

	}PLAYER_BUILD_DESC;

protected:
	CPlayer_Build(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Build(const CPlayer_Build& Prototype);
	virtual ~CPlayer_Build() = default;

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
	CTransform* Get_Transform() { return m_pTransformCom; }


protected:
	LEVELID	m_eLevel = {};
	_int	m_iModelIndex = {};

	_bool	m_bDraw = false;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;


};

END