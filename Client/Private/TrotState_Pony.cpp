#include "stdafx.h"
#include "TrotState_Pony.h"
#include "Pony.h"

void CTrotState_Pony::Enter(CPony* pony)
{
}

void CTrotState_Pony::Update(CPony* pony,float fTimeDelta)
{
}

void CTrotState_Pony::Exit(CPony* pony)
{
}

void CTrotState_Pony::Trot(CPony* pony)
{
	// ÀÌ¹Ì Trot
}

void CTrotState_Pony::Run(CPony* pony)
{
	pony->ChangeState();
}

void CTrotState_Pony::Attack(CPony* pony)
{
}

void CTrotState_Pony::Walk(CPony* pony)
{
}
