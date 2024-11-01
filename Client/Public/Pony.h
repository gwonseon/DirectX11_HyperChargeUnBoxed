#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include <Trap_Marks.h>
#include "Pony_State.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
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
	
	}PONY_DESC;

	enum PONY_ANIM {
		PONY_AttackRepeat,		PONY_Dash, 		PONY_GallopFast,		PONY_Gallop,		PONY_Idle01,
		PONY_Neigh,		PONY_Trot,		PONY_Walk, 
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


public: // 상태패턴
	void ChangeState(CPony_State* pNewState)
	{
		if (m_pCurrentState)
			m_pCurrentState->Exit(this);

		m_pCurrentState = pNewState;

		if (m_pCurrentState)
		{
			m_pCurrentState->Enter(this);
		}
	}


public:
	CModel* Get_ModelCom() { return m_pModelCom; }

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CCollider*		m_pColliderCom			= { nullptr };
	CShader*		m_pShaderCom				= { nullptr };
	CModel*			m_pModelCom					= { nullptr };
	CNavigation*	m_pNavigationCom		= { nullptr };

	_vector* m_vecTargetPos				= { nullptr };

private:
	_vector vPlayerPos{};
	_vector vPos{};
	_float3 m_fPos{};
	_float m_fRunSpeed = 0.f;
	
	_bool	m_bWalkState = true;
	_bool		m_bAnimState{};
	_bool	m_bFind_Path = false;


public:
	static CPony* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END