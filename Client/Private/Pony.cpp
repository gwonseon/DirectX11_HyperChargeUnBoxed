#include "stdafx.h"
#include "..\Public\Pony.h"

#include "GameInstance.h"
#include "Pony_Defines.h"
#include <Effect_Electricity.h>


CPony::CPony(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
	: CMonster{pDevice,pContext}
{}

CPony::CPony(const CPony& Prototype)
	: CMonster{Prototype}
	,m_pCurrentState(new CTrotState_Pony())
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
	m_vecStoreTargetPos = *m_vecTargetPos;
	m_pTrapLayer = pDesc->pTrapLayer;
	m_iCell_Idx = pDesc->iCell_Idx;
	m_pPlayer = pDesc->pPlayer;
	m_eLevel = pDesc->eID;
	m_matPlayerWorld = pDesc->matPlayerWorld;
	m_matBrainCoreWorld = pDesc->matBrainCoreWorld;
	m_pBuild = pDesc->m_pBuild;
	m_iBraincore_CellNumber = pDesc->iBraincore_CellNumber;
	m_pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(m_eLevel,TEXT("Layer_PlayerBuild"),TEXT("Com_Collider_AABB")));

	pDesc->fScale = _float3(2.5f,2.5f,2.5f);
	pDesc->fSpeedPerSec = 14.f;

	if(FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if(FAILED(Add_Components()))
		return E_FAIL;


	m_fPrevHp = m_fHp = 50.f;
	m_fEnergy = 0.f;
	m_fAttack = 10.f;
	m_bDontDestroy = true;

	m_pModelCom->Set_Animation(0,true);
	m_bCanAttacked = true;
	m_bIsBullet = false;
	return S_OK;
}

void CPony::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if(m_bKnockdown == true)
		return;
	if(m_fHp <= 0.f)
		m_bKnockdown = true;
	m_vecPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat3(&m_fPos,m_vecPosition);
	vPlayerPos = XMVectorSet(m_matPlayerWorld->_41,m_matPlayerWorld->_42,m_matPlayerWorld->_43,1.0f);

	// 트랩 데미지 2초에 한 번씩만 줄 수 있게
	if(m_bCanAttacked == false)
	{
		if(m_fAttackTime >= 2.f)
		{
			m_bCanAttacked = true;
			m_fAttackTime = 0.f;
		}
		m_fAttackTime += fTimeDelta;
	}



}

