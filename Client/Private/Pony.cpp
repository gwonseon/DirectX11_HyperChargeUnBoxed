#include "stdafx.h"
#include "..\Public\Pony.h"

#include "GameInstance.h"
#include <PonyState.h>




CPony::CPony(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CPony::CPony(const CPony& Prototype)
	: CMonster{ Prototype }
{
	
}

HRESULT CPony::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPony::Initialize(void* pArg)
{
	PONY_DESC* pDesc = static_cast<PONY_DESC*>(pArg);
	m_vecTargetPos = pDesc->vecTargetPos;
	m_matPlayerWorld = pDesc->matPlayerWorld;
	m_matBrainCoreWorld = pDesc->matBrainCoreWorld;
	m_iModelIndex = pDesc->iModelComponentIndex;
	m_eLevel = pDesc->eID;
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_Animation(0, true);
	//pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider_Sphere"), 0, CPlayer::TPS_PART_KATANA));


	current = CPonyIdle::GetInstance();
	return S_OK;
}

void CPony::Priority_Update(_float fTimeDelta)
{
	vPlayerPos = XMVectorSet(m_matPlayerWorld->_41, m_matPlayerWorld->_42, m_matPlayerWorld->_43, 1.0f);
	m_bAnimState = m_pModelCom->Play_Animation(fTimeDelta, false);
}

void CPony::Update(_float fTimeDelta)
{

	current->Update(this, fTimeDelta);

	// Handle distance-based behavior (e.g., switch to GallopFast if far from player)
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fDistance = m_pTransformCom->Cal_Distance_vec(vPlayerPos, vPos);
	if(fDistance < 5000.f)
	{
		m_pTransformCom->LookAt(vPlayerPos);
		if (fDistance > 6.f) {
			// 처음에 걷다가 몇 초 뒤 뛰는 모션으로 변경
			if (m_bWalkState == true)
				Walk();
			m_pTransformCom->Go_Straight(fTimeDelta + m_fRunSpeed);
		}
		else {
			AttackRepeat();
			m_bWalkState = true;
		}
	}
	
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	
}

void CPony::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

}

HRESULT CPony::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

HRESULT CPony::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	const _wstring Model_Component = TEXT("Prototype_Component_Model_Anim");
	const _wstring Model_Component_Result = Model_Component + to_wstring(m_iModelIndex);
	if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_Sphere::BOUND_SPHERE_DESC			SphereDesc{};
	SphereDesc.fRadius = 1.5f;
	SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPony::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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

CPony* CPony::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPony* pInstance = new CPony(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPony");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPony::Clone(void* pArg)
{
	CPony* pInstance = new CPony(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPony");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPony::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}

// 상태 설정해주기 Enter Exit 여기서 접근함
void CPony::Set_PonyState(CPonyState* state)
{
	if (current)
		current->Exit(this);
	current = state;
	if (current) 
		current->Enter(this); 
}

void CPony::Walk()
{
	current->Walk(this);	
	m_bWalkState = true;
}

void CPony::Trot()
{
	current->Trot(this);
	m_pModelCom->Set_Animation(PONY_Trot, true);
}

void CPony::Idle()
{
	current->Idle(this);
	m_pModelCom->Set_Animation(PONY_Idle01, true);
}

void CPony::Gallop()
{
	current->Gallop(this);
	m_pModelCom->Set_Animation(PONY_Gallop, true);
}

void CPony::GallopFast()
{
	current->GallopFast(this);
	m_pModelCom->Set_Animation(PONY_GallopFast, true);
}

void CPony::AttackRepeat()
{
	current->AttackRepeat(this);
	m_pModelCom->Set_Animation(PONY_AttackRepeat, false);
}
