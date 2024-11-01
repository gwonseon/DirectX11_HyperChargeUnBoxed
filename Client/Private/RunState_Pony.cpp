#include "stdafx.h"
#include "RunState_Pony.h"
#include "Pony.h"


void CRunState_Pony::Enter(CPony* pony)
{
}

void CRunState_Pony::Update(CPony* pony, float fTimeDelta)
{
	if (pony->Get_State() == CPony::ATTACK_STATE)
	{
		pony->ChangeState(new CAttackState_Pony());
	}
	else if (pony->Get_State() == CPony::TROT_STATE)
	{
		pony->ChangeState(new CTrotState_Pony());
	}
	if ((pony->Get_State() == CPony::RUN_STATE))
	{
		pony->Get_ModelCom()->Set_Animation(CPony::PONY_GallopFast, true);
	}
}

void CRunState_Pony::Exit(CPony* pony)
{
}

void CRunState_Pony::Trot(CPony* pony)
{
}

void CRunState_Pony::Run(CPony* pony)
{
}

void CRunState_Pony::Attack(CPony* pony)
{
}

void CRunState_Pony::Walk(CPony* pony)
{

}
