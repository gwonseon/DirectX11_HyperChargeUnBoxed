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

class CHp_Item final : public CGameObject
{

public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		CPlayer* pPlayer = { nullptr };
		CUI_CircleGuage* pGuage = { nullptr };

		LEVELID eID = {};
		_int	iModelIndex{};
	}HPITEM_DESC;


private:
	CHp_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHp_Item(const CHp_Item& Prototype);
	virtual ~CHp_Item() = default;


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
	CUI_CircleGuage* m_pGuage = { nullptr };


private:
	LEVELID	m_eLevel = {};
	_float m_fCharging_Time = 0.f;
	_float3 m_fScale{};
	_vector m_vecItemPos{};


public:
	static CHp_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END