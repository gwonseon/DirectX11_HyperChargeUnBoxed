#include "..\Public\CollisionMgr.h"
#include <PartObject.h>


CCollisionMgr::CCollisionMgr()
{
}

HRESULT CCollisionMgr::Initialize()
{
	return S_OK;
}


_bool CCollisionMgr::Collision_Bullet(CLayer* Target, const _wstring& strTargetComponentTag, _vector vRayDir, _vector vRayPos,  _bool* bShot, _float fDamage, _uint iTargetPartObjID)
{
	_bool Collision{};
	if(Target != nullptr)
	{
		for (auto& pTarget : Target->Get_GameObject_List())
		{
			CCollider* pTargetCollider = static_cast<CCollider*>(pTarget->Find_Component(strTargetComponentTag, iTargetPartObjID));
			_float3 fCenter = pTargetCollider->Get_Center();
			float fRadius = pTargetCollider->Get_Radius();
			_float fDistance{};
			Collision = pTargetCollider->Intersect_Mouse(vRayPos, vRayDir, fDistance);
			if (Collision == true)
			{
				pTarget->Set_CollisionChecking(true);
				if (*bShot == true)
				{
					pTarget->Set_Damaged(fDamage);
				}
				break;
			}
			else
			{
				pTarget->Set_CollisionChecking(false);
			}
		}
	}
	return Collision;
}

void CCollisionMgr::Collision_Layer(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID, _uint iDstPartObjID)
{

	if(pSrcLayer != nullptr && pDstLayer != nullptr)
	{
		for (auto& pSrc : pSrcLayer->Get_GameObject_List())
		{
			// 당하는 오브젝트의 Collider 컴포넌트 가져오기
			CCollider* pSrcCol = static_cast<CCollider*>(pSrc->Find_Component(strSrcComponentTag, iSrcPartObjID));

			// 당하는 오브젝트 위치 가져오기
			CTransform* m_pTrans = pSrc->Get_Transform();
			_vector vPos = m_pTrans->Get_State(CTransform::STATE_POSITION);
			_float3 fPos{};
			XMStoreFloat3(&fPos, vPos);

			for (auto& pDst : pDstLayer->Get_GameObject_List())
			{
				// 가하는 오브젝트의 위치 가져오기
				_vector vTargetPos = pDst->Get_Transform()->Get_State(CTransform::STATE_POSITION);
				_float3 fTargetPos{};
				XMStoreFloat3(&fTargetPos, vTargetPos);

				// 위치 비교해서 안에 들어온 애들만 검사학기 && 공격 상태일 때만 확인하기 
				if (m_pTrans->Cal_Distance(fPos, fTargetPos) < 500.f && pDst->Get_AttackState() == true && pSrc->Get_CanAttacked() == true)
				{
					// 가하는 오브젝트 Collider 컴포넌트 가져오기
					CCollider* pTarget = static_cast<CCollider*>(pDst->Find_Component(strDstComponentTag, iDstPartObjID));
					// 충돌 비교
					if (pSrcCol->Intersect(pTarget))
					{
						pSrc->Set_Attacked(true); // 공격 당했음을 알림
						pSrc->Set_Damaged(pDst->Get_Attack()); // 가해자 공격력만큼 피 깎음(Set_Damage 내부에서 에너지량에 따라 데미지 입힘)
						pSrc->Set_CanAttacked(false);
						// HP가 0일 때
						if (pSrc->Get_Hp() <= 0.f)
						{
							// 삭제하면 안되는 객체는 넉다운으로 따로 처리
							if (pSrc->Get_DontDestroyAble() == true)
							{
								pSrc->Set_knockdown(true);
							}
							else // 삭제하는 애들은 데드시킴
								pSrc->Set_Dead();
						}
						break;
					}
				}
			}
		}
	}
}

