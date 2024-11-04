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

class CBlimp : public CMonster
{
public:
	typedef struct : CMonster::MONSTER_DESC
	{
		CPlayer_Build* pBuild = { nullptr };
		_vector* vecTargetPos{};

	}BLIMP_DESC;

private:
	CBlimp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlimp(const CBlimp& Prototype);
	virtual ~CBlimp() = default;


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
	CCollider* m_pColliderCom	= { nullptr };
	CShader* m_pShaderCom		= { nullptr };
	CModel* m_pModelCom			= { nullptr };
	CPlayer_Build* m_pBuild		= { nullptr };


public:
	static CBlimp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};

END