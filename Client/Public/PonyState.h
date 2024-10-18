#pragma once
#include "Pony.h"

BEGIN(Client)
class CPonyState
{

public:
	virtual void Walk(CPony* pony) = 0;  // 발 따로 걷기
	virtual void Trot(CPony* pony) = 0;   // 우아하게 걷기
	virtual void Idle(CPony* pony) = 0;
	virtual void Gallop(CPony* pony) = 0;  // 두 발 같이 걷기
	virtual void GallopFast(CPony* pony) = 0;		// 두 발 같이 뛰기
	virtual void AttackRepeat(CPony* pony) = 0; // 공격

protected:
	void Set_PonyState(CPony* pony, CPonyState* state)
	{
		pony->Set_PonyState(state);
	}	
public:
	virtual void Update(CPony* pony, _float fTimeDelta)		= 0;
	virtual void Enter(CPony* pony)							 = 0;
	virtual void Exit(CPony* pony)							= 0;

}; 


class CPonyWalk : public CPonyState
{
	static CPonyWalk* instance;

public:
	static CPonyWalk* GetInstance();
	virtual void Update(CPony* pony, _float fTimeDelta);
	virtual void Enter(CPony* pony);
	virtual void Exit(CPony* pony);

public:
	virtual void Walk(CPony* pony);  // 발 따로 걷기
	virtual void Trot(CPony* pony);   // 우아하게 걷기
	virtual void Idle(CPony* pony);
	virtual void Gallop(CPony* pony);  // 두 발 같이 걷기
	virtual void GallopFast(CPony* pony);		// 두 발 같이 뛰기
	virtual void AttackRepeat(CPony* pony); // 공격


private:
	_float fWalkingTime = 0.f;
};

class CPonyTrot : public CPonyState
{
	static CPonyTrot* instance;

public:
	static CPonyTrot* GetInstance();
	virtual void Update(CPony* pony, _float fTimeDelta);
	virtual void Enter(CPony* pony);
	virtual void Exit(CPony* pony);

public:


	virtual void Walk(CPony* pony);  // 발 따로 걷기
	virtual void Trot(CPony* pony);   // 우아하게 걷기
	virtual void Idle(CPony* pony);
	virtual void Gallop(CPony* pony);  // 두 발 같이 걷기
	virtual void GallopFast(CPony* pony);		// 두 발 같이 뛰기
	virtual void AttackRepeat(CPony* pony); // 공격

};


class CPonyIdle : public CPonyState
{
	static CPonyIdle* instance;

public:
	static CPonyIdle* GetInstance();
	virtual void Update(CPony* pony, _float fTimeDelta);
	virtual void Enter(CPony* pony);
	virtual void Exit(CPony* pony);

public:


	virtual void Walk(CPony* pony);  // 발 따로 걷기
	virtual void Trot(CPony* pony);   // 우아하게 걷기
	virtual void Idle(CPony* pony);
	virtual void Gallop(CPony* pony);  // 두 발 같이 걷기
	virtual void GallopFast(CPony* pony);		// 두 발 같이 뛰기
	virtual void AttackRepeat(CPony* pony); // 공격

};


class CPonyGallop : public CPonyState
{
	static CPonyGallop* instance;

public:
	static CPonyGallop* GetInstance();
	virtual void Update(CPony* pony, _float fTimeDelta);
	virtual void Enter(CPony* pony);
	virtual void Exit(CPony* pony);

public:


	virtual void Walk(CPony* pony);  // 발 따로 걷기
	virtual void Trot(CPony* pony);   // 우아하게 걷기
	virtual void Idle(CPony* pony);
	virtual void Gallop(CPony* pony);  // 두 발 같이 걷기
	virtual void GallopFast(CPony* pony);		// 두 발 같이 뛰기
	virtual void AttackRepeat(CPony* pony); // 공격

};


class CPonyGallopFast : public CPonyState
{
	static CPonyGallopFast* instance;

public:
	static CPonyGallopFast* GetInstance();
	virtual void Update(CPony* pony, _float fTimeDelta);
	virtual void Enter(CPony* pony);
	virtual void Exit(CPony* pony);

public:


	virtual void Walk(CPony* pony);  // 발 따로 걷기
	virtual void Trot(CPony* pony);   // 우아하게 걷기
	virtual void Idle(CPony* pony);
	virtual void Gallop(CPony* pony);  // 두 발 같이 걷기
	virtual void GallopFast(CPony* pony);		// 두 발 같이 뛰기
	virtual void AttackRepeat(CPony* pony); // 공격

};

class CPonyAttackRepeat : public CPonyState
{
	static CPonyAttackRepeat* instance;

public:
	static CPonyAttackRepeat* GetInstance();
	virtual void Update(CPony* pony, _float fTimeDelta);
	virtual void Enter(CPony* pony);
	virtual void Exit(CPony* pony);

public:


	virtual void Walk(CPony* pony);  // 발 따로 걷기
	virtual void Trot(CPony* pony);   // 우아하게 걷기
	virtual void Idle(CPony* pony);
	virtual void Gallop(CPony* pony);  // 두 발 같이 걷기
	virtual void GallopFast(CPony* pony);		// 두 발 같이 뛰기
	virtual void AttackRepeat(CPony* pony); // 공격

};


END
