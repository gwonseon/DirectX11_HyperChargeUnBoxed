#pragma once
#include "Client_Defines.h"
#include "Level_Yard.h"
#include "Player.h"

BEGIN(Client)

class CYard_Round : public CLevel_Yard
{
public:
	typedef struct
	{
		ANIMMODEL_INDEX eModelIndex{};
		_float3		fPos{};
		_uint		iCell_Idx{};

	}MONSTER_CREATE_FOR_YARD_DESC;

private:
	CYard_Round(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CYard_Round() = default;

public:
	virtual HRESULT Initialize(_uint iRound);
	virtual void Update(_float fTimeDelta) override;

public:
	void Set_CurrentRound(_uint iRound) { m_iCurrentRound = iRound; }
	void Set_Player_Build(CPlayer_Build* pBuild) { m_pBuild = pBuild; }
	void Set_Player_BrainCore(CBrainCore* pBrain) { m_pBrain = pBrain; }
	void Set_BrainPos(_vector* vPos) { vecBrainPos = vPos; }
	void Set_PlayerPos(_vector* vPos) { vecPlayerPos = vPos; }
	const void Set_PlayerWorld_matrix(const _float4x4* vMatrix) { matPlayerWorld = vMatrix; }
	const void Set_BrainCoreWorld_matrix(const _float4x4* vMatrix) { matBrainCoreWorld = vMatrix; }
	void	Set_RemainMonster_Count(_uint iCount) { m_iCurrent_RemainMonster = iCount; }
	void	Set_TrapLayer(CLayer* pLayer) { m_pTrapLeyer = pLayer; }
	void	Set_Player(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	_uint	Get_MonsterCount() { return m_iMonsterCount; }
	void	MonsterCreate(_float fTimeDelta);


private:
	_uint m_iCurrentRound{};
	_uint m_iMyRound{}; //  이 객체가 갖고 있는 라운드
	_uint m_iMonsterCount{};
	_uint m_iCurrent_RemainMonster{};
	_float fRound_Time{}, fCreate_Time{};
	vector< MONSTER_CREATE_FOR_YARD_DESC> m_vecMonsterCreate;

	ANIMMODEL_INDEX eModel_Index{};


	CLayer* m_pTrapLeyer = { nullptr };
	_vector* vecBrainPos = { nullptr };
	_vector* vecPlayerPos = { nullptr };
	const _float4x4* matPlayerWorld = { nullptr };
	const _float4x4* matBrainCoreWorld = { nullptr };
	CPlayer_Build* m_pBuild = { nullptr };
	CBrainCore* m_pBrain = { nullptr };
	CPlayer* m_pPlayer = { nullptr };
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static CYard_Round* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iRound);
	virtual void Free() override;

};

END