// 앞이 플레이어 뒤가 코인
void CCollisionMgr::Collision_Layer_Coin(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID, _uint iDstPartObjID)
{
	for (auto& pSrc : pSrcLayer->Get_GameObject_List())
	{
		// 당하는 오브젝트의 Collider 컴포넌트 가져오기
		CCollider* pSrcCol = static_cast<CCollider*>(pSrc->Find_Component(strSrcComponentTag, iSrcPartObjID));

		// 당하는 오브젝트 위치 가져오기
		CTransform* m_pTrans = pSrc->Get_Transform();
		_vector vPos = m_pTrans->Get_State(CTransform::STATE_POSITION);
		_float3 fPos{};
		XMStoreFloat3(&fPos, vPos);

		for (auto& pDst : pDstLayer->Get_GameObject_List())
		{
			// 가하는 오브젝트의 위치 가져오기
			_vector vTargetPos = pDst->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			_float3 fTargetPos{};
			XMStoreFloat3(&fTargetPos, vTargetPos);

			// 위치 비교해서 안에 들어온 애들만 검사학기 && 공격 상태일 때만 확인하기 
			if (m_pTrans->Cal_Distance(fPos, fTargetPos) < 500.f)
			{
				// 가하는 오브젝트 Collider 컴포넌트 가져오기
				CCollider* pTarget = static_cast<CCollider*>(pDst->Find_Component(strDstComponentTag, iDstPartObjID));
				// 충돌 비교
				if (pSrcCol->Intersect(pTarget))
				{
					pDst->Set_PickUp_Coin(pSrc->Get_Coin()); // 코인 얻음
					pSrc->Set_Dead();// 동전 삭제 

				}
			}
		}
	}
}

void CCollisionMgr::Collision_Trap(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID, _uint iDstPartObjID)
{
	if (pSrcLayer != nullptr && pDstLayer != nullptr)
	{
		for (auto& pSrc : pSrcLayer->Get_GameObject_List())
		{
			// 당하는 오브젝트의 Collider 컴포넌트 가져오기
			CCollider* pSrcCol = static_cast<CCollider*>(pSrc->Find_Component(strSrcComponentTag, iSrcPartObjID));

			// 당하는 오브젝트 위치 가져오기
			CTransform* m_pTrans = pSrc->Get_Transform();
			_vector vPos = m_pTrans->Get_State(CTransform::STATE_POSITION);
			_float3 fPos{};
			XMStoreFloat3(&fPos, vPos);

			for (auto& pDst : pDstLayer->Get_GameObject_List())
			{
				// 가하는 오브젝트의 위치 가져오기
				_vector vTargetPos = pDst->Get_Transform()->Get_State(CTransform::STATE_POSITION);
				_float3 fTargetPos{};
				XMStoreFloat3(&fTargetPos, vTargetPos);
				// 위치 비교해서 안에 들어온 애들만 검사학기 && 공격 상태일 때만 확인하기 
				if (m_pTrans->Cal_Distance(fPos, fTargetPos) < 500.f && pSrc->Get_knockdown()==false )
				{
					// 가하는 오브젝트 Collider 컴포넌트 가져오기
					CCollider* pTarget = static_cast<CCollider*>(pDst->Find_Component(strDstComponentTag, iDstPartObjID));
					// 충돌 비교
					if (pSrcCol->Intersect(pTarget))
					{
						if(pDst->Get_CanAttacked() == true)
						{
							pSrc->Set_Damaged(pDst->Get_Attack()); // 가해자 공격력만큼 피 깎음(Set_Damage 내부에서 에너지량에 따라 데미지 입힘)
							pDst->Set_CanAttacked(false);
						}
						pSrc->Set_Attacked(true); // 공격 당했음을 알림
							
						// 총알 일 때
						if(pDst->Get_IsBullet() == true)
						{
							pDst->Set_Dead(); // 총알 없앰
						}
						
						// HP가 0일 때
						if (pSrc->Get_Hp() <= 0.f)
						{
							// 삭제하면 안되는 객체는 넉다운으로 따로 처리
							if (pSrc->Get_DontDestroyAble() == true)
							{
								pSrc->Set_knockdown(true);
							}
						}
						break;
					}
				}
			}
		}
	}
}

