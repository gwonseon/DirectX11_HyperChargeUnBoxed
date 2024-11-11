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


BEGIN(Engine)

class CNavigation;
END

BEGIN(Client)

class CPlayer final : public CContainerObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		LEVELID m_eLevelID{};
		_vector* vCameraAt = {};
		_vector* vCameraPos = {};
		_uint* iRound = {};
		_uint iCellIdx{};
	}PLAYER_DESC;


public:
	enum TPS_PARTOBJID { TPS_PART_BODY, TPS_PART_WEAPON, TPS_PART_EFFECT, TPS_PART_HEAD, TPS_PART_PIVOT, TPS_PART_KATANA, FPS_PART_BODY, FPS_PART_PIVOT, PART_END };
	enum TPSSTATE {
		STATE_IDLE = 0x00000001,
		WALKSTATE_NORTH = 0x00000002,
		WALKSTATE_SOUTH = 0x00000004,
		WALKSTATE_EAST = 0x00000008,
		WALKSTATE_WEST = 0x00000010,
		WALKSTATE_NORTHEAST = 0x00000020,
		WALKSTATE_NORTHWEST = 0x00000040,
		WALKSTATE_SOUTHEAST = 0x00000080,
		WALKSTATE_SOUTHWEST = 0x00000100,
		JUMP_START = 0x00000200,
		JUMP_LOOP = 0x00000400,
		JUMP_END = 0x00000800,
		RUNSTATE_NORTH = 0x00001000,
		RUNSTATE_NORTHWEST = 0x00002000,
		RUNSTATE_NORTHEAST = 0x00004000,
		RELOADING = 0x00008000,
		FIRE = 0x00010000,
		FIRE_RB = 0x00020000,
		MELEE = 0x00040000,
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
		BATTERY = 11,
		TRACKER,
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
	void Set_Dir(_vector vDir) { m_pTransformCom->Set_State(CTransform::STATE_LOOK, vDir); }
	_vector Get_Dir() { return m_pTransformCom->Get_State(CTransform::STATE_LOOK); }
	void		Get_Rotation(_vector& vRight, _vector& vUp, _vector& vLook) {
		vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
		vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	}
	_uint* Get_ViewState() { return &m_iViewState; }
	void   Set_CameraAt(_vector* pAt) { m_vecCameraAt = pAt; }
	void   Set_CameraPos(_vector* pPos) { m_vecCameraPos = pPos; }
	void   Set_Rotaion(_vector	vRight, _vector	vUp, _vector	vLook) {
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
		m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	}
	_float	Get_Rotation_Value() { return m_fRotation_Value; }
	void Set_Rotation(_float fAngleY) { m_pTransformCom->Rotation(0.f, fAngleY, 0.f); }
	void Set_RoundStart(_bool* bStart) { m_bRoundStart = bStart; }


	CTransform* Get_Transform() { return m_pTransformCom; }
	_vector* Get_TPSPosptr() { return m_vecTPS_CamPos; }
	_vector* Get_FPSPosptr() { return m_vecFPS_CamPos; }
	_vector* Get_WeaponPos() { return m_vecWeaponPos; }
	_vector* Get_WeaponDir() { return m_vecWeaponDir; }
	_bool* Get_ShotNow() { return m_pBody->Get_ShotNow(); }
	_bool* Get_ShotStart() { return m_pBody->Get_ShotStart(); }
	_uint* Get_WeaponState() { return &m_iWeaponState; }
	_uint* Get_UpperMotion() { return m_pBody->Get_UpperMotion(); }
	_bool* Get_Reloading() { return &m_bReloading; }


private: // Camera 
	_vector* m_vecTPS_CamPos{};
	_vector* m_vecFPS_CamPos{};
	_vector* m_vecCameraAt{};
	_vector* m_vecCameraPos{};

	_vector* m_vecWeaponPos{};
	_vector* m_vecWeaponDir{};
	_uint	m_iViewState{};

private:
	_uint					m_iState_Upper = {};
	_uint					m_iState_Lower = {};
	_bool					m_bJumpStart = false;
	_uint					m_iJumpCount = 0;
private:
	_bool					m_bKey_A = false;
	_bool					m_bKey_W = false;
	_bool					m_bKey_D = false;
	_bool					m_bKey_S = false;
	_bool					m_bKey_Shift = false;
	_bool					m_bKey_R = false;
	_bool					m_bReloading = false;
	float					m_fRun_FourDirection{};
	float					m_fRun_EightDirection{};
	_float					m_fReload_Charging = 0.f;

#pragma region  빌드 모드 
private:
	_bool					m_bBuildMode = false;		// 빌드 모드
	_bool					m_bBuild_Gauging = false;  // E 눌러서 빌드 중임을 알려주는 변수
	_bool					m_bCharging = false;
	_bool					m_bBuild_Able = false;

	_bool* m_bRoundStart = { nullptr }; // 빌드 모드가 끝나고 라운드가 시작했음을 알리는  포인터
	_uint* m_iRound = { nullptr };
public:
	void	Set_Build_Able(_bool bAble) { m_bBuild_Able = bAble; }

	_bool* Get_BuildMode() { return &m_bBuildMode; }
	_bool	Get_Build_Gauging() { return m_bBuild_Gauging; }

#pragma endregion  빌드 모드 

public:  // 총알
	_uint* Get_CurrentBullet() { return m_pWaepon->Get_CurrentBullet(); }
	_uint* Get_FullBullet() { return m_pWaepon->Get_FullBullet(); }

public:
	_float m_fRotation_Value{};

private:
	CWeapon* m_pWaepon = nullptr;
	CBody_Player* m_pBody = nullptr;
	CWeapon_Katana* m_pKatana = nullptr;
	CHead_Player* m_pHead = nullptr;
	CNavigation* m_pNavigationCom = nullptr;

private:
	_float	m_fHeight{};		// 점프 높이
	_float m_fPower{};			// 점프 힘
	_float	m_fInvincibleTime{}; // 무적시간

	_vector	m_vecPos{}, m_vecDir{}, m_vecDir2{};

private:
	_float					m_fMouseSensor = { 0.f };
	_vector					m_vecPivotPos{};

	_uint					m_iWeaponState = WEAPON_RIFLE;
	_uint					m_iCellidx = 0;
	LEVELID m_eLevelID{};
private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	HRESULT Bind_ShaderResources();


private:
	void Player_Movement(_float fTimeDelta);

public:
	_vector Get_Position() { return m_vecPos; }
	_vector Get_PivotPostion() { return m_vecPivotPos; }
	_uint	Get_CurrentCellIdx() { return m_pNavigationCom->Get_CurrentCell_Index(); }
	void	Set_EquipNumber(_uint iEquipNum) { m_iWeaponState = iEquipNum; }


	_bool	Set_Charging(_bool bCharge) { m_bCharging = bCharge; }

#pragma region 배터리
public:
	void	PickUp_Battery(_uint iEquipNum)
	{
		m_iPrev_WeaponState = m_iWeaponState; // 지금 들고 있는 무기를 저장해둠, 나중에 건전지 내려놓았을 때 이거 다시 들어야함
		m_iWeaponState = iEquipNum; // 무기 배터리로 변경
	}
	void	Insert_Battery()
	{
		m_iWeaponState = m_iPrev_WeaponState;
	}
	_vector* Get_BatteryPos() { return &m_vecBatteryPos; }
	_bool* Get_Visible_Battery() { return &m_bVisible_Battery; }

	_vector* Get_TrackerPos() { return &m_vecTrackerPos; }
	_bool* Get_Visible_Tracker() { return &m_bVisible_Tracker; }

	void	Set_Explosion(_bool bExplo) { m_bMissile_Explosion = bExplo; }
	_bool	Get_Explosion() { return m_bMissile_Explosion; }
private:
	_uint					m_iPrev_WeaponState = WEAPON_RIFLE;
	_vector					m_vecBatteryPos{};
	_vector					m_vecTrackerPos{};
	_bool					m_bVisible_Battery = true;
	_bool					m_bVisible_Tracker = false;
	_bool					m_bMissile_Explosion = false;
#pragma endregion 배터리

public:
	_float* Get_PlayerHP() { return &m_fHp; }
	_float* Get_PlayerEnergy() { return &m_fEnergy; }

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END