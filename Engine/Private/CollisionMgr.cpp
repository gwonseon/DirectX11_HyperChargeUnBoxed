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
	for (auto& pTarget : Target->Get_GameObject_List())
	{
		CCollider* pTargetCollider = static_cast<CCollider*>(pTarget->Find_Component(strTargetComponentTag, iTargetPartObjID));
		_float3 fCenter =  pTargetCollider->Get_Center();
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
		}
		else
		{
			pTarget->Set_CollisionChecking(false);
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
			if (m_pTrans->Cal_Distance(fPos, fTargetPos) < 500.f && pDst->Get_AttackState() == true)
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



void CCollisionMgr::Free()
{
	__super::Free();


}
