#include "stdafx.h"
#include "..\Public\TruckBody.h"

#include "GameInstance.h"
#include "Missile_Truck.h"

CTruckBody::CTruckBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CTruckBody::CTruckBody(const CTruckBody& Prototype)
	: CPartObject{ Prototype }
{
}


HRESULT CTruckBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTruckBody::Initialize(void* pArg)
{
	TRUCKBODY_DESC* pDesc = static_cast<TRUCKBODY_DESC*>(pArg);
	m_eLevelID = pDesc->m_eLevelID;
	m_pPlayer = pDesc->pPlayer;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_bDontDestroy = true;
	m_bCanAttacked = false;
	return S_OK;
}

void CTruckBody::Priority_Update(_float fTimeDelta)
{
}

void CTruckBody::Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pParentMatrix) * m_pTransformCom->Get_WorldMatrix());
	m_vecPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

}

void CTruckBody::Late_Update(_float fTimeDelta)
{
	if (m_bDead == false)
	{
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
			return;
		
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_HEIGHT, this)))
			return;

	}
}

HRESULT CTruckBody::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CTruckBody::Render_Height()
{

	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	
	_float4x4			ViewMatrix, ProjMatrix;
	CLayer* pPlayerLayer = (m_pGameInstance->Find_Layer(LEVEL_YARD, TEXT("Layer_Player")));
	CPlayer* pPlayer = static_cast<CPlayer*>(pPlayerLayer->Get_GameObject_List().front());
	_vector PlayerPos = pPlayer->Get_Position();
	_matrix			matView = XMMatrixIdentity();
	matView.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	matView.r[1] = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	matView.r[2] = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	matView.r[3] = XMVectorSet(XMVectorGetX(PlayerPos), XMVectorGetY(PlayerPos) + 6.f, XMVectorGetZ(PlayerPos), 1.f);

	XMStoreFloat4x4(&ViewMatrix, XMMatrixInverse(nullptr, matView));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixOrthographicLH(200.f, 200.f, 0.f, 30.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	_float fFar = m_pGameInstance->Get_CameraFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
		return E_FAIL;
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(6))) // 무조건 그림
			return E_FAIL;
		m_pModelCom->Render(i);
	}

	return S_OK;
}
HRESULT CTruckBody::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eLevelID, TEXT("Prototype_Component_Model_Trap16"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CTruckBody::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fFar = m_pGameInstance->Get_CameraFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;*/

	return S_OK;
}

CTruckBody* CTruckBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTruckBody* pInstance = new CTruckBody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTruckBody");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTruckBody::Clone(void* pArg)
{
	CTruckBody* pInstance = new CTruckBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CTruckBody");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTruckBody::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);

}
