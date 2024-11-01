#pragma once
#include "Client_Defines.h"
#include "Engine_Defines.h"

BEGIN(Client)

class CPony;
class CPony_State
{
public:
	virtual void Enter(CPony* pony)						= 0;
	virtual void Update(CPony* pony, float fTimeDelta)	= 0;
	virtual void Exit(CPony* pony)						= 0;
	virtual ~CPony_State() = default;

public:
	virtual void Trot(CPony* pony)						= 0;
	virtual void Run(CPony* pony)						= 0;
	virtual void Attack(CPony* pony)					= 0;
	virtual void Walk(CPony* pony)						= 0;
};

END