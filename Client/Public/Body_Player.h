#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
END

BEGIN(Client)

class CBody_Player final : public CPartObject
{
public:
	typedef struct : CPartObject::PARTOBJECT_DESC
	{
		LEVELID m_eLevelID{};
		const _uint* pParentState_Upper = { nullptr };
		const _uint* pParentState_Lower = { nullptr };
		_bool* m_bAttackState = { nullptr };
		
	}BODY_PLAYER_DESC;


	enum PLAYER_ANIM {
		PLAYER_ANIM_Aim_Pistol_CC,
		PLAYER_ANIM_ArcadeDirectional_0,
		PLAYER_ANIM_ArcadeDirectional_E,
		PLAYER_ANIM_ArcadeDirectional_N,
		PLAYER_ANIM_ArcadeDirectional_S,
		PLAYER_ANIM_ArcadeDirectional_W,
		PLAYER_ANIM_batteryThrow1,
		PLAYER_ANIM_batteryThrowIdle,
		PLAYER_ANIM_DualDagger_Attack_1,
		PLAYER_ANIM_DualDagger_Attack_2,
		PLAYER_ANIM_FiringAnimation8_Base,
		PLAYER_ANIM_FlipEnd,
		PLAYER_ANIM_FlipStart,
		PLAYER_ANIM_Idle_Dual_Dagger,
		PLAYER_ANIM_Idle_Dual_Shotgun,
		PLAYER_ANIM_Idle_Katana,
		PLAYER_ANIM_idle_new01,
		PLAYER_ANIM_Idle_Pistol,
		PLAYER_ANIM_Idle_Rifle,
		PLAYER_ANIM_Idle_Shotgun,
		PLAYER_ANIM_idle_ToWeapon,
		PLAYER_ANIM_Idle_Unarmed,
		PLAYER_ANIM_IdleBadass_1,
		PLAYER_ANIM_Jump_End,
		PLAYER_ANIM_Jump_Loop,
		PLAYER_ANIM_Jump_Start,
		PLAYER_ANIM_NinjaAttack01_RootMotion,
		PLAYER_ANIM_NinjaFlip,
		PLAYER_ANIM_NinjaSweepAttack,
		PLAYER_ANIM_NinjaSwiftAttack_RootMotion,
		PLAYER_ANIM_NinjaSwiftAttack,
		PLAYER_ANIM_Run_E_Hopping,
		PLAYER_ANIM_Run_N_Dual_Daggers,
		PLAYER_ANIM_Run_N_Hopping,
		PLAYER_ANIM_Run_N_Katana,
		PLAYER_ANIM_Run_N_Rifle,
		PLAYER_ANIM_Run_NE_Dual_Daggers,
		PLAYER_ANIM_Run_NE_Hopping,
		PLAYER_ANIM_Run_NE_Katana,
		PLAYER_ANIM_Run_NE_Rifle,
		PLAYER_ANIM_Run_NE,
		PLAYER_ANIM_Run_NW_Dual_Daggers,
		PLAYER_ANIM_Run_NW_Hopping,
		PLAYER_ANIM_Run_NW_Katana,
		PLAYER_ANIM_Run_NW_Rifle,
		PLAYER_ANIM_Run_NW,
		PLAYER_ANIM_Run_N,
		PLAYER_ANIM_Run_S_Hopping,
		PLAYER_ANIM_Run_SE_Hopping,
		PLAYER_ANIM_Run_SW_Hopping,
		PLAYER_ANIM_Run_W_Hopping,
		PLAYER_ANIM_Supporter_Lobby01,
		PLAYER_ANIM_Supporter_Lobby02,
		PLAYER_ANIM_Supporter_Lobby03,
		PLAYER_ANIM_Walk_E_Dual_Daggers,
		PLAYER_ANIM_Walk_E_Katana,
		PLAYER_ANIM_Walk_E_Rifle,
		PLAYER_ANIM_Walk_E_Shotgun,
		PLAYER_ANIM_Walk_E,
		PLAYER_ANIM_Walk_N_Dual_Daggers,
		PLAYER_ANIM_Walk_N_Katana,
		PLAYER_ANIM_Walk_N_Rifle,
		PLAYER_ANIM_Walk_N_Shotgun,
		PLAYER_ANIM_Walk_NE_Dual_Daggers,
		PLAYER_ANIM_Walk_NE_Katana,
		PLAYER_ANIM_Walk_NE_Rifle,
		PLAYER_ANIM_Walk_NE_Shotgun,
		PLAYER_ANIM_Walk_NE,
		PLAYER_ANIM_Walk_NW_Dual_Daggers,
		PLAYER_ANIM_Walk_NW_Katana,
		PLAYER_ANIM_Walk_NW_Rifle,
		PLAYER_ANIM_Walk_NW_Shotgun,
		PLAYER_ANIM_Walk_NW,
		PLAYER_ANIM_Walk_N,
		PLAYER_ANIM_Walk_S_Dual_Daggers,
		PLAYER_ANIM_Walk_S_Katana,
		PLAYER_ANIM_Walk_S_Rifle,
		PLAYER_ANIM_Walk_S_Shotgun,
		PLAYER_ANIM_Walk_SE_Dual_Daggers,
		PLAYER_ANIM_Walk_SE_Katana,
		PLAYER_ANIM_Walk_SE_Rifle,
		PLAYER_ANIM_Walk_SE_Shotgun,
		PLAYER_ANIM_Walk_SE,
		PLAYER_ANIM_Walk_SW_Dual_Daggers,
		PLAYER_ANIM_Walk_SW_Katana,
		PLAYER_ANIM_Walk_SW_Rifle,
		PLAYER_ANIM_Walk_SW_Shotgun,
		PLAYER_ANIM_Walk_SW,
		PLAYER_ANIM_Walk_S,
		PLAYER_ANIM_Walk_W_Dual_Daggers,
		PLAYER_ANIM_Walk_W_Katana,
		PLAYER_ANIM_Walk_W_Rifle,
		PLAYER_ANIM_Walk_W_Shotgun,
		PLAYER_ANIM_Walk_W,
		PLAYER_ANIM_WeaponReload
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
		BATTERY = 11,
		WEAPON_END
	};

