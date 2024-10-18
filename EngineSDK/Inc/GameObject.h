#pragma once

#include "Transform.h"

/* 프로토타입을 통해 객체를 생성한다. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		_uint			iData = {};
	}GAMEOBJ_DESC;
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	/* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
	virtual HRESULT Initialize_Prototype();

	/* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
	/* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한ㄴ다. */
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	 

public:
	bool IsValid() const { return !m_bDead; }
	bool Get_Dead() { return m_bDead; }
	void Set_Dead() { m_bDead = true; }


public:
	_float3	Get_PickingPos() { return m_fPickingPos; }
	class CTransform* Get_Transform() { return m_pTransformCom; }

	virtual class CComponent* Find_Component(const _wstring& strComponentTag, _uint iPartObjID = 0);


protected:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	class CTransform* m_pTransformCom = { nullptr };

protected:
	map<const _wstring, class CComponent*>			m_Components;

protected:
	_uint							m_iData = {};
	_float3							m_fPickingPos{};
	_bool							m_bDead = false;
	_vector							m_vecPosition{};

	
public:
	void	Set_Hp(_float Hp)				{ m_fHp = Hp; }
	void	Set_Energy(_float Energy)		{ m_fEnergy = Energy; }
	void	Set_Attact(_float Attack)		{ m_fAttack = Attack; }
	void	Set_Coin(_uint Coin)			{ m_iCoin = Coin; }

	void	Set_GetEnergy(_float Energy)	{ m_fEnergy += Energy; }
	void	Set_Heal(_float Heal)			{ m_fHp += Heal; }
	void	Set_UseCoin(_uint Price)		{ m_iCoin -= Price; }
	void	Set_PickUp_Coin(_uint Price)	{ m_iCoin += Price; }

	void	Set_Attacked(_bool bAttacked)	{ m_bAttacked = bAttacked; }  // 공격 당했음을 알려줌
	void	Set_knockdown(_bool bknockdown) { m_bKnockdown = bknockdown; }

	_float	Get_Hp()						{ return m_fHp; }		// 체력 얼마나 있는지
	_float	Get_Energy()					{ return m_fEnergy; }	// 쉴드량 얼마나 있는지
	_float	Get_Attack()					{ return m_fAttack; }	// 공격력 얼마인지 
	_uint	Get_Coin()						{ return m_iCoin; }		// 돈 얼마나 있는지
		
	_bool	Get_Attacked()					{ return m_bAttacked; }    // 공격을 당했는지 알려줌
	_bool	Get_DontDestroyAble()			{ return m_bDontDestroy; } // 객체 삭제하면 안되는 애인지 아닌지 알려줌
	_bool	Get_knockdown()					{ return m_bKnockdown; }   // 객체 삭제하면 안되는 애들 죽었다고 알리기 위함
	_bool	Get_AttackState()				{ return m_bAttackState; } // 공격 모션인지 아닌지 확인용(이때만 충돌이 되어야 함)
	// 에너지가 있으면 에너지 깎고, 에너지 없으면 Hp깎음
	void	Set_Damaged(_float Attack) {
		if (m_fEnergy > 0)	{
			m_fEnergy -= Attack;
		}
		else {
			m_fHp -= Attack;
		}
		if (m_fEnergy < 0)	{
			// 에너지가 음수면 그만큼 Hp 깎아준다.
			m_fHp -= m_fEnergy;
			m_fEnergy = 0.f;
		}
	}


protected:
	_float							m_fHp{};
	_float							m_fEnergy{};
	_float							m_fAttack{};

	_uint							m_iCoin{};
	_bool							m_bAttacked		= false; // 공격 받았음을 표시 
	_bool							m_bDontDestroy	= false;
	_bool							m_bKnockdown	= false; // 삭제되면 안되는 애들 죽음 상태를 얘로 대체
	_bool							m_bAttackState	= false;
protected:
	HRESULT Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;



};

END