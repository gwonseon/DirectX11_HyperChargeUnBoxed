#pragma once
#include "Client_Defines.h"
#include "Player_Build.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider; 
END

BEGIN(Client)

class CBrainCore final :  public CPlayer_Build
{
public:
	typedef struct : public CPlayer_Build::PLAYER_BUILD_DESC
	{
		_int	iModelComponentIndex{};
	}BRAIN_CORE_DESC;


private:
	CBrainCore(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBrainCore(const CBrainCore& Prototype);
	virtual ~CBrainCore() = default;

public:
	virtual HRESULT Initialize_Prototype() override;

	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Height();
public:
	_vector* Get_BrainPos() { return &m_vecPos; }
	_float* Get_BrainHp() { return &m_fHp; }
	_float* Get_BrainEnergy() { return &m_fEnergy; }


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CCollider* m_pColliderCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	_vector m_vecPos{};

	_bool m_bOnce = false;

public:
	static CBrainCore* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END

