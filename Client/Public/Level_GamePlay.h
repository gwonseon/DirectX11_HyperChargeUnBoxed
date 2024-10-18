#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "Camera_Free.h"
#include "Player.h"
#include <UI_CircleGuage.h>
#include "BrainCore.h"
BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{

private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	void	Interaction_Weapon();

private:
	HRESULT Ready_Layer_UI_MACHINE_HP(const _tchar* pLayerTag);

	HRESULT Ready_Layer_UI_Button(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Terrain(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster_Attack_Far(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_WeaponITem(const _tchar* pLayerTag);
	HRESULT Ready_Layer_PlayerBuild(const _tchar* pLayerTag);

	
private:
	void Load_Map();


private:
	CCamera_Free* m_pCamera;
	CPlayer* m_pPlayer;
	CWeapon_Item* m_pWeaponItem[2];
	CUI_CircleGuage* m_pGuage;
	CBrainCore* m_pBrain;

	_float	m_fDelay{};

	// Ãæµ¹¿ë
private:
	CLayer* pPlayerLayer = { nullptr };
	CLayer* pNearMonsterLayer = { nullptr };
	CLayer* pFarMonsterLayer = { nullptr };


	_float XPos{}, ZPos{};
public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END