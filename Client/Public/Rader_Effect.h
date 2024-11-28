#pragma once
#include "Client_Defines.h"
#include "Player_Build.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CRader_Effect final : public CPlayer_Build
{
public:
	typedef struct : public CPlayer_Build::PLAYER_BUILD_DESC
	{
		LEVELID eID = {};
	}RADER_DESC;

private:
	CRader_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRader_Effect(const CRader_Effect& Prototype);
	virtual ~CRader_Effect() = default;


public:
	virtual HRESULT Initialize_Prototype() override;

	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	LEVELID	m_eLevel = {};
	_uint	m_iModelIndex = 0;
	_float  m_fUV{};
public:
	static CRader_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
