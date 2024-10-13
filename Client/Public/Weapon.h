#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon final : public CPartObject
{
public:
	typedef struct : CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };
		const _float4x4* pSocketMatrix = { nullptr };


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
		WEAPON_END
	};

private:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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


public:
	void Set_WeaponState(_uint iState) { m_iWeaponState = iState; }
	HRESULT Weapon_Exchange();

	void	Set_SocketMatrix(const _float4x4* matSocket) { m_pSocketMatrix = matSocket; }

	void   Set_CameraAt(_vector* pAt) { m_vecCameraAt = pAt; }


private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom[WEAPON_EA] = { nullptr };


	const _float4x4* m_pSocketMatrix = { nullptr };
	const _uint* m_pParentState = { nullptr };
	_float Scale{};
	_float3 Position{};
	_float3	Rotation{};

	_uint m_iWeaponState{};
	WEAPONSTATE m_eWeaponState = WEAPON_END;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	_uint* m_iViewState{};

	_vector* m_vecCameraAt{};
public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END