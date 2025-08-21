
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

	m_pParentState = pDesc->pParentState;
	m_pSocketMatrix = pDesc->pSocketMatrix;

	/* 추가적으로 초기화가 필요하다면 수행해준다. */
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if(FAILED(Add_Components()))
		return E_FAIL;


	Position = {3.8f,2.f,0.3f};
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,XMVectorSet(Position.x,Position.y,Position.z,1.f));
	//m_pTransformCom->Set_Scaling(3.f, 3.f, 3.f);

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

	//if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_NUMPAD1))
	//{
	//	Position.x += 0.1f;
	//}
	//if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_NUMPAD2))
	//{
	//	Position.y += 0.1f;
	//}
	//if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_NUMPAD3))
	//{
	//	Position.z += 0.1f;
	//}
	//if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_NUMPAD4))
	//{
	//	Position.x -= 0.1f;
	//}
	//if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_NUMPAD5))
	//{
	//	Position.y -= 0.1f;
	//}
	//if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_NUMPAD6))
	//{
	//	Position.z -= 0.1f;
	//}
	//
	//if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_P))
	//{
	//	cout << Position.x << "    " << Position.y << "    " << Position.z << endl;
	//}

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(Position.x, Position.y, Position.z, 1.f));

}

void CFPS_Pivot::Late_Update(_float fTimeDelta)
{

	if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND,this)))
		return;
}

HRESULT CFPS_Pivot::Render()
{


	return S_OK;
}

HRESULT CFPS_Pivot::Add_Components()
{
	/* For.Com_Shader */
	if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"),reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;

}

HRESULT CFPS_Pivot::Bind_ShaderResources()
{
	if(FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix",&m_WorldMatrix)))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition",m_pGameInstance->Get_CamPosition(),sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if(nullptr == pLightDesc)
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir",&pLightDesc->vDirection,sizeof(_float4))))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse",&pLightDesc->vDiffuse,sizeof(_float4))))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient",&pLightDesc->vAmbient,sizeof(_float4))))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular",&pLightDesc->vSpecular,sizeof(_float4))))
		return E_FAIL;

	return S_OK;

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

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}