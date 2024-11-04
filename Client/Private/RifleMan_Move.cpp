#include "stdafx.h"
#include "RifleMan_Move.h"
#include "RifleMan.h"

void CRifleMan_Move::Enter(CRifleMan* rifleman)
{
	
}

void CRifleMan_Move::Update(CRifleMan* rifleman, float fTimeDelta)
{
	rifleman->Get_ModelCom()->Set_Animation(CRifleMan::AA_ArmyMen_Move_01, false);
}

void CRifleMan_Move::Exit(CRifleMan* rifleman)
{

}

void CRifleMan_Move::Move(CRifleMan* rifleman)
{

}

void CRifleMan_Move::Fire(CRifleMan* rifleman)
{
	rifleman->ChangeState(new CRifleMan_Fire());
}
