#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CHead_Player final : public CPartObject
{
public:
	typedef struct : CPartObject::PARTOBJECT_DESC
	{
		LEVELID m_eLevelID{};
		const _uint* pParentState = { nullptr };
		const _float4x4* pSocketMatrix = { nullptr };
		_uint* m_iWeaponState{};
	}HEADPLAYER_DESC;

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
	CHead_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHead_Player(const CHead_Player& Prototype);
	virtual ~CHead_Player() = default;

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
	void	Set_PlayerViewState(_bool bTPS) { m_bTPSState = bTPS;	}

private:
	_bool m_bTPSState = false;

	_uint* m_iWeaponState{};
	_uint* m_iViewState{};
	_float3 Position{}, Rotation{};
	_float		m_fAngle_Y{};
	LEVELID m_eLevelID{};
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	const _float4x4* m_pSocketMatrix = { nullptr };
	const _uint* m_pParentState = { nullptr };




private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CHead_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END