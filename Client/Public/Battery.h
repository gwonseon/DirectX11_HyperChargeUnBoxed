#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Player.h"
#include "UI_CircleGuage.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CBattery final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		CUI_CircleGuage* pGauge = { nullptr };
		CPlayer* pPlayer = { nullptr };
		LEVELID eID = {};
	}BATTERY_DESC;


private:
	CBattery(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBattery(const CBattery& Prototype);
	virtual ~CBattery() = default;


public:
	virtual HRESULT Initialize_Prototype() override;

	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CPlayer* m_pPlayer = { nullptr };
	CUI_CircleGuage* m_pGauge = { nullptr };
private:
	LEVELID	m_eLevel = {};
	_vector* m_vecPos = {nullptr};
	_vector m_vecPrevPos{};
	_bool*   m_bVisible = { nullptr };
	_bool	m_bFirst_PickUp = true;
	_float3 fPrevPos{}, fPos{};
public:
	static CBattery* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};


END