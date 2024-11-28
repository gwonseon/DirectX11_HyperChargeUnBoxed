#pragma once

#include "Client_Defines.h"
#include "Monster.h"
#include "Player_Build.h"
BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CHelicopter : public CMonster
{
public:
	typedef struct : CMonster::MONSTER_DESC
	{
		CPlayer_Build* pBuild = { nullptr };
		_vector* vecTargetPos{};
	}HELICOPTER_DESC;

	enum HELICOPTER_ANIM { HELICOPTER_CENTER, HELICOPTER_EAST, HELICOPTER_NORTH_EAST, HELICOPTER_NORTH_WEST, HELICOPTER_NOTRH, HELICOPTER_SOUTH, HELICOPTER_WEST, HELICOPTER_DIORAMA };

private:
	CHelicopter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHelicopter(const CHelicopter& Prototype);
	virtual ~CHelicopter() = default;

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
	void	DeadMotion(_float fTimeDelta);
private:
	CCollider* m_pColliderCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* pTargetCollider = { nullptr };
	CPlayer_Build* m_pBuild = { nullptr };

private:
	_vector* m_vecTargetPos = { nullptr };


private:
	_bool		m_bAnimState{};
	_float		m_fRotation{};


	_float		m_iShot_Count = 0; // 3¹ß ½î±â À§ÇØ ¸î ¹ß ½ú´ÂÁö ÀúÀå
	_float		m_fShot_Time_Delay = 3.f; // ÃÑ¾Ë ½î±â¿ë µô·¹ÀÌ ½Ã°£
	_float     m_fAcc;
public:
	static CHelicopter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END