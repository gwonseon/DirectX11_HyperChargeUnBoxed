#include "stdafx.h"
#include "AttackState_Pony.h"
#include "Pony.h"


void CAttackState_Pony::Enter(CPony* pony)
{
}

void CAttackState_Pony::Update(CPony* pony, float fTimeDelta)
{
	if (pony->Get_State() == CPony::RUN_STATE)
	{
		pony->ChangeState(new CRunState_Pony());
	}
	if (pony->Get_State() == CPony::TROT_STATE)
	{
		pony->ChangeState(new CTrotState_Pony());
	}
	if ((pony->Get_State() == CPony::ATTACK_STATE))
	{
		pony->Get_ModelCom()->Set_Animation(CPony::PONY_AttackRepeat, true);
	}
}

void CAttackState_Pony::Exit(CPony* pony)
{
}

void CAttackState_Pony::Trot(CPony* pony)
{
}

void CAttackState_Pony::Run(CPony* pony)
{
}

void CAttackState_Pony::Attack(CPony* pony)
{
}

void CAttackState_Pony::Walk(CPony* pony)
{
}