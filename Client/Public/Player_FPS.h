#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPlayer_FPS: public CPartObject
{
public:
	typedef struct: CPartObject::PARTOBJECT_DESC
	{
		LEVELID m_eLevelID{};
		const _uint* pParentState = {nullptr};

	}FPS_PLAYER_DESC;

	enum FPS_ANIM
	{
		FPS_Chopper_Holster,
		FPS_Chopper_Idle,
		FPS_Chopper_Sprint,
		FPS_Chopper_Walk,
		FPS_DualDagger_Attack_1,
		FPS_DualDagger_Attack_2,
		FPS_DualDagger_Idle,
		FPS_DualDagger_SprintAdd,
		FPS_DualDagger_Sprint,
		FPS_DualDagger_Walk,
		FPS_FP_Evil_Idle1,
		FPS_FP_EvilCharge1,
		FPS_FP_EvilFire7,
		FPS_FP_EvilMeleeLeft1,
		FPS_FP_EvilSprint1,
		FPS_FP_EvilWalking1,
		FPS_Katana_Attack1,
		FPS_Katana_Attack2,
		FPS_Katana_Holster,
		FPS_Katana_Idle,
		FPS_Katana_Sprint,
		FPS_Katana_Walk,
		FPS_PowerRifle_Fire1,
		FPS_Rifle_Fire,
		FPS_Rifle_Idle,
		FPS_Rifle_Reload,
		FPS_Rifle_Sprint,
		FPS_Rifle_Walk,
		FPS_Smoker_Draw,
		FPS_Smoker_Fire,
		FPS_Smoker_Reload,
		FPS_TempVault,
		FPS_Unarmed_Idle,
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
	CPlayer_FPS(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CPlayer_FPS(const CPlayer_FPS& Prototype);
	virtual ~CPlayer_FPS() = default;

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
	void	Set_PlayerViewState(_bool bFPS) {
		m_bFPSState = bFPS;
	}


public:



private:
	CShader* m_pShaderCom = {nullptr};
	CModel* m_pModelCom = {nullptr};


private:
	const _uint* m_pParentState = {nullptr};
	_bool	m_bAnimState = false;
	_bool	m_bAnimInit = false;

	_bool	m_bFPSState = false;

	_uint	m_iWeaponState = 0;
	WEAPONSTATE m_eWeapon{}; // 스위치문 편하게 만드려고
	LEVELID m_eLevelID{};

	// 점프
public:
	_uint Get_JumpState() {
		return m_iJumpState;
	}
	void  Set_JumpState(_float& fHeight,_float& fPowr) {
		m_fHeight = fHeight;
		m_fPower = fPowr;
	}


	_float3 Position{},Rotation{};
	float	Scale{};

public:
	void Set_WeaponState(_uint iState) {
		m_iWeaponState = iState;
	}
private:
	_uint   m_iJumpState = 0;	// 점프 상태

private:
	_float	m_fHeight{};		// 점프 높이
	_float	m_fPower{};			// 점프 힘




private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CPlayer_FPS* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END