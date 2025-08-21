#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CMissile_TimeUI final: public CUIObject
{
public:
	typedef struct: public CUIObject::UIOBJECT_DESC
	{
		_uint* iRound			= {nullptr};
		_float* fTimer			= {nullptr};
		_uint	iIndex{};
	}MISSILE_TIMER_UI_DESC;

private:
	CMissile_TimeUI(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CMissile_TimeUI(const CMissile_TimeUI& Prototype);
	virtual ~CMissile_TimeUI() = default;

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

private:
	// 타이머
	_float*						m_fTimer = {nullptr};
	// 라운드
	_uint*						m_iRound = {nullptr};

	// 그릴지 안그릴지 결정
	_bool						m_bDraw = true;


private:
	CShader* m_pShaderCom = {nullptr};
	CTexture* m_pTextureCom = {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = {nullptr};
public:
	static CMissile_TimeUI* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END