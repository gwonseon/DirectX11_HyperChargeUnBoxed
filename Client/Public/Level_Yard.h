#pragma once

#include "Client_Defines.h"
#include "Level.h"


#include "Camera_Free.h"
#include "Player.h"
#include "UI_CircleGuage.h"
#include "BrainCore.h"
#include "Trap_Marks.h"
#include "Energy_Machine.h"
#include "Energy_Lader.h"
#include "Energy_Cap.h"
#include "Battery.h"
#include "InGameUI.h"
#include "Missile_Truck.h"

BEGIN(Client)

class CYard_Round;
class CLevel_Yard : public CLevel
{
public:
	enum TEXT_STATE_YARD
	{
		STATE_DONT_DRAW = 0x00000001,
		STATE_HALF_HP = 0x00000002,
		STATE_HALF_ENERGY = 0x00000004,
		STATE_WARNING = 0x00000008,
		STATE_MISSILE_WARNING = 0x00000010,
		STATE_GOOD = 0x00000020,
		STATE_PROVOKE = 0x00000040,

	};
	
protected:
	CLevel_Yard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Yard() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	void	Interaction();
	void    Texture_Render();
	void	Texture_Update(_float fTimeDelta);
private:
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Terrain(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_WeaponITem(const _tchar* pLayerTag);
	HRESULT Ready_Layer_ITem(const _tchar* pLayerTag);
	HRESULT Ready_Layer_PlayerBuild(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Icon(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Trap(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Damaged(const _tchar* pLayerTag);

	HRESULT Ready_Layer_MissileTruck(const _tchar* pLayerTag);

	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag);

private:
	void Load_Map();
	void Build_Check();
	void RoundMgr_And_MonsterSpawn(_float fTimeDelta);
	void Conversation_Draw(_bool bDraw);
private:
	CCamera_Free* m_pCamera = { nullptr };
	CPlayer* m_pPlayer = { nullptr };
	CUI_CircleGuage* m_pGuage = { nullptr };
	CBrainCore* m_pBrain = { nullptr };
	CEnergy_Machine* m_pEnergyMachine = { nullptr };
	CEnergy_Cap* m_pEnergyMachine_Cap = { nullptr };
	CBattery* m_pBattery = { nullptr };
	CInGameUI* m_pBatteryUI = { nullptr };
	CInGameUI* m_pBatteryGaugeUI = { nullptr };
	CInGameUI* m_pConversationBox = { nullptr };
	CInGameUI* m_pCharacter = { nullptr };
	CInGameUI* m_pMissile_Timer = { nullptr };

	CWeapon_Item* m_pWeaponItem[2];
	CMissile_Truck* m_pMissile_Truck = { nullptr };


	_float	m_fDelay{};
	_float  m_fConversation_Draw_Timer{};
	_bool m_bOnce = false;
	// 충돌용
private:
	CLayer* pPlayerLayer = { nullptr };
	CLayer* pNearMonsterLayer = { nullptr };
	CLayer* pFarMonsterLayer = { nullptr };
	CLayer* pCoin = { nullptr };
	CLayer* pTrap = { nullptr };
	CLayer* pTrap_Shield = { nullptr };
	CLayer* pMonsterBullet = { nullptr };
	CLayer* pCircleUI = { nullptr };
	CLayer* pItem = { nullptr };
	CLayer* pExplosion = { nullptr };
	CLayer* pBuild = { nullptr };
	CLayer* pTruck = { nullptr };

	_bool* m_pReloading = { nullptr };
	_uint	m_iCurrentRound = 0;
	_uint	m_iPreviousRound = 0;
	_float XPos{}, ZPos{};
private:
	vector<CTrap_Marks*> m_vecTrapMark;

	// 라운드
private:
	_float	m_fSkipTimer{};
	CYard_Round* m_pRound[3] = { nullptr };
	_bool		m_bRoundStart = false;

	_uint m_eTextState = STATE_DONT_DRAW;
	_uint m_iDrawNumber = 99;
	_uint m_iMissile_Round = 1;
public:
	static CLevel_Yard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};
END