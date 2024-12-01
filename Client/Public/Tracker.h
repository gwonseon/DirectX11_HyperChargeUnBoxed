#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Player.h"
#include "InGameUI.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CTracker final : public CGameObject
{

public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		LEVELID eID = {};
		_uint* iRound = {};
		_uint* ShooterModelIdx = { nullptr };

		CPlayer* pPlayer = { nullptr };
	}TRACKER_DESC;

	enum TRACKER_STATE { TRACKER_IDLE, TRACKER_TURN_ON, TRACKER_TURN_OFF, _END};
private:
	CTracker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTracker(const CTracker& Prototype);
	virtual ~CTracker() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	_bool* Get_Shot() { return &m_bStart_Shot; }
	CTransform* Get_Transform() { return m_pTransformCom; }
	_float* Get_Timer() { return &m_fTimer; }
	_bool** Get_PickUp() { return &m_bPickUp_Player; }

	void		Set_Fall(_bool bFall) { m_bMissile_Fall = bFall; }
	_vector		Get_Pos(){ return m_vecPosition; }

	TRACKER_STATE Get_TrackerState() { return m_eTrackerState; }

	
private:
	CShader*	m_pShaderCom	= { nullptr };
	CModel*		m_pModelCom		= { nullptr };
	CPlayer*	m_pPlayer		= { nullptr };

	_uint* m_iRound = {};
private:
	LEVELID m_eLevel{};
	TRACKER_STATE m_eTrackerState{};

	_float m_fTimer{};  // 미사일 전체 타이머
	_float m_fMissileTimer{};  // 미사일 폭발 타이머
	_float m_fPickUpTimer{};	// 플레이어가 줍는 시간


	_bool m_bStart_Shot = false;
	_bool m_bOnce = false;
	_bool* m_bPickUp_Player = { nullptr };
	_bool m_bMissile_Fall = false;

	_uint* m_pShooterModelIdx = { nullptr };

	_float m_fFallingSpeed = 10.f;

public:
	static CTracker* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END