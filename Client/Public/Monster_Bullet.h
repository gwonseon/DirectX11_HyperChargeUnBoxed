#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Player_Build.h"
#include "Camera_Free.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)


class CMonster_Bullet final : public CGameObject
{
public:
	enum MONSTERBULLET_TYPE { TANK_BULLET, HELICOPTER_BULLET,RIFLEMAN_BULLET, MONSTERBULLET_END };

	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		LEVELID eID = {};
		MONSTERBULLET_TYPE eType;
		_uint m_iModelNumber{};
		_vector vDir{}, vTargetPos{};
		CPlayer_Build* m_pBuild = { nullptr };
		CPlayer* pPlayer = { nullptr };
		CCamera* pCamera = { nullptr };
	}MONSTER_BULLET_DESC;

	
private:
	CMonster_Bullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Bullet(const CMonster_Bullet& Prototype);
	virtual ~CMonster_Bullet() = default;

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


private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CCollider* m_pTargetCollider = { nullptr };
	CCollider* m_pPlayerCollider = { nullptr };
	CPlayer_Build* m_pBuild = { nullptr };
	CPlayer* m_pPlayer = { nullptr };
	CCamera* m_pCamera = { nullptr };

private:
	LEVELID m_eLevel{};
	MONSTERBULLET_TYPE m_eType{};
	_uint m_iModelNumber{};
	_vector m_vecDir{}, m_vecTargetPos{};
	_bool bCollision = false;
public:
	static CMonster_Bullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END