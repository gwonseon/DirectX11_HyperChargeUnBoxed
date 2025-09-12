#include "stdafx.h"
#include "Player2_Parts.h"


CPlayer2_Parts::CPlayer2_Parts(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
	: CPartObject{pDevice,pContext}
{}

CPlayer2_Parts::CPlayer2_Parts(const CPlayer2_Parts & Prototype)
	: CPartObject{Prototype}
{}

HRESULT CPlayer2_Parts::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer2_Parts::Initialize(void * pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayer2_Parts::Priority_Update(_float fTimeDelta)
{}

void CPlayer2_Parts::Update(_float fTimeDelta)
{}

void CPlayer2_Parts::Late_Update(_float fTimeDelta)
{}

HRESULT CPlayer2_Parts::Render()
{
	return S_OK;
}

CPlayer2_Parts * CPlayer2_Parts::Create(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
{
	CPlayer2_Parts* pInstance = new CPlayer2_Parts(pDevice,pContext);
	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer2_Parts");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer2_Parts::Clone(void * pArg)
{
	CPlayer2_Parts* pInstance = new CPlayer2_Parts(*this);
	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer2_Parts");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer2_Parts::Free()
{
	__super::Free();
}
