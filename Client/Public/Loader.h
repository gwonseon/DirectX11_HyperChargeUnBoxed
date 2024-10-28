#pragma once

/* 다음 레벨에 대한 자원을 준비한다. */
#include "Client_Defines.h"
#include "Base.h"
#include "Loading_UI.h"

BEGIN(Engine)
class CGameInstance;
class CModel;
END

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVELID eNextLevelID);
	HRESULT Loading();

public:
	_bool isFinished() {
		return m_isFinished;
	}
	_float Get_LoadingPer() { return m_fPersent; }

#ifdef _DEBUG
public:
	void Output_LoadingState();
#endif


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	CLoading_UI* m_pLoadingUi = { nullptr };

private:
	LEVELID						m_eNextLevelID = { LEVEL_END };
	HANDLE						m_hThread = {};
	CRITICAL_SECTION			m_CriticalSection = {};

private:
	_wstring					m_strLoadingText = {};
	_float						m_fPersent = {};
	_bool						m_isFinished = { false };


private:
	// vector<CModel*>				m_Model; Assimp Outer 보류



private:
	HRESULT Loading_For_LogoLevel();
	HRESULT Loading_For_GamePlayLevel();
	HRESULT Loading_For_ImGuiLevel();
	HRESULT Loading_For_NavigationLevel();
	HRESULT Loading_For_MonsterSpawnLevel();

	HRESULT Loading_DataFile(LEVELID eLevelID);
	HRESULT Loading_DataFile_For_GameLevel();
	HRESULT Loading_DataFile_For_NavigationLevel();
	HRESULT Loading_DataFile_For_MonsterSpawnLevel();

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void Free() override;
};

END