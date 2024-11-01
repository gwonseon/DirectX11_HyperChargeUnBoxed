#pragma once
#include "Pony_State.h"
#include "Client_Defines.h"

#include "Engine_Defines.h"
#include "Pony_Defines.h"

BEGIN(Client)
class CPony;
class CAttackState_Pony :    public CPony_State
{
public:
	void Enter(CPony* pony) override;
	void Update(CPony* pony, float fTimeDelta) override;
	void Exit(CPony* pony) override;

public:
	void Trot(CPony* pony) override;
	void Run(CPony* pony) override;
	void Attack(CPony* pony) override;
	void Walk(CPony* pony) override;
};
END
