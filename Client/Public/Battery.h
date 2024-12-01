#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Player.h"
#include "UI_CircleGuage.h"
#include "InGameUI.h"
#include "Energy_Machine.h"
#include "BrainCore.h"
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
		CInGameUI* pInGameUI_Gauge = { nullptr };
		CInGameUI* pInGameUI = { nullptr };
		CBrainCore* pBrain = { nullptr };
		CEnergy_Machine* pEnergy_Machine = { nullptr };
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
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	CPlayer*				m_pPlayer = { nullptr };
	CBrainCore*				m_pBrain = { nullptr };
	CUI_CircleGuage*		m_pGauge = { nullptr };
	CEnergy_Machine*		m_pEnergy_Machine = { nullptr };
	CInGameUI*				m_pInGameUI = { nullptr };
	CInGameUI*				m_pInGameUI_Gauge = { nullptr };

private:
	LEVELID	m_eLevel = {};
	_vector* m_vecPos = {nullptr};
	_vector m_vecPrevPos{};
	_bool*   m_bVisible = { nullptr };
	_bool	m_bFirst_PickUp = true;
	_float3 fPrevPos{}, fPos{};
	_float	m_fCharging_Delay{};


	_bool  m_bFallOnce = false;
	_bool  m_bPickUpOnce = false;
public:
	static CBattery* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};


END