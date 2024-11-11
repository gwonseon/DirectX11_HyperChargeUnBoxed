#include "stdafx.h"
#include "RifleMan_Idle.h"
#include "RifleMan.h"

void CRifleMan_Idle::Enter(CRifleMan* rifleman)
{
}

void CRifleMan_Idle::Update(CRifleMan* rifleman, float fTimeDelta)
{
	rifleman->Get_ModelCom()->Set_Animation(CRifleMan::AA_ArmyMen_Move_03, true);
}

void CRifleMan_Idle::Exit(CRifleMan* rifleman)
{
}

void CRifleMan_Idle::Move(CRifleMan* rifleman)
{
	rifleman->ChangeState(new CRifleMan_Move());
}

void CRifleMan_Idle::Fire(CRifleMan* rifleman)
{
	rifleman->ChangeState(new CRifleMan_Fire());
}

void CRifleMan_Idle::Idle(CRifleMan* rifleman)
{
}
