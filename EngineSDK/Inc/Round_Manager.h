#pragma once
#include "Base.h"
#include <Layer.h>

BEGIN(Engine)

class CRound_Manager final : public CBase
{
public:
	enum ROUND{ BREAKTIME_ROUND, FIRST_ROUND, SECOND_ROUND, THIRD_ROUND, ROUND_END};
private:
	CRound_Manager();
	virtual ~CRound_Manager() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta, _uint& iCurrentRound, _bool& bBuildMode, CLayer* Monster_Near, CLayer* Monster_Far, _bool& bRoundStart,_float& SkipTimer);
	

	

public:
	void Set_CurrentLevel(_uint iLevel) { m_iLevel = iLevel; }
	void Set_MissileState(_bool bBroken) { m_bMissile_Broken = bBroken; }

	void Set_Reset()
	{
		m_iCurrent_Round = 0;
		m_iMonster_Count = 0;
	}
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };


private:
	_uint		m_iMonster_Count = 0;
	_uint		iTemp{};
	_uint		m_iCurrent_Round = 0; // 현재 라운드
	_float		m_fBreakTimeSkip_Timer = 5.f; // 쉬는 시간 스킵 타이머
	_float		m_fBreakTime_Timer{}; // 쉬는 시간 타이머
	ROUND		m_eRound{};

	_uint		m_iLevel = 0;  // 3은 GamePlay   4은 Yard
	_bool		m_bMissile_Broken = false;
public:
	static CRound_Manager* Create();
	virtual void Free() override;
};

END