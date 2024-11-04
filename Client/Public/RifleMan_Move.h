#pragma once
#include "Client_Defines.h"
#include "Engine_Defines.h"
#include "RifleMan_State.h"
#include "RifleMan_Defines.h"


BEGIN(Client)

class CRifleMan;
class CRifleMan_Move : public CRifleMan_State
{
public:
	void Enter(CRifleMan* rifleman) override;
	void Update(CRifleMan* rifleman, float fTimeDelta) override;
	void Exit(CRifleMan* rifleman) override;

public:
	void Move(CRifleMan* rifleman) override;
	void Fire(CRifleMan* rifleman) override;
};

END