#pragma once
#include "Client_Defines.h"
#include "Player_Build.h"

BEGIN(Engine)
class CShader;
class CModel;
END
BEGIN(Client)
class CEnergy_Cap final : public CPlayer_Build
{
public:
	typedef struct : public CPlayer_Build::PLAYER_BUILD_DESC
	{

	}ENERGYCAP_DESC;

private:
	CEnergy_Cap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnergy_Cap(const CEnergy_Cap& Prototype);
	virtual ~CEnergy_Cap() = default;


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

public:
	void	Set_BatteryIn() { m_bBattery_In = true; }
private:
	_bool	m_bBattery_In = false;
	_vector vPos{};
	_float3	fPos{};
	_float	fRotation{};
public:
	static CEnergy_Cap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
