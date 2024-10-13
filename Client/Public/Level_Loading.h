#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "Level_Loading.h"
#include "Loading_UI.h"
#include "BackGround.h"
/* 현재 로딩화면을 보여준다. */
/* 다음 레벨에 대한 자원을 준비한다.(CLoader 하청을 맡길거야) */

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVELID eNextLevelID);
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CLoader* m_pLoader = { nullptr };
	LEVELID						m_eNextLevelID = { LEVEL_END };
	_float						m_fLoading_Per = 0.f;

	CLoading_UI* m_pLoadingUI = { nullptr };
	CLoading_UI* m_pLoadingUI_Logo = { nullptr };
	CLoading_UI* m_pLoadingUI_GameTitle = { nullptr };
	CLoading_UI* m_pLoadingUIBack = { nullptr };
	
	CBackGround* m_pBackGround = { nullptr };
public:
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI_Loading(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI_LOGO(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI_GameTitle(const _tchar* pLayerTag);
public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void Free() override;
};

END