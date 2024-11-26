#pragma once

#include "Client_Defines.h"
#include "Player_Build.h"


BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CTrap_Bricks final : public CPlayer_Build
{
public:
	typedef struct : public CPlayer_Build::PLAYER_BUILD_DESC
	{
		_bool* m_bBuild = {nullptr};
		_bool* m_bBuild_PreView = { nullptr };
	}TRAP_BRICKS_DESC;


private:
	CTrap_Bricks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrap_Bricks(const CTrap_Bricks& Prototype);
	virtual ~CTrap_Bricks() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Height();

public:
	void	Set_ReBuild() {
		m_bKnockdown = false;
		m_bOnce = false;
		m_fHp = 100.f;
	}

public:
	_bool Get_CanBuy() { return m_bCanBuy; }
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CPlayer* m_pPlayer = { nullptr };


private:
	_uint	m_iModel_Idx{};
	_bool*	m_bBuild = { nullptr };
	_bool*	m_bBuild_PreView = { nullptr };
	_bool	m_bOnce = false;

	_bool   m_bCanBuy = false;
public:
	static CTrap_Bricks* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END