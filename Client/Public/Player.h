#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"
#include "GameInstance.h"
#include "Body_Player.h"
#include "Weapon.h"
#include "Head_Player.h"
#include "Pivot.h"
#include "Weapon_Katana.h"
#include "Weapon_Item.h"
#include "Player_FPS.h"

BEGIN(Engine)

END

BEGIN(Client)

class CPlayer final : public CContainerObject
{
public:
	enum TPS_PARTOBJID { TPS_PART_BODY, TPS_PART_WEAPON, TPS_PART_EFFECT, TPS_PART_HEAD, TPS_PART_PIVOT , TPS_PART_KATANA, FPS_PART_BODY, FPS_PART_PIVOT, PART_END };
	enum TPSSTATE {
		STATE_IDLE					= 0x00000001,
		WALKSTATE_NORTH				= 0x00000002,
		WALKSTATE_SOUTH				= 0x00000004,
		WALKSTATE_EAST				= 0x00000008,
		WALKSTATE_WEST				= 0x00000010,
		WALKSTATE_NORTHEAST			= 0x00000020,
		WALKSTATE_NORTHWEST			= 0x00000040,
		WALKSTATE_SOUTHEAST			= 0x00000080,
		WALKSTATE_SOUTHWEST			= 0x00000100,
		JUMP_START					= 0x00000200,
		JUMP_LOOP					= 0x00000400,
		JUMP_END					= 0x00000800,
		RUNSTATE_NORTH				= 0x00001000,
		RUNSTATE_NORTHWEST			= 0x00002000,
		RUNSTATE_NORTHEAST			= 0x00004000,
		RELOADING					= 0x00008000,
		FIRE						= 0x00010000
	};

	enum TPS_JUMPSTATE
	{
		LANDING_STATE,
		JUMPING_START_STATE,
		JUMPING_LOOP_STATE,
		JUMPING_END_STATE
	};
	enum WEAPONSTATE
	{
		WEAPON_UNARMED,
		WEAPON_RIFLE,
		WEAPON_SHOTGUN,
		WEAPON_PULSECANNON,
		WEAPON_TELEPORT,
		WEAPON_LOCKETLAUNCHER,
		WEAPON_RIFLE_SECOND,
		WEAPON_KATANA,
		WEAPON_END
	};
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

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
	void Set_Dir(_vector vDir) { m_pTransformCom->Set_State(CTransform::STATE_LOOK,vDir); }
	_vector Get_Dir() { return m_pTransformCom->Get_State(CTransform::STATE_LOOK); }
	void		Get_Rotation(_vector& vRight, _vector& vUp, _vector& vLook) {
		vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
		vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	}
	_uint* Get_ViewState() { return &m_iViewState; }
	void   Set_CameraAt(_vector* pAt) {	m_vecCameraAt = pAt;}

	void		Set_Rotaion(_vector	vRight, _vector	vUp, _vector	vLook) {
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
		m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	}
	_float	Get_Rotation_Value() {return m_fRotation_Value;	}
	void Set_Rotation(_float fAngleY) { m_pTransformCom->Rotation(0.f, fAngleY, 0.f); }
	


	CTransform* Get_Transform() {	return m_pTransformCom; }
	_vector* Get_TPSPosptr()	{	return m_vecTPS_CamPos;	}
	_vector* Get_FPSPosptr()	{	return m_vecFPS_CamPos; }

private:
	_vector* m_vecTPS_CamPos{};
	_vector* m_vecFPS_CamPos{};
	_vector* m_vecCameraAt{};

	_uint	m_iViewState{};

private:
	_uint					m_iState_Upper = {};
	_uint					m_iState_Lower = {};
	_bool					m_bJumpStart = false;
	

private:
	_bool					m_bKey_A = false;
	_bool					m_bKey_W = false;
	_bool					m_bKey_D = false;
	_bool					m_bKey_S = false;
	_bool					m_bKey_Shift = false;
	_bool					m_bKey_R = false;


public:
	_float m_fRotation_Value{};

private:
	CWeapon* m_pWaepon = nullptr;
	CBody_Player* m_pBody = nullptr;
	CWeapon_Katana* m_pKatana = nullptr;
	CPlayer_FPS* m_pFPS = nullptr;
	CHead_Player* m_pHead = nullptr;

private:
	_float	m_fHeight{};		// 점프 높이
	_float m_fPower{};			// 점프 힘


	_vector	m_vecPos{}, m_vecDir{}, m_vecDir2{};

private:
	_float					m_fMouseSensor = { 0.f };
	_vector					m_vecPivotPos{};

	_bool					m_bCharging = false;


	_uint					m_iWeaponState = WEAPON_RIFLE;

private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	HRESULT Bind_ShaderResources();


private:
	void Player_Movement(_float fTimeDelta);

public:
	_vector Get_Position() { return m_vecPos; }

	_vector Get_PivotPostion() { return m_vecPivotPos; }

	void	Set_EquipNumber(_uint iEquipNum) { m_iWeaponState = iEquipNum; }
	_bool	Set_Charging(_bool bCharge) { m_bCharging = bCharge; }

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END