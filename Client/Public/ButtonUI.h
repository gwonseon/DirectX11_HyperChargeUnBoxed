#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CButtonUI : public CUIObject
{
public:
	enum BUTTONTAG { BUTTON_PLAY, BUTTON_CREATE, BUTTON_MINI, BUTTON_END };

	typedef struct : public CUIObject::UIOBJECT_DESC
	{
		enum BUTTONTAG eTag {};

	}BUTTONUI_DESC;
private:
	CButtonUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CButtonUI(const CButtonUI& Prototype);
	virtual ~CButtonUI() = default;

public:
	/* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
	virtual HRESULT Initialize_Prototype() override;

	/* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
	/* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한ㄴ다. */
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	_bool	Get_bClick() { return m_bClick; }
private:
	HRESULT Add_Components(_int iNum);
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom_Button0 = { nullptr };

private:
	BUTTONTAG m_eTag{};

	POINT	m_ptMouse{};
	_float3	m_vMousePos = {};
	_float2	m_vObjectPos = {};
	_bool	m_bClick = false;
	_float4	m_fButtonRange = {};







public:
	static CButtonUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END