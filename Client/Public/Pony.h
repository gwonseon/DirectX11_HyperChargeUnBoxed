#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include <Trap_Marks.h>
#include "Pony_State.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
class CTexture; 
class CNavigation;
END

BEGIN(Client)

class CPony : public CMonster
{
private:
	CPony_State* m_pCurrentState;

public:
	typedef struct : CMonster::MONSTER_DESC
	{
		_vector* vecTargetPos{};
		CPlayer_Build* m_pBuild = { nullptr };
	}PONY_DESC;

	enum PONY_ANIM {
		PONY_AttackRepeat,		PONY_Dash, 		PONY_GallopFast,		PONY_Gallop,		PONY_Idle01,
		PONY_Neigh,		PONY_Trot,		PONY_Walk, 
	};

	enum PONY_STATE
	{
		TROT_STATE, RUN_STATE, ATTACK_STATE, PONY_STATE_END
	};
private:
	CPony(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPony(const CPony& Prototype);
	virtual ~CPony() = default;

public:
	virtual HRESULT Initialize_Prototype() override;

	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public: // 상태패턴
	void ChangeState(CPony_State* pNewState)
	{
		if (m_pCurrentState)
		{
			m_pCurrentState->Exit(this);
			delete m_pCurrentState;
		}

		m_pCurrentState = pNewState;

		if (m_pCurrentState)
		{
			m_pCurrentState->Enter(this);
		}
	}


public:
	CModel* Get_ModelCom() { return m_pModelCom; }
	PONY_STATE Get_State() { return m_ePonyState; }

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CCollider*		m_pColliderCom			= { nullptr };
	CShader*		m_pShaderCom				= { nullptr };
	CModel*			m_pModelCom					= { nullptr };
	CNavigation*	m_pNavigationCom		= { nullptr };
	CCollider*	m_pTargetCollider = { nullptr };
	CPlayer_Build* m_pBuild = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	_vector* m_vecTargetPos				= { nullptr };

private:
	_vector vPlayerPos{};
	_float3 m_fPos{};
	_float m_fRunSpeed = 0.f;
	_float m_fAttackTime = 0.f;
	_bool	m_bWalkState = true;
	_bool		m_bAnimState{};
	_bool	m_bFind_Path = false;
	_bool  m_bOnce = false;
	PONY_STATE m_ePonyState = TROT_STATE;
public:
	static CPony* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END