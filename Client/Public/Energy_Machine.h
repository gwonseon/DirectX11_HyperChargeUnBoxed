#pragma once
#include "Client_Defines.h"
#include "Player_Build.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CEnergy_Machine final : public CPlayer_Build
{
public:
	typedef struct : public CPlayer_Build::PLAYER_BUILD_DESC
	{
		CPlayer* pPlayer = { nullptr };
		_int	iModelComponentIndex{};
	}ENERGYMACHINE_DESC;

private:
	CEnergy_Machine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnergy_Machine(const CEnergy_Machine& Prototype);
	virtual ~CEnergy_Machine() = default;

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
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	CPlayer*	m_pPlayer = { nullptr };

public:
	void		Set_BatteryInsert(_bool bInsert) { m_bBattery_Insert = bInsert; }
	_bool		Get_Battery_Is_In() { return m_bBattery_Insert; }
	_vector		Get_EnergyMachinePos() { return m_vecPos; }
private:
	_bool		m_bBattery_Insert = false;

	_vector m_vecPos{};
public:
	static CEnergy_Machine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END