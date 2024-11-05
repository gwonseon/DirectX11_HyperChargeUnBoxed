#pragma once
#include "Base.h"
#include <Layer.h>


BEGIN(Engine)

class CUIManager final : public CBase
{
private:
	CUIManager();
	virtual ~CUIManager() = default;


public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);


	void CircleGauge_Interaction(CLayer* Item, CLayer* UI);

private:
	class CGameInstance* m_pGameInstance = { nullptr };



public:
	static CUIManager* Create();
	virtual void Free() override;

};

END