#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END


BEGIN(Client)

class CTruckShooter final : public CGameObject
{
public:
	typedef struct : CGameObject::GAMEOBJ_DESC
	{
		LEVELID m_eLevelID{};
		_uint* iRound = { nullptr };
	}TRUCKSHOOTER_DESC;

private:
	CTruckShooter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTruckShooter(const CTruckShooter& Prototype);
	virtual ~CTruckShooter() = default;


public:
	virtual HRESULT Initialize_Prototype() override;

	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Height();


public:
	_uint* Get_ModelIdx() { return &m_iModelIdx; }

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom[2] = {nullptr};


	_uint* m_iRound = { nullptr };

private:
	LEVELID m_eLevelID{};
	_float fRotX{}, fRotZ{};
	_float m_fRotY = -90.f;
	_bool m_bStart_Shoot = false;
	_bool m_bReady_Shot = false;
	_uint m_iModelIdx = 0;


public:
	static CTruckShooter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END