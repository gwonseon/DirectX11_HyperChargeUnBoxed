#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "VIBuffer_Terrain.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "Environment.h"
#include "Player.h"
#include "Terrain.h"

BEGIN(Client)

class CMonster_Path final : public CLevel
{
public:
	enum PATHFIND_TYPE { NORMAL_PATHFIND, ASTAR_PATHFIND, PATHFIND_END };
	enum PLAY_ROUND { PLAY_FIRST_ROUND, PLAY_SECOND_ROUND, PLAY_THIRD_ROUND, PLAY_ROUND_END };

public:
	typedef struct
	{
		_float3 fPos{ };
		_uint	iLevel{};
		_uint	iRound{};
		_uint	iModel_Idx{};
		_uint	iCellIdx{};
	}MONSTER_SPAWN_DESC;
private:
	CMonster_Path(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMonster_Path() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	HRESULT Ready_Layer_Terrain(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Lights();


private:
	void	Save_FirstRound();
	void	Save_SecondRound();
	void	Save_ThirdRound();
	void	Load();
	void	Add_Data();
	void	Picking_Create();
	void	Load_Map();
private:
	PATHFIND_TYPE			m_ePathFind_Type{};

	int					m_iRound{};
	int					m_iLevel{};
	int					m_iCellIndex{};
	float				Position[3];
	_float3				m_fPickingPos{};

	
	_bool				m_bAdd = false;
	_bool				m_bSave = false;
	_bool				m_bLoad = false;
	
	vector< MONSTER_SPAWN_DESC> m_vecMonsterSpawn[LEVEL_END][3];

	_bool				m_bOnce = false;
	vector<CCollisionBox*> m_vecCollisionCenter;
private:
	CVIBuffer_Terrain* pVIBuffer_Terrain = { nullptr }; // 터레인 피킹
	CTerrain* m_pTerrain = { nullptr };


private:// 이미지 버튼
	void Create_ImageButton();
	void ButtonImage_List();
private: // 이미지버튼
	CTexture* m_pLoad = nullptr;
	CTexture* m_pSave = nullptr;
	CTexture* m_pMonster = nullptr;
private:// 이미지 버튼
	ID3D11ShaderResourceView* my_Savetexture = nullptr;
	ID3D11ShaderResourceView* my_Loadtexture = nullptr;

	int  m_iModelIndex = 0;


public:
	static CMonster_Path* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

// 여기에 ImGui 몬스터 이동 경로 찍는 레벨 생성할 것
// 따로 만든 이유는 저 코드가 너무 복잡해서
// 이거 열때 찍어놓은 정보 Load하자
// 만드는 김에 사용한 오브젝트 정보도 기억해두고 꺼내오는 코드도 짜자



END