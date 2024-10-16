#include "stdafx.h"
#include "..\Public\Monster_Path.h"


#include "Camera_Free.h"
#include "Monster.h"
#include "Level_Loading.h"
CMonster_Path::CMonster_Path(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CMonster_Path::Initialize()
{
	ShowCursor(true);
	return S_OK;
}

void CMonster_Path::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

}

HRESULT CMonster_Path::Render()
{
	return E_NOTIMPL;
}

CMonster_Path* CMonster_Path::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

void CMonster_Path::Free()
{
}
