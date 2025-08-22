
#include "stdafx.h"
#include "..\Public\Pivot.h"

#include "GameInstance.h"
#include "Player.h"
CPivot::CPivot(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
	: CPartObject{pDevice,pContext}
{}

CPivot::CPivot(const CPivot& Prototype)
	: CPartObject{Prototype}
{}

HRESULT CPivot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPivot::Initialize(void* pArg)
{
	PIVOT_DESC* pDesc = static_cast<PIVOT_DESC*>(pArg);
	m_pSocketMatrix = pDesc->pSocketMatrix;

	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,XMVectorSet(Position.x,Position.y,Position.z,1.f));


	return S_OK;

}

void CPivot::Priority_Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for(size_t i = 0; i < 3; i++)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_WorldMatrix,m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
	m_vecTPS_CamPos = XMVectorSet(m_WorldMatrix._41,m_WorldMatrix._42,m_WorldMatrix._43,1.f);
}

void CPivot::Update(_float fTimeDelta)
{

}

void CPivot::Late_Update(_float fTimeDelta)
{
}




CPivot* CPivot::Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
{
	CPivot* pInstance = new CPivot(pDevice,pContext);

	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPivot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPivot::Clone(void* pArg)
{
	CPivot* pInstance = new CPivot(*this);

	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPivot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPivot::Free()
{
	__super::Free();
}