#include "stdafx.h"
#include "..\Public\Helicopter.h"

#include "GameInstance.h"
CHelicopter::CHelicopter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CHelicopter::CHelicopter(const CHelicopter& Prototype)
	: CMonster{ Prototype }
{
}

HRESULT CHelicopter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHelicopter::Initialize(void* pArg)
{
	HELICOPTER_DESC* pDesc = static_cast<HELICOPTER_DESC*>(pArg);
	m_vecTargetPos = pDesc->vecTargetPos;
	m_matPlayerWorld = pDesc->matPlayerWorld;
	m_matBrainCoreWorld = pDesc->matBrainCoreWorld;
	m_iModelIndex = pDesc->iModelComponentIndex;
	m_eLevel = pDesc->eID;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_Animation(1, true);
	m_bAttackState = true;
//	pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider_Sphere"), 0, CPlayer::TPS_PART_KATANA));
	m_fHp = 100.f;
	m_fEnergy = 0.f;
	return S_OK;
}

void CHelicopter::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
	m_bAnimState = m_pModelCom->Play_Animation(fTimeDelta, false);
	if (m_bCanAttacked == false)
		m_fCurrentTime += fTimeDelta;
}

void CHelicopter::Update(_float fTimeDelta)
{
	// 데미지 입는 타이밍 딜레이로 맞춤
	if (m_fCurrentTime >= m_fDamaged_DelayTime)
	{
		m_bCanAttacked = true;
		m_fCurrentTime = 0.f;
	}

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fDistance = m_pTransformCom->Cal_Distance_vec_No_Height(*m_vecTargetPos, vPos);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(vPos), 10.f, XMVectorGetZ(vPos),1.f));

	if (fDistance >= 90.f)
	{
		m_pTransformCom->LookAt(*m_vecTargetPos);
		m_pModelCom->Set_Animation(HELICOPTER_DIORAMA, true);
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	else
	{
		// Turn 함수로 하면 좋을듯, 이동 좌표는 직접 찍을 것이기 때문에 방향 신경쓰지 말자 
		if (XMConvertToRadians(m_fRotation) <= 90.f)
		{
			m_fRotation += fTimeDelta * 10.f;
		}
			
		m_pTransformCom->Rotation(0.f, XMConvertToRadians(m_fRotation), 0.f);
		m_pModelCom->Set_Animation(HELICOPTER_DIORAMA, true);
	}
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CHelicopter::Late_Update(_float fTimeDelta)
{
//	m_pColliderCom->Intersect(pTargetCollider);

	__super::Late_Update(fTimeDelta);
}

HRESULT CHelicopter::Render()
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

HRESULT CHelicopter::Add_Components()
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

	CBounding_Sphere::BOUND_SPHERE_DESC			SphereDesc{};
	SphereDesc.fRadius = 1.2f;
	SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CHelicopter::Bind_ShaderResources()
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

CHelicopter* CHelicopter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHelicopter* pInstance = new CHelicopter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHelicopter");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHelicopter::Clone(void* pArg)
{
	CHelicopter* pInstance = new CHelicopter(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CHelicopter");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHelicopter::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