void CPony::Update(_float fTimeDelta)
{
	if(m_bDead == true)
		return;
	if(m_bKnockdown == true)
	{
		if(m_bOnce == false)
		{
			m_pGameInstance->StopSound(SOUND_PONY_BITE);
			m_pGameInstance->StopSound(SOUND_PONY_DEAD);
			m_pGameInstance->StopSound(SOUND_PONY_WALK);

			m_pGameInstance->PlaySoundW(L"RunAway.wav",Engine::CHANNELID::SOUND_PONY_DEAD,m_fSound * 0.6f);

			m_bOnce = true;
		}
		if(m_fDissolve >= 1.f)
			m_bDead = true;
		m_fDissolve += fTimeDelta ;

		return;
	}
	__super::Update(fTimeDelta);

	// 콜라이더 업데이트
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	// 상태 패턴 업데이트
	m_pCurrentState->Update(this,fTimeDelta);
	// 타겟 찾는 시간 딜레이
	m_fTime_For_Target += fTimeDelta;
	// 넉백이 True일 때 넉백 모션하게 하기, 카타나만
	if(m_bAttacked == true)
	{
		m_fKnockBack_Height = XMVectorGetY(m_vecPosition);
		m_fKnockBack_Power = 8.f;
		m_bKnockBacking = true;
		m_bAttacked = false;
	}

	_float fDistance = m_pTransformCom->Cal_Distance_vec(vPlayerPos,m_vecPosition);
	if(m_pGameInstance->Get_DIKeyState_Down(DIK_O))
	{
		if(m_fDistnace < 400.f)
			m_fDistnace = 3000.f;
		else
			m_fDistnace = 0.5f;
	}
	// 사정거리 안에 플레이어가 없으면 
	if(fDistance > 	 m_fDistnace)
	{
		if(m_bFind_Path == false && m_bRequest_Path == false)
		{
			m_bRequest_Path = true;
			m_vecFindingPath = m_pGameInstance->Get_ptrThreadpool()->enqueue([=]() {
				cout << "길 찾기 시작함" <<  endl;
				return m_pTransformCom->PathFind(0.f,m_pNavigationCom,m_pNavigationCom->Get_CurrentCell_Index(),m_iBraincore_CellNumber);
			});
		}

		if(m_bRequest_Path == true && m_vecFindingPath.valid() 
			&& m_vecFindingPath.wait_for(chrono::seconds(0)) == future_status::ready)
		{
			// 길 찾았음
			Path = m_vecFindingPath.get();         
			m_bFind_Path = true;                   
			m_bRequest_Path = false;               
		}

		//if(m_bFind_Path == false)
		//{
		//	cout << "길찾기" << endl;
		//	Path = m_pTransformCom->PathFind(0.f,m_pNavigationCom,m_pNavigationCom->Get_CurrentCell_Index(),m_iBraincore_CellNumber);
		//	m_bFind_Path = true;
		//}



		if(m_fTime_For_Target >= 3.f) // 항상 검사하기엔 검사량이 많아서 검사 빈도수를 줄여줌
		{
			m_fTime_For_Target = 0.f;
			_int iCheck_Count = 0;
			// 트랩마다 위치 검사해서 가까이에 있으면 트랩을 향해 공격 진행
			for(auto pTrap : m_pTrapLayer->Get_GameObject_List())
			{
				if(static_cast<CTrap_Marks*>(pTrap)->Get_Build_Done() == true)
				{
					m_vecNewTargetPos = static_cast<CTrap_Marks*>(pTrap)->Get_TrapPos();
					// 근접 공격이기 때문에 먼거리에서 트랩을 찾을 필요는 없음
					if(m_pTransformCom->Cal_Distance_vec(m_vecNewTargetPos,m_vecPosition) <= 1000.f && static_cast<CTrap_Marks*>(pTrap)->Get_knockdown() == false)
					{
						// 새 타겟으로 바꿔줌
						m_vecTargetPos = &m_vecNewTargetPos;
						break;
					}
				}
				++iCheck_Count;
			}
			// 새로운 타겟이 근처에 없으면 기록해뒀던 브레인 코어 공격
			if(iCheck_Count == m_pTrapLayer->Get_GameObjectList_Size())
			{
				m_vecTargetPos = &m_vecStoreTargetPos;
			}
		}

		if(m_pTransformCom->Cal_Distance_vec(m_vecPosition,*m_vecTargetPos) <= 1000.f)
		{
			m_pTransformCom->LookAt(*m_vecTargetPos);
			if(m_pTransformCom->Cal_Distance_vec(m_vecPosition,*m_vecTargetPos) <= 30.f)
			{
				if(m_fAttackTime >= 2.f)
				{
					// 2초마다 데미지 주기( 브레인 코어에)
					_bool bCollision = m_pColliderCom->Intersect(m_pTargetCollider);
					if(bCollision == true)
					{
						m_pBuild->Set_Damaged(m_fAttack);
						m_fAttackTime = 0.f;
					}
				}

				m_pGameInstance->PlaySoundW(L"FE_Ninja_Animal_Puma.wav",Engine::CHANNELID::SOUND_PONY_BITE,m_fSound * 0.1f);

				m_fAttackTime += fTimeDelta;
				m_ePonyState = ATTACK_STATE;
				m_bAnimState = m_pModelCom->Play_Animation(fTimeDelta * 0.1f,true);
				m_bAttackState = true;
				m_bWalkState = true;
			} else
			{
				m_ePonyState = TROT_STATE;
				/*	m_pCurrentState->Trot(this);*/
				m_pTransformCom->Go_Straight_Nav(fTimeDelta * 1.5f,m_pNavigationCom);
			}
		} else
		{
			if(m_bFind_Path == true && Path.size() > 0)
			{
				// 길찾기 수행
				if(m_pTransformCom->Cal_Distance(Path.front(),m_fPos) <= 100.f)
				{
					if(Path.size() > 1)
						Path.erase(Path.begin());
				}
			}


			m_ePonyState = TROT_STATE;
			/*m_pCurrentState->Walk(this);*/
			if(m_bFind_Path == true /*&& m_bRequest_Path == false*/ && Path.size() > 0)
				m_pTransformCom->LookAt(XMVectorSet(Path.front().x,Path.front().y,Path.front().z,1.f));
			m_pTransformCom->Go_Straight_Nav(fTimeDelta * 1.5f,m_pNavigationCom);
		}

		m_pModelCom->Play_Animation(fTimeDelta,false);

		m_bAttackState = false;
	} else
	{
		m_bFind_Path = false;
		// 플레이어와의 거리가 멀어졌을 때
		if(fDistance > 10.f)
		{
			//if(m_iPrevPlayer_Cell_Idx != m_pPlayer->Get_CurrentCellIdx())
			//{
			//	// 현재 내 위치와 플레이어 위치 찾아서 길찾기 수행
			//	Path = m_pTransformCom->PathFind(0.f, m_pNavigationCom, m_pNavigationCom->Get_CurrentCell_Index(), m_pPlayer->Get_CurrentCellIdx());
			//	m_iPrevPlayer_Cell_Idx = m_pPlayer->Get_CurrentCellIdx();
			//}
			//// 길찾기 수행
			//if (m_pTransformCom->Cal_Distance(Path.front(), m_fPos) <= 600.f)
			//{
			//	if (Path.size() > 1)
			//		Path.erase(Path.begin());
			//}
			if(m_bWalkState == true)
			{
				m_ePonyState = TROT_STATE;
				/*	m_pCurrentState->Walk(this);*/
			}

			m_pTransformCom->LookAt(vPlayerPos);
			m_pModelCom->Play_Animation(fTimeDelta,false);
			m_pTransformCom->Go_Straight_Nav(fTimeDelta + m_fRunSpeed,m_pNavigationCom); // 뛸 때 m_fRunSpeed값이 바뀜
			m_bAttackState = false;
		} else
		{
			m_pGameInstance->PlaySoundW(L"FE_Ninja_Animal_Puma.wav",Engine::CHANNELID::SOUND_PONY_BITE,m_fSound * 0.1f);

			// 공격 상태
			m_ePonyState = ATTACK_STATE;
			/*m_pCurrentState->Attack(this);*/
			m_pTransformCom->LookAt(vPlayerPos);
			m_bAnimState = m_pModelCom->Play_Animation(fTimeDelta,false);
			m_bAttackState = true;
			m_bWalkState = true;
		}
	}
}

