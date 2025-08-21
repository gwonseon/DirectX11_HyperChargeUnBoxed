#pragma once
#include "Client_Defines.h"
#include "UIObject.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CConstUI final: public CUIObject
{
public:
	enum UITYPE{
		UI_SHIFT,UI_RBUTTON,UI_LBUTTON,UI_SPACE,
		UI_ENERGY_ICON,UI_HP_ICON,UI_CREDIT_ICON,UI_RUN_ICON,UI_JUMP_ICON,UI_VIEWCHANGE_ICON,
		UI_PUNCH_ICON,UI_V,UI_F,UI_C,UI_NUCLEAR,
		UI_END
	};

	typedef struct: public CUIObject::UIOBJECT_DESC
	{
		_uint	iIndex{};
		UITYPE	eUITag{};
	}CONST_UI_DESC;

private:
	CConstUI(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CConstUI(const CConstUI& Prototype);
	virtual ~CConstUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components(_int iNum);
	HRESULT Bind_ShaderResources();


private:
	CShader* m_pShaderCom = {nullptr};
	CTexture* m_pTextureCom = {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = {nullptr};

private:
	UITYPE		m_eUIType = UI_END;


public:
	static CConstUI* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END