#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CBulb final : public CGameObject
{
public:
	enum BULB_TYPE{BULB_CEIL, BULB_SPOT, BULB_END};
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		BULB_TYPE eBulbType = {};
		LEVELID eID = {};
	}BULB_DESC;

private:
	CBulb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBulb(const CBulb& Prototype);
	virtual ~CBulb() = default;


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

private:
	BULB_TYPE m_eBulbType = {};
	LEVELID	m_eLevel = {};
	_uint	m_iModelIndex = 0;

	_float3 m_fPos_Temp{};
	_float3 m_fAngle_Temp{};
public:
	static CBulb* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END