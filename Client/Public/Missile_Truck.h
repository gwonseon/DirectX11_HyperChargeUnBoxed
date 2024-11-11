#pragma once
#include "Client_Defines.h"
#include "ContainerObject.h"

#include "GameInstance.h"
#include "Truck_Missile.h"
#include "TruckShooter.h"
#include "Tracker.h"
#include "TruckBody.h"
#include "Player.h"

BEGIN(Engine)
class CCollider;
class CLayer;
END 

BEGIN(Client)

class CMissile_Truck final : public CContainerObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		LEVELID m_eLevelID{};
		_uint*		iRound		= {nullptr};
		CPlayer*	pPlayer		= { nullptr };
	}MISSILETRUCK_DESC;

	enum MISSILETRUCK_PARTOBJID { MISSILETRUCK_BODY, MISSILETRUCK_SHOOTER, MISSILETRUCK_END};

private:
	CMissile_Truck(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMissile_Truck(const CMissile_Truck& Prototype);
	virtual ~CMissile_Truck() = default;

public:
	/* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
	virtual HRESULT Initialize_Prototype() override;

	/* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
	/* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한ㄴ다. */
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	

public:
	CCollider* Get_Collider() { return m_pColliderCom; }

private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	HRESULT Bind_ShaderResources();

public:
	_float* Get_Timer() { return (m_pTracker->Get_Timer()); }
	CTracker* Get_Tracker() { return m_pTracker; }

private:
	CTruckShooter*	m_pShooter		=	{ nullptr };
	CTracker*		m_pTracker		=	{ nullptr };
	CTruckBody*		m_pTruckBody	=	{ nullptr };
	CTruck_Missile* m_pMissile		=	{ nullptr };
	CPlayer*		m_pPlayer		=	{ nullptr };
	CCollider*		m_pColliderCom	=	{ nullptr };
	CLayer*			pTruck = { nullptr };
	_uint*			m_iRound		=	{ nullptr };

private:
	LEVELID m_eLevelID{};
	_float3 m_fPos{}, m_fScale{};
	


public:
	static CMissile_Truck* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};

END