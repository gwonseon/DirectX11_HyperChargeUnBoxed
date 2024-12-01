#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Player.h"
#include "UI_CircleGuage.h"
#include "Aura.h"


BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CCoin_Item final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		CPlayer* pPlayer			= { nullptr };
		CUI_CircleGuage* pGuage		= { nullptr };
		LEVELID eID = {};
		_int	iModelIndex{};
	}COINITEM_DESC;


private:
	CCoin_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCoin_Item(const CCoin_Item& Prototype);
	virtual ~CCoin_Item() = default;


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
	CShader*							m_pShaderCom = { nullptr };
	CModel*								m_pModelCom = { nullptr };
	CTexture*							m_pTextureCom = { nullptr };

	CPlayer*							m_pPlayer = { nullptr };
	CUI_CircleGuage*					m_pGuage = { nullptr };
	CAura*								m_pAura = { nullptr };
private:
	LEVELID	m_eLevel = {};
	_uint	m_iModelIndex	= 0;
	_float m_fCharging_Time{};
	_float m_fSizeUp{};
	_float3 m_fScale{};
	_vector m_vecItemPos{};
	_bool					m_bOnce = false;


	_float m_fDeadTime{};

	_bool m_bInteraction_Player{};
public:
	static CCoin_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
