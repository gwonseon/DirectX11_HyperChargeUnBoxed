#pragma once
#include "Client_Defines.h"
#include "Engine_Defines.h"

BEGIN(Client)

class CPlayer2;
class CPlayer_State
{
public:
	virtual ~CPlayer_State() = default;
	virtual void Enter(CPlayer2* pPlayer) = 0;
	virtual void Update(CPlayer2* pPlayer,float fTimeDelta) = 0;
	virtual void Exit(CPlayer2* pPlayer) = 0;

public:

};
class CPlayerState_Idle;
struct PLAYER_STATE_DESC
{
	static unique_ptr<CPlayer_State> Idle();
};

END