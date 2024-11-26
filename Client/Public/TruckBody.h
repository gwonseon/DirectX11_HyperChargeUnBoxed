#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CModel;
END


BEGIN(Client)

class CTruckBody final : public CPartObject
{
public:
	typedef struct : CPartObject::PARTOBJECT_DESC
	{
		LEVELID m_eLevelID{};
		CPlayer* pPlayer = { nullptr };

	}TRUCKBODY_DESC;

private:
	CTruckBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTruckBody(const CTruckBody& Prototype);
	virtual ~CTruckBody() = default;

public:


public:
	virtual HRESULT Initialize_Prototype() override;

	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Height();



private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CPlayer* m_pPlayer = { nullptr };


private:
	LEVELID m_eLevelID{};


public:
	static CTruckBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END