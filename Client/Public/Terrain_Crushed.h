#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CTerrain_Crushed : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		LEVELID eLevel{};
		CPlayer* pPlayer = {nullptr};
	}TERRAIN_CRUSHED_DESC;

private:
	CTerrain_Crushed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain_Crushed(const CTerrain_Crushed& Prototype);
	virtual ~CTerrain_Crushed() = default;

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
	CPlayer* m_pPlayer = { nullptr };

private:
	LEVELID m_eLevel{};
	_float m_fAnimSpeed{};
	_float m_fDeadDelay{};
	_bool m_bAnimState = false;
public:
	static CTerrain_Crushed* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
