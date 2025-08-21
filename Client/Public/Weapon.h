#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon final: public CPartObject
{
public:
	typedef struct: CPartObject::PARTOBJECT_DESC
	{
		LEVELID m_eLevelID{};
		const _uint* pParentState = {nullptr};
		const _float4x4* pSocketMatrix = {nullptr};
		_vector* vCameraAt = {nullptr};
		_vector* vCameraPos = {nullptr};
		_bool* bShotStart{}; // 사격 시작 타이밍
		_bool* bReload{}; // 장전
		_float* fReloadingTime{};
		_float3* vTargetPos{};
	}WEAPON_DESC;


	enum WEAPON_INDEX_LIST
	{
		WEAPONPARTS_BASE,
		WEAPONPARTS_CAP,
		WEAPONPARTS_RIFLE,
		WEAPONPARTS_SHOTGUN,
		WEAPONPARTS_PULSE,
		WEAPONPARTS_TELEPORT,
		WEAPONPARTS_LOCKETLAUNCHER,
		WEAPONPARTS_RIFLE_SECOND,
		WEAPONPARTS_KATANA,
		WEAPONPARTS_FLASHLIGHT,
		WEAPONPARTS_LOCKMISSILE,
		WEAPONPARTS_END
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
	CWeapon(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& Prototype);
	virtual ~CWeapon() = default;

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
	virtual HRESULT Render_Shadow() override;

public:
	void	Set_TPSState(_bool State) {
		m_bTPSState = State;
	}
	void	Set_BulletIn(_bool bIn) {
		m_bBulletIn = bIn;
	}
	void	Set_WeaponState(_uint iState) {
		m_iWeaponState = iState;
		switch(m_iWeaponState)
		{
		case Client::CWeapon::WEAPON_UNARMED:
		break;
		case Client::CWeapon::WEAPON_RIFLE:
		m_iCurrent_Bullet = m_iRifle_Bullet;
		m_iFull_Bullet = m_iRifle_Bullet;
		break;
		case Client::CWeapon::WEAPON_SHOTGUN:
		break;
		case Client::CWeapon::WEAPON_PULSECANNON:
		break;
		case Client::CWeapon::WEAPON_TELEPORT:
		break;
		case Client::CWeapon::WEAPON_LOCKETLAUNCHER:
		m_iCurrent_Bullet = m_iLocket_Bullet;
		m_iFull_Bullet = m_iLocket_Bullet;
		break;
		case Client::CWeapon::WEAPON_RIFLE_SECOND:
		break;
		case Client::CWeapon::WEAPON_KATANA:
		break;
		case Client::CWeapon::BATTERY:
		break;
		case Client::CWeapon::TRACKER:
		break;
		case Client::CWeapon::WEAPON_END:
		break;
		default:
		break;
		}
	}
	HRESULT Weapon_Exchange();

	void	Set_SocketMatrix(const _float4x4* matSocket) {
		m_pSocketMatrix = matSocket;
	}

	void   Set_CameraAt(_vector* pAt) {
		m_vecCameraAt = pAt;
	}
	void   Set_CameraPos(_vector* pPos) {
		m_vecCameraPos = pPos;
	}

	_vector*	Get_WeaponPos() {
		return &m_vecWeaponPos;
	}
	_vector*	Get_WeaponDir() {
		return &m_vecWeaponDir;
	}


public:
	_uint*		Get_FullBullet() {
		return &m_iFull_Bullet;
	}
	_uint*		Get_CurrentBullet() {
		return &m_iCurrent_Bullet;
	}

private:
	CShader* m_pShaderCom = {nullptr};
	CModel* m_pModelCom[WEAPON_EA] = {nullptr};


	const _float4x4* m_pSocketMatrix = {nullptr};
	const _uint* m_pParentState = {nullptr};

	_float Scale{};
	_float fPos{};

	_float3 Position{};
	_float3	Rotation{};

	_vector	m_vecWeaponPos{};
	_vector	m_vecWeaponDir{};
	_vector m_vecFlarePos{};
	_uint m_iShaderPassNum{};  // 무기가 벽에 겹쳐도 보이게
	_uint m_iWeaponState{};
	WEAPONSTATE m_eWeaponState = WEAPON_END;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	_uint* m_iViewState{};
	_vector* m_vecCameraAt{};
	_vector* m_vecCameraPos{};
	_float3* m_vecTargetPos{};

	_bool* m_bShotStart = {nullptr};
	_bool* m_bReloading = {nullptr};

	_float* m_pReloading_Time = {nullptr};
	// 총알
private:
	_uint m_iRifle_Bullet = 30;
	_uint m_iLocket_Bullet = 5;

	_uint m_iFull_Bullet{};
	_uint m_iCurrent_Bullet = 30;

private:

	LEVELID m_eLevelID{};
	_bool		m_bTPSState{};
	_bool		m_bBulletIn = false;

	_float		m_fAngle_Y{};
public:
	static CWeapon* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END