void CCollisionMgr::Collision_Explosion(CLayer* pExplosionLayer, CLayer* pAttackedLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iCount, _uint iSrcPartObjID, _uint iDstPartObjID)
{
	if (pExplosionLayer != nullptr && pAttackedLayer != nullptr)
	{
		for (auto& pExplosion : pExplosionLayer->Get_GameObject_List())
		{
			CCollider* pExplosionCol = static_cast<CCollider*>(pExplosion->Find_Component(strSrcComponentTag, iSrcPartObjID));
			// 당하는 오브젝트 위치 가져오기
			CTransform* m_pExplosionTrans = pExplosion->Get_Transform();
			_vector vExplosionPos = m_pExplosionTrans->Get_State(CTransform::STATE_POSITION);
			_float3 fExplosionPos{};
			XMStoreFloat3(&fExplosionPos, vExplosionPos);
			if(pExplosion->Get_AttackState() == true)
			{
				for (auto& pAttacked : pAttackedLayer->Get_GameObject_List())
				{
					_vector vAttackedTargetPos = pAttacked->Get_Transform()->Get_State(CTransform::STATE_POSITION);
					_float3 fAttackedTargetPos{};
					XMStoreFloat3(&fAttackedTargetPos, vAttackedTargetPos);
					// 위치 비교해서 안에 들어온 애들만 검사
					if (!(m_pExplosionTrans->Cal_Distance(fExplosionPos, fAttackedTargetPos) < 2000.f))
						continue;
					 // 트랩 오브젝트 Collider 컴포넌트 가져오기
					CCollider* pAttackedCol = static_cast<CCollider*>(pAttacked->Find_Component(strDstComponentTag, iDstPartObjID));
					if (pAttackedCol == nullptr)
						continue;
					// 충돌 비교
					if (pAttacked->Get_CanAttacked() == false)
						continue;
					if (pExplosionCol->Intersect(pAttackedCol))
					{
						pAttacked->Set_Damaged(pExplosion->Get_Attack()); // 가해자 공격력만큼 피 깎음(Set_Damage 내부에서 에너지량에 따라 데미지 입힘
						// HP가 0일 때
						if (pAttacked->Get_Hp() <= 0.f)
						{
							// 삭제하면 안되는 객체는 넉다운으로 따로 처리
							if (pAttacked->Get_DontDestroyAble() == true)
								pAttacked->Set_knockdown(true);
							else // 삭제하는 애들은 데드시킴
								pAttacked->Set_Dead();
						}
					}
				}
				
			}
			if(iCount == 1)
			{
				pExplosion->Set_AttackState(false);
				pExplosion->Set_Dead();
			}
		}
	}
}

void CCollisionMgr::Anti_OverLapping(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID, _uint iDstPartObjID)
{
	if (pSrcLayer != nullptr && pDstLayer != nullptr)
	{
		for (auto& pSrc : pSrcLayer->Get_GameObject_List())
		{
			// 당하는 오브젝트의 Collider 컴포넌트 가져오기
			CCollider* pSrcCol = static_cast<CCollider*>(pSrc->Find_Component(strSrcComponentTag, iSrcPartObjID));

			// 당하는 오브젝트 위치 가져오기
			CTransform* m_pTrans = pSrc->Get_Transform();
			_vector vPos = m_pTrans->Get_State(CTransform::STATE_POSITION);
			_float3 fPos{};
			XMStoreFloat3(&fPos, vPos);
			for (auto& pDst : pDstLayer->Get_GameObject_List())
			{
				// 가하는 오브젝트의 위치 가져오기
				_vector vTargetPos = pDst->Get_Transform()->Get_State(CTransform::STATE_POSITION);
				_float3 fTargetPos{};
				XMStoreFloat3(&fTargetPos, vTargetPos);
				// 위치 비교해서 안에 들어온 애들만 검사
				if (m_pTrans->Cal_Distance(fPos, fTargetPos) < 500.f)
				{
					// 가하는 오브젝트 Collider 컴포넌트 가져오기
					CCollider* pTarget = static_cast<CCollider*>(pDst->Find_Component(strDstComponentTag, iDstPartObjID));
					// 충돌 비교
					if (pSrcCol->Intersect(pTarget))
					{
						// 둘 사이의 대각선 거리를 가져와서 반지름 길이를 더한 값 만큼의 길이가 되게 밀어주기
						// 토요일에 하자 귀찮다
					}
				}
			}
		}
	}

}



void CCollisionMgr::Free()
{
	__super::Free();


}
