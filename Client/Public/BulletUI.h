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

class CBulletUI final: public CUIObject
{
public:
	typedef struct: public CUIObject::UIOBJECT_DESC
	{
		_uint	iIndex{};
		CPlayer* pPlayer{};
	}BULLET_UI_DESC;

private:
	CBulletUI(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CBulletUI(const CBulletUI& Prototype);
	virtual ~CBulletUI() = default;

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
	//  플레이어에게서 가져와야 하는 값이 많아서 플레이어 포인터를 들고옴 ( 아차피 삭제 안됨 ㄱㅊ)
	CPlayer*					m_pPlayer = {nullptr};

private:
	// 그릴지 안그릴지 결정
	_bool						m_bDraw = true;


private:
	CShader* m_pShaderCom = {nullptr};
	CTexture* m_pTextureCom = {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = {nullptr};
public:
	static CBulletUI* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END