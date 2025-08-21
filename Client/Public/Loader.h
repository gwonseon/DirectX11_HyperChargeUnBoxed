#pragma once

/* ���� ������ ���� �ڿ��� �غ��Ѵ�. */
#include "Client_Defines.h"
#include "Base.h"

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

private:
	LEVELID						m_eNextLevelID = { LEVEL_END };
	HANDLE						m_hThread = {};
	CRITICAL_SECTION			m_CriticalSection = {};

private:
	_wstring					m_strLoadingText = {};
	_float						m_fPersent = {};
	_bool						m_isFinished = { false };

private:
	_uint m_iGrass_Count[4];
	vector<_float3> m_vecGrassPos[4];
	

private:
	LEVELID m_eTargetLevel = LEVEL_MONSTERSPAWN;



private:
	HRESULT Loading_For_LogoLevel();
	HRESULT Loading_For_GamePlayLevel();
	HRESULT Loading_For_GameYardLevel();
	HRESULT Loading_For_ImGuiLevel();
	HRESULT Loading_For_NavigationLevel();
	HRESULT Loading_For_MonsterSpawnLevel();

	HRESULT Loading_DataFile(LEVELID eLevelID);
	HRESULT Loading_DataFile_For_GameLevel();
	HRESULT Loading_DataFile_For_YardLevel();


	HRESULT Loading_DataFile_For_NavigationLevel();
	HRESULT Loading_DataFile_For_MonsterSpawnLevel(LEVELID eLevelID);

	HRESULT Loading_Effect(LEVELID eLevelID);
	HRESULT Loading_DataFile_For_Instancing_YardLevel();
	HRESULT Loading_DataFile_For_Instancing_ImGuiLevel();

	HRESULT Loading_UI();
public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void Free() override;
};

END