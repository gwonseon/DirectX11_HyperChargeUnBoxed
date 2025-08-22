
#include "stdafx.h"
#include "..\Public\FPS_Pivot.h"

#include "GameInstance.h"
#include "Player.h"
CFPS_Pivot::CFPS_Pivot(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
	: CPartObject{pDevice,pContext}
{}

CFPS_Pivot::CFPS_Pivot(const CFPS_Pivot& Prototype)
	: CPartObject{Prototype}
{}

HRESULT CFPS_Pivot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFPS_Pivot::Initialize(void* pArg)
{
	FPSPIVOT_DESC* pDesc = static_cast<FPSPIVOT_DESC*>(pArg);
	m_pSocketMatrix = pDesc->pSocketMatrix;

	/* 추가적으로 초기화가 필요하다면 수행해준다. */
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,XMVectorSet(Position.x,Position.y,Position.z,1.f));

	return S_OK;

}

void CFPS_Pivot::Priority_Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for(size_t i = 0; i < 3; i++)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_WorldMatrix,m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
	m_vecFPS_CamPos = XMVectorSet(m_WorldMatrix._41,m_WorldMatrix._42,m_WorldMatrix._43,1.f);
}

void CFPS_Pivot::Update(_float fTimeDelta)
{
}

void CFPS_Pivot::Late_Update(_float fTimeDelta)
{
}

CFPS_Pivot* CFPS_Pivot::Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
{
	CFPS_Pivot* pInstance = new CFPS_Pivot(pDevice,pContext);

	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFPS_Pivot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFPS_Pivot::Clone(void* pArg)
{
	CFPS_Pivot* pInstance = new CFPS_Pivot(*this);

	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CFPS_Pivot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFPS_Pivot::Free()
{
	__super::Free();

}