#include "stdafx.h"
#include "Player_State.h"
#include "Player_Define.h"

unique_ptr<CPlayer_State> PLAYER_STATE_DESC::Idle()
{
	return make_unique<CPlayerState_Idle>();
}