void CPony::Late_Update(_float fTimeDelta)
{
	if(m_bKnockdown == false)
	{
		__super::Late_Update(fTimeDelta);
	}


	if(m_bKnockdown == true)
	{
		if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONLIGHT,this)))
			return;
		if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM,this)))
			return;
		return;
	}
	if(m_bOverlab_SameLayer == true || m_bOverlab_DifferentLayer == true)
	{
		m_vecPosition += m_vecDirection * fTimeDelta * 0.5f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION,m_vecPosition);
	}

	// 넉백이 true일 때 넉백 모션
	if(m_bKnockBacking == true)
	{
		_vector vKnockBack_DIr = m_vecPosition - vPlayerPos; // 플레이어 방향으로부터 반대방향으로 날아가기
		vKnockBack_DIr = XMVector3Normalize(vKnockBack_DIr);
		if(m_pTransformCom->KnockBack(fTimeDelta,vKnockBack_DIr,m_fKnockBack_Power,m_fKnockBack_Height) == true)
		{
			// 모션 끝남
			m_bCanAttacked = true;
			m_bKnockBacking = false;
		}
	}
	_bool bKatanaCheck = m_pGameInstance->Get_KatanaState();
	if(m_fPrevHp != m_fHp && bKatanaCheck == true)
	{
		CEffect_Electricity::EFFECT_ELECTRICITY_DESC pElectricity{};
		pElectricity.eLevel = m_eLevel;
		pElectricity.fScale = _float3{10.f,10.f,10.f};
		pElectricity.iTexNum = 3;
		pElectricity.vecPos = &m_vecPosition;
		m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel,TEXT("Effect_Layer"),TEXT("Prototype_GameObject_Effect_Lightning"),&pElectricity);
		m_fPrevHp = m_fHp;
	} else if(bKatanaCheck == false)
	{
		m_fPrevHp = m_fHp;
	}
}

