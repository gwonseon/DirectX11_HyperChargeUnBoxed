#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CExplosion final : public CGameObject
{
public:
	enum EXPLOSION_TYPE { EXPLOSION_TRUCK, EXPLOSION_TANK , EXPLOSION_LOCKET,EXPLOSION_END };

	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		LEVELID eID{};
		_uint	iModelIndex{};
		EXPLOSION_TYPE eType{};
	}EXPLOSION_DESC;

private:
	CExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CExplosion(const CExplosion& Prototype);
	virtual ~CExplosion() = default;


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


private:
	LEVELID	m_eLevel = {};
	EXPLOSION_TYPE m_eType{};



	_uint	m_iModelIndex = 0;



public:
	static CExplosion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END