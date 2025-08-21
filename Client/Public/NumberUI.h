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
class CNumberUI final: public CUIObject
{
public:
	enum DIGIT {
		ONE_DIGIT,        // 일의 자리
		TEN_DIGIT,        // 십의 자리
		HUNDREDS_DIGIT,   // 백의 자리
		THOUSANDS_DIGIT,  // 천의 자리
		TENS_OF_THOUSANDS_DIGIT, // 만의 자리
		HUNDREDS_OF_THOUSANDS_DIGIT // 십만의 자리
	};

	enum TYPE_OF_USAGE{
		TYPE_COIN,TYPE_BULLET,TYPE_FULLBULLET,TYPE_END
	};
	typedef struct: public CUIObject::UIOBJECT_DESC
	{
		TYPE_OF_USAGE eTypeUsage{};
		DIGIT	eDigit{};
		CPlayer* pPlayer{};

	}NUMBERUI_DESC;

private:
	CNumberUI(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CNumberUI(const CNumberUI& Prototype);
	virtual ~CNumberUI() = default;

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
	void Coin(_float fTimeDelta);
	void Bullet(_float fTimeDelta);
	void FullBullet(_float fTimeDelta);
private:
	CShader* m_pShaderCom = {nullptr};
	CTexture* m_pTextureCom = {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = {nullptr};


	CPlayer* m_pPlayer = {nullptr};


private:
	_uint m_iDigit{};
	_uint m_iUsage{};
	_uint m_iNumber{};
	_uint m_iGetNum{};
	_uint m_iLastDigit{};
	_float fShot_Size{};
	_float m_fScaleX{};
	_float m_fScaleY{};
	_float m_fDistance{};
	_vector vFirstPos{};
	LEVELID m_eLevel{};

	_bool m_bDraw = false;

	_vector vPos{};
private:
	HRESULT Add_Components(_int iNum);
	HRESULT Bind_ShaderResources();

public:
	static CNumberUI* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};


END