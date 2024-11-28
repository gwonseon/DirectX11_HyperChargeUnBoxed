#include "stdafx.h"
#include "..\Public\Helicopter.h"

#include "GameInstance.h"
#include <Monster_Bullet.h>
#include <Effect_Explosion_Tank.h>

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
	pDesc->fSpeedPerSec = 10.f;
	pDesc->fScale = _float3(3.f, 3.f, 3.f);

	m_vecTargetPos = pDesc->vecTargetPos;
	m_pBuild = pDesc->pBuild;
	m_iModelIndex = ANIM_HELICOPTER;
	m_eLevel = pDesc->eID;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_Animation(1, true);
	m_bAttackState = true;
	m_fHp = 100.f;
	m_fEnergy = 0.f;
	return S_OK;
}

void CHelicopter::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
	if (m_bDeadState == true)
		return;

	m_bAnimState = m_pModelCom->Play_Animation(fTimeDelta, false);
	if (m_bCanAttacked == false)
		m_fCurrentTime += fTimeDelta;
}

void CHelicopter::Update(_float fTimeDelta)
{

	m_vecPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (m_bDead == true)
		return;
	if (m_bDeadState == true)
	{
		DeadMotion(fTimeDelta);
		return;
	}
	// 데미지 입는 타이밍 딜레이로 맞춤
	if (m_fCurrentTime >= m_fDamaged_DelayTime)
	{
		m_bCanAttacked = true;
		m_fCurrentTime = 0.f;
	}

	_float fDistance = m_pTransformCom->Cal_Distance_vec_No_Height(*m_vecTargetPos, m_vecPosition);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition = XMVectorSet(XMVectorGetX(m_vecPosition), 20.f, XMVectorGetZ(m_vecPosition),1.f));

	if (fDistance >= 350.f)
	{
		_vector vLookPos = *m_vecTargetPos;
		vLookPos = XMVectorSetY(vLookPos, 20.f);
		m_pTransformCom->LookAt(vLookPos);
		m_pModelCom->Set_Animation(HELICOPTER_DIORAMA, true);
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	else
	{
		// Turn 함수로 하면 좋을듯, 이동 좌표는 직접 찍을 것이기 때문에 방향 신경쓰지 말자 
		if (XMConvertToRadians(m_fRotation) <= XMConvertToRadians(90.f))
		{
			
			m_fRotation += fTimeDelta * 20.f;
		}
		else // 다 돌았을 때
		{
			// 총알 생성 
			// 몇 초에 한 번씩 총알이 생성되게 만들면 되지 않을까 
			if (m_fShot_Time_Delay >= 4.f)
			{
				if(m_iShot_Count < 3)
				{
					_float3 fPos{};
					XMStoreFloat3(&fPos, m_vecPosition);
					CMonster_Bullet::MONSTER_BULLET_DESC Desc{};
					Desc.eID = m_eLevel;
					Desc.fPosition = fPos;
					Desc.m_iModelNumber = 1;
					Desc.eType = CMonster_Bullet::HELICOPTER_BULLET;
					Desc.vTargetPos = *m_vecTargetPos;
					Desc.m_pBuild = m_pBuild;
					static_cast<CMonster_Bullet*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel, TEXT("MonsterBullet_Layer"), TEXT("Prototype_GameObject_MonsterBullet"), &Desc));
					m_iShot_Count++;
				}
				else
				{
					m_fShot_Time_Delay = 0.f;
					m_iShot_Count = 0;
				}
			}
			m_fShot_Time_Delay += fTimeDelta;
		}
			
		m_pTransformCom->Rotation(0.f, XMConvertToRadians(m_fRotation), 0.f);
		m_pModelCom->Set_Animation(HELICOPTER_DIORAMA, true);
	}
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CHelicopter::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	if (m_bDeadState == true)
		return;
	
	if (m_bOverlab_SameLayer == true || m_bOverlab_DifferentLayer == true)
	{
		m_vecPosition += m_vecDirection * fTimeDelta * 0.5f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);
	}

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

	if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Collider_Sphere"),
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

	_float fFar = m_pGameInstance->Get_CameraFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
		return E_FAIL;


	return S_OK;
}

void CHelicopter::DeadMotion(_float fTimeDelta)
{
	if (XMVectorGetY(m_vecPosition) < 0.f)
	{
		CEffect_Explosion_Tank::EFFECT_Tank_Explosion_DESC Effect{};
		Effect.eLevel = m_eLevel;
		Effect.eType = CEffect_Explosion_Tank::EXPLOSION_TANK_DEAD;
		Effect.fScale = _float3{ 20.f, 20.f, 20.f };
		Effect.fPosition = _float3{ XMVectorGetX(m_vecPosition), XMVectorGetY(m_vecPosition) ,XMVectorGetZ(m_vecPosition) };
		m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Tank_Explosion"), &Effect);
		m_bDead = true;
	}

	m_fAcc += fTimeDelta * 2.f;

	m_vecPosition -= XMVectorSet(0.f, 1.f, 0.f, 0.f) * fTimeDelta * (5.f + m_fAcc *3.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);
	m_pTransformCom->Turn(0.f, 1.f, 0.f, fTimeDelta * (1.f + m_fAcc * 0.5f));

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
