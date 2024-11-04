#include "stdafx.h"
#include "..\Public\Weapon_Katana.h"

#include "GameInstance.h"
#include "Player.h"

CWeapon_Katana::CWeapon_Katana(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWeapon_Katana::CWeapon_Katana(const CWeapon_Katana& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CWeapon_Katana::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Katana::Initialize(void* pArg)
{
	KATANA_DESC* pDesc = static_cast<KATANA_DESC*>(pArg);

	m_pParentState = pDesc->pParentState;
	m_pSocketMatrix = pDesc->pSocketMatrix;
	m_eLevelID = pDesc->m_eLevelID;
	/* 추가적으로 초기화가 필요하다면 수행해준다. */

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_iViewState = pDesc->m_iViewState;

	Position = { -0.0599999f, 0.639999f, 0.0599998f };
	Scale = { 3.33 };
	Rotation = { 101.3,57.5,-68 };
	m_pTransformCom->Set_Scaling(Scale, Scale, 2.f);
	m_pTransformCom->Rotation(XMConvertToRadians(Rotation.x), XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(Position.x, Position.y, Position.z, 1.f));


	return S_OK;
}

void CWeapon_Katana::Priority_Update(_float fTimeDelta)
{
	if(m_bKatanaState == true)
	{


	}
}

void CWeapon_Katana::Update(_float fTimeDelta)
{
	if (m_bKatanaState == true)
	{
		_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

		for (size_t i = 0; i < 3; i++)
			SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

		m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));
	}
}



void CWeapon_Katana::Late_Update(_float fTimeDelta)
{
	if(m_bKatanaState == true)
	{
		
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
			return;
	}
}

HRESULT CWeapon_Katana::Render()
{
	if (m_bKatanaState == true)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
				return E_FAIL; 
			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;

			m_pModelCom->Render(i);
		}
#ifdef _DEBUG
		m_pColliderCom->Render();
#endif

	}
	return S_OK;
}

HRESULT CWeapon_Katana::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(m_eLevelID ,TEXT("Prototype_Component_Model_Weapon8"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider_Sphere */
	CBounding_Sphere::BOUND_SPHERE_DESC			SphereDesc{};

	SphereDesc.fRadius = 0.7f;
	SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(m_eLevelID, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CWeapon_Katana::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
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
		return E_FAIL;

	return S_OK;
}

CWeapon_Katana* CWeapon_Katana::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Katana* pInstance = new CWeapon_Katana(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Katana");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon_Katana::Clone(void* pArg)
{
	CWeapon_Katana* pInstance = new CWeapon_Katana(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CWeapon_Katana");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Katana::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
