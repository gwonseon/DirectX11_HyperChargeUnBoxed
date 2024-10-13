#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CBody_Player final : public CPartObject
{
public:
	typedef struct : CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState_Upper = { nullptr };
		const _uint* pParentState_Lower = { nullptr };

		
	}BODY_PLAYER_DESC;


	enum PLAYER_ANIM {
		PLAYER_ANIM_Aim_Pistol_CC,
		PLAYER_ANIM_ArcadeDirectional_0,
		PLAYER_ANIM_ArcadeDirectional_E,
		PLAYER_ANIM_ArcadeDirectional_N,
		PLAYER_ANIM_ArcadeDirectional_S,
		PLAYER_ANIM_ArcadeDirectional_W,
		PLAYER_ANIM_FiringAnimation8_Base,
		PLAYER_ANIM_Idle_Dual_Dagger,
		PLAYER_ANIM_Idle_Dual_Shotgun,
		PLAYER_ANIM_Idle_Katana,
		PLAYER_ANIM_Idle_Pistol,
		PLAYER_ANIM_Idle_Rifle,
		PLAYER_ANIM_Idle_Shotgun,
		PLAYER_ANIM_Idle_Unarmed,
		PLAYER_ANIM_Jump_End,
		PLAYER_ANIM_Jump_Loop,
		PLAYER_ANIM_Jump_Start,
		PLAYER_ANIM_Pistol_Reload,
		PLAYER_ANIM_Run_N_Dual_Daggers,
		PLAYER_ANIM_Run_N_Katana,
		PLAYER_ANIM_Run_N_Rifle,
		PLAYER_ANIM_Run_NE_Dual_Daggers,
		PLAYER_ANIM_Run_NE_Katana,
		PLAYER_ANIM_Run_NE_Rifle,
		PLAYER_ANIM_Run_NE,
		PLAYER_ANIM_Run_NW_Dual_Daggers,
		PLAYER_ANIM_Run_NW_Katana,
		PLAYER_ANIM_Run_NW_Rifle,
		PLAYER_ANIM_Run_NW,
		PLAYER_ANIM_Run_N,
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
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player(const CBody_Player& Prototype);
	virtual ~CBody_Player() = default;

public:
	const _float4x4* Get_SocketMatrix(const _char* pBoneName);

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
	void UpperBody_Anim(_float fTimeDelta);
	void LowerBody_Anim(_float fTimeDelta);

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:

	_bool	m_bAnimState = false;
	_bool	m_bAnimInit = false;
	_bool	m_bUpperAnimState = false;
	_bool	m_bTPSState = false;

	_uint	m_iWeaponState = 0;

	_float	m_fArmAngle{};
private:		
	_uint   m_iJumpState = 0;	// 점프 상태
	_float	m_fHeight{};		// 점프 높이
	_float	m_fPower{};			// 점프 힘

private:
		const _uint* m_pParentState_Upper = { nullptr };
		const _uint* m_pParentState_Lower = { nullptr };
		_uint*		 m_iViewState		  = { nullptr };


public:// 점프
	_uint Get_JumpState() { return m_iJumpState; }
	void  Set_JumpState(_float& fHeight, _float& fPowr) {
		m_fHeight = fHeight;
		m_fPower = fPowr;
	}


	_bool Get_UpperBody_AnimState() { return m_bUpperAnimState; }
public:
	void	Set_PlayerViewState(_bool bTPS) { m_bTPSState = bTPS; }


public:
	void Set_WeaponState(_uint iState) { m_iWeaponState = iState; }
	
private:
	WEAPONSTATE m_eWeapon{}; // 스위치문 편하게 만드려고

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END