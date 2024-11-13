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


class CInGameUI final : public CUIObject
{
public:
	enum GAMEUI{UI_SHIFT, UI_RBUTTON, UI_LBUTTON, UI_SPACE,UI_DEAD,UI_BATTERY,UI_BATTERY_GAGE ,
		UI_MACHINE_HP,UI_BULLET,UI_CHARACTER,UI_CONVERSATIONBOX, UI_BUILDMODE_CONVERSATIONBOX, UI_BUILDMODE_F,
		UI_MACHINE_ENERGY,UI_DAMAGED,UI_PLAYER_HP, UI_PLAYER_ENERGY,
		UI_ENERGY_ICON, UI_HP_ICON, UI_CREDIT_ICON,UI_RUN_ICON, UI_JUMP_ICON, UI_MODECHANGE_ICON, UI_VIEWCHANGE_ICON,
		UI_PUNCH_ICON, UI_V, UI_F, UI_C, UI_CENTERICON, UI_SLICE, UI_MISSILE_TIMER, UI_NUCLEAR,
		UI_END};

	typedef struct : public CUIObject::UIOBJECT_DESC
	{
		_uint* iRound			= { nullptr };

		_float* fPlayerHP		= { nullptr };
		_float* fPlayerEnergy	= { nullptr };
		_float* fBrainHP		= { nullptr };
		_float* fBrainEnergy	= { nullptr };
		_float* fTimer			= { nullptr };
		GAMEUI	eUITag{};
		_uint	iIndex{};
	
		CPlayer* pPlayer{};
		CUI_CircleGuage* pCircle = { nullptr };
	}INGAMEUI_DESC;

private:
	CInGameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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

	void Change_Count(_int iDeltaCount) { m_iCount += iDeltaCount; }

public:
	void Battery_UI(_float fTimeDelta);
	void Machine_HP_UI(_float fTimeDelta);
	void Charater_UI(_float fTimeDelta);
	void UI_Conversation(_float fTimeDelta);
	void Machine_UI_Energy(_float fTimeDelta);
	void Player_UI_Hp(_float fTimeDelta);
	void Player_UI_Energy(_float fTimeDelta);

public:
	void Set_BatteryGauge(float fGauge) { m_fBatteryGauge = fGauge; }
	void Set_Draw(_bool bDraw) { m_bDraw = bDraw; }

private:
	//_float						m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};
	//_float4x4					m_ViewMatrix, m_ProjMatrix;


	// 배터리
	_uint						m_iBattery = 0;
	float						m_fBatteryGauge = 0.f;


	// 캐릭터 대화 상자
	_uint						m_iCharacter_Number = 0;

	// 대화상자 뒷배경
	_uint						m_iIndex = 0;
	
	// 기계 HP
	float*						m_fMachineHP;

	// 기계 Energy
	float*						m_fMachineEnergy;

	// 플레이어 HP
	float*						m_fPlayerHp;
	// 플레이어 Energy
	float*						m_fPlayerEnergy;

	// 글자 출력 위치를 위해서 같이 띄우는 위치 저장
	_float3						m_fUIPosition{};

	// 내가 가진 코인이 얼마나 있는지 알림
	_uint						m_iCoin{};

	// 그릴지 안그릴지 결정
	_bool						m_bDraw = true;

	LEVELID m_eLevel{};

	//  플레이어에게서 가져와야 하는 값이 많아서 플레이어 포인터를 들고옴 ( 아차피 삭제 안됨 ㄱㅊ)
	CPlayer*					m_pPlayer = { nullptr };

	// 타이머
	_float*						m_fTimer = { nullptr };

	// 라운드
	_uint*						m_iRound = { nullptr };

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CUI_CircleGuage* m_pCircle = { nullptr };


	GAMEUI		m_eUIType = UI_END;
private:
	HRESULT Add_Components(_int iNum);
	HRESULT Bind_ShaderResources();

public:
	static CInGameUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END