	enum UPPERBODY_STATE
	{
		KATANA_ATTACK1 = 0x00000001,
		KATANA_ATTACK2 = 0x00000002,
		BATTERY_HOLD = 0x00000004,
		BATTERY_THROW = 0x00000008,
		RIFLE_FIRE = 0x00000010,
		RIFLE_CLOSE_ATTACK = 0x00000020
	};

	enum ATTACK_MOTION
	{
		IDLE_MOTION,
		ATTACK_FIRE_MOTION,
		ATTACK_KATANA_MOTION,
		ATTACK_MELEE_MOTION,
		RELOAD_MOTION,
		IDLE_KATANA_MOTION,
		RIFLE_FIRE_MOTION,
		SHOTGUN_FIRE_MOTION,
		PULSECANNON_FIRE_MOTION,
		TELEPORTGUN_FIRE_MOTION,
		LOCKETLAUNCHER_FIRE_MOTION,
	};
private: 
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player(const CBody_Player& Prototype);
	virtual ~CBody_Player() = default;

public:
	const _float4x4* Get_SocketMatrix(const _char* pBoneName);

public:
	virtual HRESULT Initialize_Prototype() override;

	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
	
public:
	void UpperBody_Anim(_float fTimeDelta);
	void LowerBody_Anim(_float fTimeDelta);

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	_bool	m_bAnimInit = false;		// 애니메이션 초기화용
	_bool	m_bAnimState = false;		// 하체 애니메이션 끝났는지 확인
	_bool	m_bUpperAnimState = false;	// 상체 애니메이션 끝났는지 확인
	_bool	m_bTPSState = false;		// TPS인지 
	_bool	m_bShoot_State = false;		// Idle 상태를 위한 bool 함수, Idle상태 = 쏘는 자세 첫 자세
	_bool   m_bRunState = false;		// 뛸 때 애니메이션 속도 다르게

	_float3					Rotation{};
	LEVELID m_eLevelID{};

public:
	_uint* Get_UpperMotion() {	return &m_iUpperMotion;	}
private:
	_uint	m_iUpperMotion = 0;			// 상체 모션, 애니메이션 마다 본 위치나 currentPosition이 달라짐


	_uint	m_iShaderPassNum = 0;		// 1인칭일 때 벽을 뚫어도 몸이 보이게 
	_uint	m_iWeaponState = 0;			// 어떤 무기를 들고 있는지, 0이 Idle, 1이 공격, 2가 Katana 공격

	_float	m_fArmAngle{};
private:		
	_uint   m_iJumpState = 0;	// 점프 상태
	_float	m_fHeight{};		// 점프 높이
	_float  m_fMinHeight = 0.f;
	_float	m_fPower{};			// 점프 힘

private:
		const _uint* m_pParentState_Upper = { nullptr };
		const _uint* m_pParentState_Lower = { nullptr };
		_uint*		 m_iViewState		  = { nullptr };	// 1인칭인지 3인칭인지


public:// 점프
	_uint Get_JumpState() { return m_iJumpState; }
	void  Set_JumpState(_float& fHeight, _float& fPowr, _float& minHeight) {
		m_fHeight = fHeight;
		m_fPower = fPowr;
		m_fMinHeight = minHeight;
	}


	_bool	Get_UpperBody_AnimState() { return m_bUpperAnimState; }
public:
	void	Set_PlayerViewState(_bool bTPS) { m_bTPSState = bTPS; }
	void	Set_WeaponState(_uint iState) { m_iWeaponState = iState; }


	// 총 딜레이용
private:
	_float					m_fRiflrDelay = 0.1f;
	_float					m_fShotGunDelay = 1.f;
	_float					m_fPulseCannonDelay = 2.f;
	_float					m_fTeleportDelay = 2.f;
	_float					m_fLocketLauncherDelay = 0.6f;
	_float					m_fCurrentDelay = 0.f;

	_bool					m_bShotStart = false;  // 쏘는 시작을 알려줌 (이때 총알 발사와 반동)
	_bool					m_bShotNow = false;		// 애니메이션 작동의 시작과 끝을 알려줌 ( 딜레이 계산)
	_bool					m_bTemp = false;
	_bool*					m_bAttackState = { nullptr }; // 공격 상태인지 아닌지 체크
public:

	_bool*					Get_ShotNow()	{ return &m_bShotNow; }
	_bool*					Get_ShotStart() { return &m_bShotStart; }
private:
	WEAPONSTATE				m_eWeapon{}; // 스위치문 편하게 만드려고 임시 생성

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END