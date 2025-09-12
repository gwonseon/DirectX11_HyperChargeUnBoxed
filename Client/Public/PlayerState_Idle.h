#pragma once
#include "Client_Defines.h"
#include "Engine_Defines.h"

#include "Player_State.h"

BEGIN(Client)

class CPlayerState_Idle : public CPlayer_State
{
public:
	void Enter(CPlayer2* pPlayer) override;
	void Update(CPlayer2* pPlayer,float fTimeDelta) override;
	void Exit(CPlayer2* pPlayer) override;

};

END