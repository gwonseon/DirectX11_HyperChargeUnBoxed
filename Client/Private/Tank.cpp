#include "stdafx.h"
#include "..\Public\Tank.h"

#include "GameInstance.h"
#include <Monster_Bullet.h>
#include <Trap_Marks.h>


CTank::CTank(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CTank::CTank(const CTank& Prototype)
	: CMonster{ Prototype }

{
}

HRESULT CTank::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTank::Initialize(void* pArg)
{
	TANK_DESC* pDesc = static_cast<TANK_DESC*>(pArg);

	pDesc->fSpeedPerSec = 5.f;
	pDesc->fScale = _float3(3.f, 3.f, 3.f);
	m_vecTargetPos = pDesc->vecTargetPos;
	m_vecStoreTargetPos = *m_vecTargetPos;
	m_pTrapLayer = pDesc->pTrapLayer;
	m_iModelIndex = ANIM_TANK;
	m_eLevel = pDesc->eID;
	m_pBuild = pDesc->m_pBuild;
	m_iCell_Idx = pDesc->iCell_Idx;
	m_iBraincore_CellNumber = pDesc->iBraincore_CellNumber;
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_Animation(1, true);
	
	m_fHp = 100.f;
	m_fEnergy = 0.f;
	m_fAttack = 0.f; //  탱크 자체의 공격력은 0, 미사일이 공격력 갖게 하자

	Path = m_pTransformCom->PathFind(0.f, m_pNavigationCom, m_iCell_Idx, m_iBraincore_CellNumber);
	return S_OK;
}


void CTank::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (m_bCanAttacked == false)
		m_fCurrentTime += fTimeDelta;
	m_fTime_For_Target += fTimeDelta;


	
}

void CTank::Update(_float fTimeDelta)
{	
	if (m_bDead == true)
		return;
	// 데미지 입는 타이밍 딜레이로 맞춤
	if(m_fCurrentTime >= m_fDamaged_DelayTime)
	{
		m_bCanAttacked = true;
		m_fCurrentTime = 0.f;
	}



	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	// 트랩이 있을 경우에 해당 트랩을 공격하도록 타겟을 변경해줌
	if (m_fTime_For_Target >= 3.f) // 항상 검사하기엔 검사량이 많아서 검사 빈도수를 줄여줌
	{
		m_fTime_For_Target = 0.f;
		_int iCheck_Count = 0;
		for (auto pTrap : m_pTrapLayer->Get_GameObject_List())
		{
			if (static_cast<CTrap_Marks*>(pTrap)->Get_Build_Done() == true)
			{
				m_vecNewTargetPos = static_cast<CTrap_Marks*>(pTrap)->Get_TrapPos();
				// 공격 사거리보다 먼거리까지 검사해야함
				if (m_pTransformCom->Cal_Distance_vec(m_vecNewTargetPos, m_vecPosition) <= 2500.f && static_cast<CTrap_Marks*>(pTrap)->Get_knockdown() == false)
				{
					m_vecTargetPos = &m_vecNewTargetPos;
					break;
				}
			}
			++iCheck_Count;
		}
		if (iCheck_Count == m_pTrapLayer->Get_GameObjectList_Size())
		{
			m_vecTargetPos = &m_vecStoreTargetPos;
		}
	}
	_float fAnimSpeed = 1.f;
	_float fDistance = m_pTransformCom->Cal_Distance_vec(*m_vecTargetPos, m_vecPosition);
	if (fDistance > 2000.f)
	{
		m_vecPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float3 fPos{};		XMStoreFloat3(&fPos, m_vecPosition);
		// 목표 Path와 현재 내 위치 사이의 거리를 파악해서 Path 바꾸기 
		if (m_pTransformCom->Cal_Distance(Path.front(), fPos) <= 200.f)
		{
			if(Path.size() > 1)
				Path.erase(Path.begin());
		}
		// Path 따라 갈 때는 Path 목표 바라보기
		m_pTransformCom->LookAt(XMVectorSet(Path.front().x, Path.front().y, Path.front().z, 1.f));
		m_pTransformCom->Go_Straight(fTimeDelta * 1.5f);
		fAnimSpeed = 1.f;
		
		m_pModelCom->Set_Animation(MONSTER_Tank_Drive, true);
		m_bFirstShot = false;
	}
	else
	{
		if (m_bAnimState == true && m_bShotOnce == false || m_bFirstShot == false)
		{
			m_bShotOnce = true;	m_bFirstShot = true;
			// 포탄 발사
			_float3 fPos{};
			XMStoreFloat3(&fPos,m_vecPosition);
			CMonster_Bullet::MONSTER_BULLET_DESC Desc{};
			Desc.eID = m_eLevel;
			Desc.fPosition = fPos;
			Desc.m_iModelNumber = 3;
			Desc.eType = CMonster_Bullet::TANK_BULLET;
			Desc.vTargetPos = *m_vecTargetPos;
			Desc.m_pBuild =  m_pBuild;
			static_cast<CMonster_Bullet*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel, TEXT("MonsterBullet_Layer"), TEXT("Prototype_GameObject_MonsterBullet"), &Desc));
		}
		m_pTransformCom->LookAt(*m_vecTargetPos);
		fAnimSpeed = 0.7f;
		m_pModelCom->Set_Animation(MONSTER_Tank_RecoilForwardFire, false);
		// 한 번만 쏘게 만들기 위함
		if (m_bAnimState == false) 
			m_bShotOnce = false;
	}
	
	m_bAnimState = m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed, false);
	__super::Update(fTimeDelta);
}

void CTank::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	if (m_bOverlab_SameLayer == true || m_bOverlab_DifferentLayer == true)
	{
		m_vecPosition += m_vecDirection * fTimeDelta * 0.5f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);
	}
}

HRESULT CTank::Render()
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

HRESULT CTank::Add_Components()
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
	/* For.Com_Collider_Sphere*/
	CBounding_Sphere::BOUND_SPHERE_DESC			SphereDesc{};
	SphereDesc.fRadius = 1.4f;
	SphereDesc.vCenter = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
		return E_FAIL;

	// For.Com_Navigation
	CNavigation::NAVIGATION_DESC		Desc{};
	Desc.iCurrentCellIndex = m_iCell_Idx;
	switch (m_eLevel)
	{
	case Client::LEVEL_GAMEPLAY:
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
			return E_FAIL;
		break;
	}
	case Client::LEVEL_YARD:
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Yard"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
			return E_FAIL;
		break;
	}

	default:
		break;
	}


	return S_OK;
}

HRESULT CTank::Bind_ShaderResources()
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

CTank* CTank::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTank* pInstance = new CTank(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTank");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTank::Clone(void* pArg)
{
	CTank* pInstance = new CTank(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CTank");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTank::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
}
