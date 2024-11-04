#pragma once
#include "Client_Defines.h"
#include "Engine_Defines.h"


BEGIN(Client)

class CRifleMan;
class CRifleMan_State
{
public:
	virtual void Enter(CRifleMan* rifleman) = 0;
	virtual void Update(CRifleMan* rifleman, float fTimeDelta) = 0;
	virtual void Exit(CRifleMan* rifleman) = 0;
	virtual ~CRifleMan_State() = default;

public:
	virtual void Move(CRifleMan* rifleman) = 0;
	virtual void Fire(CRifleMan* rifleman) = 0;
};

END