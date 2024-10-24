#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
END

BEGIN(Client)
class CPonyState;
class CPony : public CMonster
{
	class CPonyState* current;
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
	CModel* Get_ModelCom() { return m_pModelCom; }

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CCollider* m_pColliderCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	_vector* m_vecTargetPos;
	CCollider* pTargetCollider = { nullptr };
private:
	_bool		m_bAnimState{};
	_vector vPlayerPos{};
	_vector vPos{};
	_float m_fRunSpeed = 0.f;
	_bool	m_bWalkState = true;;
public:
	void	Set_PonyState(CPonyState* state);
	void	Set_WalkState(_bool bWalk) { m_bWalkState = bWalk; }
	void	Set_RunSpeed(_float fSpeed) { m_fRunSpeed = fSpeed; }
	void	Walk();
	void	Trot();
	void	Idle();
	void	Gallop();
	void	GallopFast();
	void	AttackRepeat();



public:
	static CPony* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END