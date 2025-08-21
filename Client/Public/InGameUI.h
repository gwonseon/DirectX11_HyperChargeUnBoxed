#pragma once

#include "Client_Defines.h"
#include "UIObject.h"
#include <Player.h>
#include "UI_CircleGuage.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)


class CInGameUI : public CUIObject
{
public:
	enum GAMEUI{
		UI_CHARACTER,UI_CONVERSATIONBOX,UI_CONVERSATIONBOX_BACKGROUND,
		UI_BUILDMODE_CONVERSATIONBOX,UI_BUILDMODE_F,
		UI_MODECHANGE_ICON,	UI_CENTERICON,UI_SLICE,	UI_END
	};

	typedef struct: public CUIObject::UIOBJECT_DESC
	{
		_uint	iIndex{};
		GAMEUI	eUITag{};
		CPlayer* pPlayer{};
		CUI_CircleGuage* pCircle = {nullptr};
	}INGAMEUI_DESC;

protected:
	CInGameUI(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CInGameUI(const CInGameUI& Prototype);
	virtual ~CInGameUI() = default;

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

	void Change_Count(_int iDeltaCount) {
		m_iCount += iDeltaCount;
	}

public:
	void Set_Draw(_bool bDraw) {
		m_bDraw = bDraw;
	}


private:
	// 캐릭터 대화 상자
	_uint						m_iCharacter_Number = 0;

	// 글자 출력 위치를 위해서 같이 띄우는 위치 저장
	_float3						m_fUIPosition{};

	// 그릴지 안그릴지 결정
	_bool						m_bDraw = true;

	//  플레이어에게서 가져와야 하는 값이 많아서 플레이어 포인터를 들고옴 ( 아차피 삭제 안됨 ㄱㅊ)
	CPlayer*					m_pPlayer = {nullptr};


	GAMEUI		m_eUIType = UI_END;

private:
	CShader* m_pShaderCom = {nullptr};
	CTexture* m_pTextureCom = {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = {nullptr};
	CUI_CircleGuage* m_pCircle = {nullptr};


private:
	HRESULT Add_Components(_int iNum);
	HRESULT Bind_ShaderResources();

public:
	static CInGameUI* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END