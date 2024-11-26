#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include <TruckShooter.h>
#include "Tracker.h"
#include "Explosion.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CTruck_Missile final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		LEVELID eID = {};
		_bool* bShotStart = { nullptr };
		_uint* ShooterModelIdx = { nullptr };
		CTracker* m_pTracker = { nullptr };
		CPlayer* pPlayer = { nullptr };
	}MISSILE_DESC;

	enum MISSILE_STATE { MISSILE_IDLE, MISSILE_SHOT_START, MISSILE_SHOT_ACCEL, MISSILE_SHOT_FALL, MISSILE_BOMB, MISSILE_END };
private:
	CTruck_Missile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTruck_Missile(const CTruck_Missile& Prototype);
	virtual ~CTruck_Missile() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Height();
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };


private:
	LEVELID m_eLevel{};
	MISSILE_STATE m_eMissile_State{};

	_float fRotX{};
	_float m_fAngle = 0.f;
	_float m_fPower = 0.f;

	_float m_fSpeed = 0.f;

	_bool m_bStart_Shoot = false;
	_bool m_bMoving = false;
	_bool m_bMidArrived = false;
	_bool m_bFog = false;
	_float m_fFogEnd = 1.2f;

	_uint* m_pShooterModelIdx = { nullptr };
	_bool* m_bShotStart = { nullptr };
	CTracker* m_pTracker = { nullptr };
	CPlayer* m_pPlayer = { nullptr };

	_vector PrevPos{};
	_vector MidPos{};
	_vector StartPos{};
	_vector StartPos_Store{};

	vector<_float3> m_vecStartingPos;

public:
	static CTruck_Missile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END