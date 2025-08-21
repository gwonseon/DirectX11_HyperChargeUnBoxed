#include "stdafx.h"
#include "..\Public\Blimp.h"

#include "GameInstance.h"
#include <Monster_Bullet.h>

CBlimp::CBlimp(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
	: CMonster{pDevice,pContext}
{

}

CBlimp::CBlimp(const CBlimp& Prototype)
	: CMonster{Prototype}
{

}

HRESULT CBlimp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlimp::Initialize(void* pArg)
{
	BLIMP_DESC* pDesc = static_cast<BLIMP_DESC*>(pArg);

	pDesc->fSpeedPerSec = 5.f;
	pDesc->fScale = _float3(3.f,3.f,3.f);

	m_vecTargetPos = pDesc->vecTargetPos;
	m_pBuild = pDesc->pBuild;
	m_iModelIndex = ANIM_HELICOPTER;
	m_eLevel = pDesc->eID;

	if(FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if(FAILED(Add_Components()))
		return E_FAIL;

	m_bAttackState = true;
	//m_fHp = 3000.f;
	m_fHp = 100.f;
	m_fEnergy = 0.f;

	return S_OK;
}

void CBlimp::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
	if(m_bDead == true)
	{
		m_pModelCom->Set_Animation(0,true); // Á×´Â ¸ð¼Ç »ÓÀÓ
		m_pModelCom->Play_Animation(fTimeDelta,false);
	}
}

void CBlimp::Update(_float fTimeDelta)
{

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

}

void CBlimp::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CBlimp::Render()
{
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for(size_t i = 0; i < iNumMeshes; i++)
	{
		if(FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom,i,aiTextureType_DIFFUSE,0,"g_DiffuseTexture")))
			return E_FAIL;

		if(FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom,i,"g_BoneMatrices")))
			return E_FAIL;

		if(FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}
	#ifdef _DEBUG

	m_pColliderCom->Render();
	#endif
	return S_OK;
}

HRESULT CBlimp::Add_Components()
{
	if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"),reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	const _wstring Model_Component = TEXT("Prototype_Component_Model_Anim");
	const _wstring Model_Component_Result = Model_Component + to_wstring(ANIM_BLIMP);
	if(FAILED(__super::Add_Component(m_eLevel,Model_Component_Result,
		TEXT("Com_Model"),reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_Sphere::BOUND_SPHERE_DESC			SphereDesc{};
	SphereDesc.fRadius = 1.2f;
	SphereDesc.vCenter = _float3(0.f,SphereDesc.fRadius,0.f);

	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"),reinterpret_cast<CComponent**>(&m_pColliderCom),&SphereDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CBlimp::Bind_ShaderResources()
{
	if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom,"g_WorldMatrix")))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fFar = m_pGameInstance->Get_CameraFar();
	if(FAILED(m_pShaderCom->Bind_RawValue("g_fFar",&fFar,sizeof(float))))
		return E_FAIL;
	/* if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
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

CBlimp* CBlimp::Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
{
	CBlimp* pInstance = new CBlimp(pDevice,pContext);

	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBlimp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBlimp::Clone(void* pArg)
{
	CBlimp* pInstance = new CBlimp(*this);
	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBlimp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBlimp::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}