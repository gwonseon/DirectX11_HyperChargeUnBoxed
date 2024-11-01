#include "stdafx.h"
#include "TrotState_Pony.h"
#include "Pony.h"

void CTrotState_Pony::Enter(CPony* pony)
{
}

void CTrotState_Pony::Update(CPony* pony,float fTimeDelta)
{
	if (pony->Get_State() == CPony::ATTACK_STATE)
	{
		pony->ChangeState(new CAttackState_Pony());
	}
	else if (pony->Get_State() == CPony::RUN_STATE)
	{
		pony->ChangeState(new CRunState_Pony());
	}

	if ((pony->Get_State() == CPony::TROT_STATE))
	{
		pony->Get_ModelCom()->Set_Animation(CPony::PONY_Trot, true);
	}
}

void CTrotState_Pony::Exit(CPony* pony)
{
}

void CTrotState_Pony::Trot(CPony* pony)
{
}

void CTrotState_Pony::Run(CPony* pony)
{

}

void CTrotState_Pony::Attack(CPony* pony)
{
	
}

void CTrotState_Pony::Walk(CPony* pony)
{
}
