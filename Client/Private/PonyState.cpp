#include "stdafx.h"
#include "..\Public\PonyState.h"

CPonyWalk* CPonyWalk::instance;
CPonyTrot* CPonyTrot::instance;
CPonyIdle* CPonyIdle::instance;
CPonyGallop* CPonyGallop::instance;
CPonyGallopFast* CPonyGallopFast::instance;
CPonyAttackRepeat* CPonyAttackRepeat::instance;



CPonyWalk* CPonyWalk::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new CPonyWalk();
	}

	return instance;
}

void CPonyWalk::Update(CPony* pony, _float fTimeDelta)
{
	 fWalkingTime += fTimeDelta;

	if (fWalkingTime >= 5.0f) {
		pony->GallopFast(); 
		pony->Set_WalkState(false);
		pony->Set_RunSpeed(fTimeDelta);
	}
}
void CPonyWalk::Enter(CPony* pony)
{
	pony->Get_ModelCom()->Set_Animation(CPony::PONY_Walk, true);
}
void CPonyWalk::Exit(CPony* pony)
{
	fWalkingTime = 0.f;
	pony->Set_RunSpeed(0.f);
}

void CPonyWalk::Walk(CPony* pony)
{
}

void CPonyWalk::Trot(CPony* pony)
{
	Set_PonyState(pony, CPonyTrot::GetInstance());
}

void CPonyWalk::Idle(CPony* pony)
{
	Set_PonyState(pony, CPonyIdle::GetInstance());
}

void CPonyWalk::Gallop(CPony* pony)
{
	Set_PonyState(pony, CPonyGallop::GetInstance());

}

void CPonyWalk::GallopFast(CPony* pony)
{
	Set_PonyState(pony, CPonyGallopFast::GetInstance());

}

void CPonyWalk::AttackRepeat(CPony* pony)
{
	Set_PonyState(pony, CPonyAttackRepeat::GetInstance());
}

CPonyTrot* CPonyTrot::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new CPonyTrot();
	}

	return instance;
}

void CPonyTrot::Update(CPony* pony, _float fTimeDelta)
{
}

void CPonyTrot::Enter(CPony* pony)
{
}

void CPonyTrot::Exit(CPony* pony)
{
}

void CPonyTrot::Walk(CPony* pony)
{
	Set_PonyState(pony, CPonyWalk::GetInstance());
}

void CPonyTrot::Trot(CPony* pony)
{

}

void CPonyTrot::Idle(CPony* pony)
{
	Set_PonyState(pony, CPonyIdle::GetInstance());

}

void CPonyTrot::Gallop(CPony* pony)
{
	Set_PonyState(pony, CPonyGallop::GetInstance());

}

void CPonyTrot::GallopFast(CPony* pony)
{
	Set_PonyState(pony, CPonyGallopFast::GetInstance());

}

void CPonyTrot::AttackRepeat(CPony* pony)
{
	Set_PonyState(pony, CPonyAttackRepeat::GetInstance());

}

CPonyIdle* CPonyIdle::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new CPonyIdle();
	}

	return instance;
}

void CPonyIdle::Update(CPony* pony, _float fTimeDelta)
{
}

void CPonyIdle::Enter(CPony* pony)
{
}

void CPonyIdle::Exit(CPony* pony)
{
}

void CPonyIdle::Walk(CPony* pony)
{
	Set_PonyState(pony, CPonyWalk::GetInstance());

}

void CPonyIdle::Trot(CPony* pony)
{
	Set_PonyState(pony, CPonyTrot::GetInstance());

}

void CPonyIdle::Idle(CPony* pony)
{

}

void CPonyIdle::Gallop(CPony* pony)
{
	Set_PonyState(pony, CPonyGallop::GetInstance());

}

void CPonyIdle::GallopFast(CPony* pony)
{
	Set_PonyState(pony, CPonyGallopFast::GetInstance());

}

void CPonyIdle::AttackRepeat(CPony* pony)
{
	Set_PonyState(pony, CPonyAttackRepeat::GetInstance());

}

CPonyGallop* CPonyGallop::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new CPonyGallop();
	}

	return instance;
}

void CPonyGallop::Update(CPony* pony, _float fTimeDelta)
{
}

void CPonyGallop::Enter(CPony* pony)
{
}

void CPonyGallop::Exit(CPony* pony)
{
}

void CPonyGallop::Walk(CPony* pony)
{
	Set_PonyState(pony, CPonyWalk::GetInstance());
}

void CPonyGallop::Trot(CPony* pony)
{
	Set_PonyState(pony, CPonyTrot::GetInstance());
}

void CPonyGallop::Idle(CPony* pony)
{
	Set_PonyState(pony, CPonyIdle::GetInstance());
}

void CPonyGallop::Gallop(CPony* pony)
{

}

void CPonyGallop::GallopFast(CPony* pony)
{
	Set_PonyState(pony, CPonyGallopFast::GetInstance());
}

void CPonyGallop::AttackRepeat(CPony* pony)
{
	Set_PonyState(pony, CPonyAttackRepeat::GetInstance());
}

CPonyGallopFast* CPonyGallopFast::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new CPonyGallopFast();
	}

	return instance;
}

void CPonyGallopFast::Update(CPony* pony, _float fTimeDelta)
{
}

void CPonyGallopFast::Enter(CPony* pony)
{
}

void CPonyGallopFast::Exit(CPony* pony)
{
}

void CPonyGallopFast::Walk(CPony* pony)
{
	Set_PonyState(pony, CPonyWalk::GetInstance());
}

void CPonyGallopFast::Trot(CPony* pony)
{
	Set_PonyState(pony, CPonyTrot::GetInstance());
}

void CPonyGallopFast::Idle(CPony* pony)
{
	Set_PonyState(pony, CPonyIdle::GetInstance());
}

void CPonyGallopFast::Gallop(CPony* pony)
{
	Set_PonyState(pony, CPonyGallop::GetInstance());
}

void CPonyGallopFast::GallopFast(CPony* pony)
{
	
}

void CPonyGallopFast::AttackRepeat(CPony* pony)
{
	Set_PonyState(pony, CPonyAttackRepeat::GetInstance());
}

CPonyAttackRepeat* CPonyAttackRepeat::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new CPonyAttackRepeat();
	}

	return instance;
}

void CPonyAttackRepeat::Update(CPony* pony, _float fTimeDelta)
{
}

void CPonyAttackRepeat::Enter(CPony* pony)
{
}

void CPonyAttackRepeat::Exit(CPony* pony)
{
}

void CPonyAttackRepeat::Walk(CPony* pony)
{
	Set_PonyState(pony, CPonyWalk::GetInstance());
}

void CPonyAttackRepeat::Trot(CPony* pony)
{
	Set_PonyState(pony, CPonyTrot::GetInstance());
}

void CPonyAttackRepeat::Idle(CPony* pony)
{
	Set_PonyState(pony, CPonyIdle::GetInstance());
}

void CPonyAttackRepeat::Gallop(CPony* pony)
{
	Set_PonyState(pony, CPonyGallop::GetInstance());
}

void CPonyAttackRepeat::GallopFast(CPony* pony)
{
	Set_PonyState(pony, CPonyGallopFast::GetInstance());
}

void CPonyAttackRepeat::AttackRepeat(CPony* pony)
{

}
