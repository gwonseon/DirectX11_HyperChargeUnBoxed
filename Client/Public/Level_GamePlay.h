#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "Camera_Free.h"
#include "Player.h"
#include <UI_CircleGuage.h>
#include "BrainCore.h"


BEGIN(Client)
class CGamePlay_Round;
class CLevel_GamePlay  : public CLevel
{

protected:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	void	Interaction_Weapon();
	void    Texture_Render();
private:
	HRESULT Ready_Layer_UI_MACHINE_HP(const _tchar* pLayerTag);

	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Terrain(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Coin(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster_Attack_Far(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_WeaponITem(const _tchar* pLayerTag);
	HRESULT Ready_Layer_PlayerBuild(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Icon(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Damaged(const _tchar* pLayerTag);

	
private:
	void Load_Map();


private:
	CCamera_Free* m_pCamera;
	CPlayer* m_pPlayer;
	CWeapon_Item* m_pWeaponItem[2];
	CUI_CircleGuage* m_pGuage;
	CBrainCore* m_pBrain;

	_float	m_fDelay{};
	_bool m_bOnce = false;
	// Ãæµ¹¿ë
private:
	CLayer* pPlayerLayer		= { nullptr };
	CLayer* pNearMonsterLayer	= { nullptr };
	CLayer* pFarMonsterLayer	= { nullptr };
	CLayer* pCoin				= { nullptr };

	_uint	m_iCurrentRound = 0;
	_bool* m_pReloading = { nullptr };
	_float XPos{}, ZPos{};

private:
	_float	m_fSkipTimer{};
	CGamePlay_Round* m_pRound[3] = {nullptr};
	_bool		m_bRoundStart = false;
public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END