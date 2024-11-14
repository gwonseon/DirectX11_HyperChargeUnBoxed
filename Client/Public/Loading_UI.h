#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)


class CLoading_UI final : public CUIObject
{
public:
	enum UITAG{ LOADING_GAGE, LOADING_LOGO, LOADING_GAMENAME, LOADING_BACKGROUND_GAMENAME,LOADING_END};
	typedef struct : public CUIObject::UIOBJECT_DESC
	{
		enum UITAG eTag{};
		enum LEVELID eTargetLevel {};
	}LOADINGUI_DESC;
private:
	CLoading_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoading_UI(const CLoading_UI& Prototype);
	virtual ~CLoading_UI() = default;

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


private:
	HRESULT Add_Components(_int iNum);
	HRESULT Bind_ShaderResources();

public:
//	void	Set_Percent(_float fPer) { m_fPercent = fPer; }
	void	Set_Percent(_float fPer) {
		m_fPercent = fPer;
	}

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CTexture* m_pTextureCom_Loading0 = { nullptr };
	CTexture* m_pTextureCom_Loading1 = { nullptr };
	CTexture* m_pTextureCom_Loading2 = { nullptr };
	_uint		m_iIndex = 0;
	_float		m_fPercent = 0.f;
	_float		m_fPrePercent = 0.f;
	UITAG		m_eTag{};
	_float		m_fTick{};

	LEVELID m_eTargetLevel{};
public:
	static CLoading_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END