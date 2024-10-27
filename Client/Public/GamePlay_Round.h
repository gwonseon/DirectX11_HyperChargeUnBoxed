#pragma once
#include "Client_Defines.h"
#include "Level_GamePlay.h"


BEGIN(Client)

class CGamePlay_Round : public CLevel_GamePlay
{
public:
	typedef struct
	{
		ANIMMODEL_INDEX eModelIndex{};
		_float3		fPos{};
		_uint		iCell_Idx{};

	}MONSTER_CREATE_DESC;
private:
	CGamePlay_Round(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CGamePlay_Round() = default;

public:
	virtual HRESULT Initialize() ;
	virtual void Update(_float fTimeDelta) override;

public:
	void Set_CurrentRound(_uint iRound) { m_iCurrentRound = iRound; }
	void Set_Player_Build(CPlayer_Build* pBuild) { m_pBuild = pBuild; }
	void Set_Player_BrainCore(CBrainCore* pBrain) { m_pBrain = pBrain; }
	void Set_BrainPos(_vector* vPos) { vecBrainPos = vPos; }
	void Set_PlayerPos(_vector* vPos) { vecPlayerPos = vPos; }
	const void Set_PlayerWorld_matrix(const _float4x4* vMatrix) { matPlayerWorld = vMatrix; }
	const void Set_BrainCoreWorld_matrix(const _float4x4* vMatrix) { matBrainCoreWorld = vMatrix; }


private:
	_uint m_iCurrentRound{};
	_float fRound_Time;
	vector< MONSTER_CREATE_DESC> m_vecMonsterCreate;

	ANIMMODEL_INDEX eModel_Index{};



	_vector* vecBrainPos{};
	_vector* vecPlayerPos{};
	const _float4x4* matPlayerWorld = { nullptr };
	const _float4x4* matBrainCoreWorld = { nullptr };
	CPlayer_Build* m_pBuild = { nullptr };
	CBrainCore* m_pBrain = { nullptr };

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static CGamePlay_Round* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	virtual void Free() override;
};

END