HRESULT CPony::Render()
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
		if(m_bKnockdown == true)
		{
			if(FAILED(m_pShaderCom->Begin(6)))
				return E_FAIL;
		} else
		{
			if(FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;
		}

		m_pModelCom->Render(i);
	}

	#ifdef _DEBUG
	m_pColliderCom->Render();
	#endif

	return S_OK;
}

HRESULT CPony::Render_Shadow()
{
	_float4x4			ViewMatrix,ProjMatrix;

	_float fFar = m_pGameInstance->Get_CameraFar();
	fFar = 5000.f;
	_float4 fPlayerPos = m_pGameInstance->Get_PlayerPos();
	XMStoreFloat4x4(&ViewMatrix,XMMatrixLookAtLH(XMVectorSet(364.283f - 5.f,30.f,300.f - 5.f,1.f),XMVectorSet(364.283f,0.f,300.f,1.f),XMVectorSet(0.f,1.f,0.f,0.f)));
	XMStoreFloat4x4(&ProjMatrix,XMMatrixPerspectiveFovLH(XMConvertToRadians(120.f),(_float)1280.f / 720.f,0.1f,fFar));

	if(FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix",m_pTransformCom->Get_WorldMatrixPtr())))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",&ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",&ProjMatrix)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_RawValue("g_fFar",&fFar,sizeof(float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for(size_t i = 0; i < iNumMeshes; i++)
	{
		if(FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom,i,"g_BoneMatrices")))
			return E_FAIL;

		if(FAILED(m_pShaderCom->Begin(5)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CPony::Add_Components()
{
	/* For.Com_Texture */
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_Dissolved"),
		TEXT("Com_Texture"),reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"),reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	const _wstring Model_Component = TEXT("Prototype_Component_Model_Anim");
	const _wstring Model_Component_Result = Model_Component + to_wstring(ANIM_PONY);
	if(FAILED(__super::Add_Component(m_eLevel,Model_Component_Result,
		TEXT("Com_Model"),reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_Sphere::BOUND_SPHERE_DESC			SphereDesc{};
	SphereDesc.fRadius = 1.2f;
	SphereDesc.vCenter = _float3(0.f,SphereDesc.fRadius,0.f);
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"),reinterpret_cast<CComponent**>(&m_pColliderCom),&SphereDesc)))
		return E_FAIL;

	// For.Com_Navigation
	CNavigation::NAVIGATION_DESC		Desc{};
	Desc.iCurrentCellIndex = m_iCell_Idx;
	switch(m_eLevel)
	{
	case Client::LEVEL_GAMEPLAY:
	{
		if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Navigation"),
			TEXT("Com_Navigation"),reinterpret_cast<CComponent**>(&m_pNavigationCom),&Desc)))
			return E_FAIL;
		break;
	}
	case Client::LEVEL_YARD:
	{
		if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Navigation_Yard"),
			TEXT("Com_Navigation"),reinterpret_cast<CComponent**>(&m_pNavigationCom),&Desc)))
			return E_FAIL;
		break;
	}

	default:
	break;
	}



	return S_OK;
}

HRESULT CPony::Bind_ShaderResources()
{
	if(m_bKnockdown == true)
	{
		if(FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom,"g_MaskTexture",static_cast<_uint>(0))))
			return E_FAIL;
		if(FAILED(m_pShaderCom->Bind_RawValue("g_fDissolve_Value",&m_fDissolve,sizeof(float))))
			return E_FAIL;
	}

	if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom,"g_WorldMatrix")))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fFar = m_pGameInstance->Get_CameraFar();
	if(FAILED(m_pShaderCom->Bind_RawValue("g_fFar",&fFar,sizeof(float))))
		return E_FAIL;


	return S_OK;
}

CPony* CPony::Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
{
	CPony* pInstance = new CPony(pDevice,pContext);

	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPony");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPony::Clone(void* pArg)
{
	CPony* pInstance = new CPony(*this);
	if(FAILED(pInstance->Initialize(pArg)))
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
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pTextureCom);

}