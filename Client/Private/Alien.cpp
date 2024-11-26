#include "stdafx.h"
#include "..\Public\Alien.h"


#include "GameInstance.h"
#include <Effect_Electricity.h>
CAlien::CAlien(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CAlien::CAlien(const CAlien& Prototype)
	: CMonster{ Prototype }

{
}

HRESULT CAlien::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAlien::Initialize(void* pArg)
{
	ALIEN_DESC* pDesc = static_cast<ALIEN_DESC*>(pArg);
	m_vecTargetPos = pDesc->vecTargetPos;
	m_matPlayerWorld = pDesc->matPlayerWorld;
	m_matBrainCoreWorld = pDesc->matBrainCoreWorld;

	pDesc->fScale = _float3(2.f, 2.f, 2.f);
	pDesc->fSpeedPerSec = 10.f;

	m_iCell_Idx = pDesc->iCell_Idx;
	m_iModelIndex = ANIM_ALIEN;
	m_eLevel = pDesc->eID;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_Animation(0, true);
	m_fAttack = 10.f;
	m_fEnergy = 0.f;
	m_fPrevHp = m_fHp = 60.f;
	
	m_bAttackState = true;
	return S_OK;
}

void CAlien::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	m_vecPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pModelCom->Set_Animation(0, true);
	vPlayerPos = XMVectorSet(m_matPlayerWorld->_41, m_matPlayerWorld->_42, m_matPlayerWorld->_43, 1.0f);
	if (XMVectorGetY(vPlayerPos) <= (XMVectorGetY(m_vecPosition) + 2.f))
		m_pTransformCom->LookAt(vPlayerPos);
	m_bAnimState = m_pModelCom->Play_Animation(fTimeDelta, false);
	
	
	_bool bKatanaCheck = m_pGameInstance->Get_KatanaState();
	if (m_fPrevHp != m_fHp && bKatanaCheck == true)
	{
		CEffect_Electricity::EFFECT_ELECTRICITY_DESC pElectricity{};
		pElectricity.eLevel = m_eLevel;
		pElectricity.fScale = _float3{ 10.f, 10.f,10.f };
		pElectricity.iTexNum = 3;
		pElectricity.vecPos = &m_vecPosition;
		m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel, TEXT("Effect_Layer"), TEXT("Prototype_GameObject_Effect_Lightning"), &pElectricity);
		m_fPrevHp = m_fHp;
	}
	else if (bKatanaCheck == false)
	{
		m_fPrevHp = m_fHp;
	}

}

void CAlien::Update(_float fTimeDelta)
{
	
	_float fDistance = m_pTransformCom->Cal_Distance_vec(vPlayerPos, m_vecPosition);
	if (fDistance > 4.f)
	{
		m_pTransformCom->Go_Straight_Nav(fTimeDelta,m_pNavigationCom);
	}
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	// 넉백이 True일 때 넉백 모션하게 하기
	if (m_bAttacked == true)
	{
		m_fKnockBack_Height = XMVectorGetY(m_vecPosition);
		m_fKnockBack_Power = 8.f;
		m_bKnockBacking = true;
		m_bAttacked = false;
	}

}

void CAlien::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	if (m_bOverlab_SameLayer == true || m_bOverlab_DifferentLayer == true)
	{
		m_vecPosition += m_vecDirection * fTimeDelta * 0.5f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);
	}
	// 넉백이 true일 때 넉백 모션
	if (m_bKnockBacking == true)
	{
		_vector vKnockBack_DIr = m_vecPosition - vPlayerPos; // 플레이어 방향으로부터 반대방향으로 날아가기
		vKnockBack_DIr = XMVector3Normalize(vKnockBack_DIr);
		if (m_pTransformCom->KnockBack(fTimeDelta, vKnockBack_DIr, m_fKnockBack_Power, m_fKnockBack_Height) == true)
		{
			// 넉백 끝남
			m_bCanAttacked = true;
			m_bKnockBacking = false;
		}

	}
}

HRESULT CAlien::Render()
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
	if (m_bDead)
		return S_OK;
	m_pColliderCom->Render();
#endif

	return S_OK;
}

HRESULT CAlien::Add_Components()
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
	SphereDesc.fRadius = 1.8f;
	SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);
	if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
		return E_FAIL;

	
	// For.Com_Navigation
	CNavigation::NAVIGATION_DESC		Desc{};
	Desc.iCurrentCellIndex = m_iCell_Idx;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAlien::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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

CAlien* CAlien::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAlien* pInstance = new CAlien(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAlien");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAlien::Clone(void* pArg)
{
	CAlien* pInstance = new CAlien(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CAlien");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAlien::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNavigationCom);

}
