#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include <Aura.h>
BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)


class CCoin final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		LEVELID eID = {};
	}COIN_DESC;

private:
	CCoin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCoin(const CCoin& Prototype);
	virtual ~CCoin() = default;


public:
	virtual HRESULT Initialize_Prototype() override;

	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_vector		Get_Pos()
	{
		if (m_pTransformCom)
			return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		return { 0.f,0.f, 0.f, 0.f };
	}
	_float3		Get_Scale() { return m_fScale; }
	LEVELID		Get_Level() { return m_eLevel; }

public:
	void	Set_Scale(_float fTimeDelta, _float PosX, _float PosY, _float PosZ)
	{
		m_pTransformCom->Set_Scaling(PosX, PosY, PosZ);
		m_fScale.x = PosX;
		m_fScale.y = PosY;
		m_fScale.z = PosZ;
	}
	void	MovePos( _float PosX, _float PosY, _float PosZ)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { PosX,PosY,PosZ,1 });
	}
	void Set_Dead() {
		m_pAura->Set_Dead();
		m_bDead = true;
	}
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CAura* m_pAura = { nullptr };
private:
	LEVELID	m_eLevel = {};
	_uint	m_iModelIndex = 0;

	_float3	m_fScale = {};

public:
	static CCoin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};


END

