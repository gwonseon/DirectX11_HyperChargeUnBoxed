#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "VIBuffer_Terrain.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "Environment.h"
#include "Player.h"

BEGIN(Client)

class CMonster_Path final : public CLevel
{
private:
	CMonster_Path(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMonster_Path() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
public:
	static CMonster_Path* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

// 여기에 ImGui 몬스터 이동 경로 찍는 레벨 생성할 것
// 따로 만든 이유는 저 코드가 너무 복잡해서
// 이거 열때 찍어놓은 정보 Load하자
// 만드는 김에 사용한 오브젝트 정보도 기억해두고 꺼내오는 코드도 짜자



END