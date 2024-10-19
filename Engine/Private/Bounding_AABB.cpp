#include "..\Public\Bounding_AABB.h"


#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"


CBounding_AABB::CBounding_AABB()
{
}

HRESULT CBounding_AABB::Initialize(const BOUND_DESC* pBoundDesc)
{
	const BOUND_AABB_DESC* pDesc = static_cast<const BOUND_AABB_DESC*>(pBoundDesc);

	m_pBoundDesc_Original = new BoundingBox(pDesc->vCenter, pDesc->vExtents);
	m_pBoundDesc = new BoundingBox(*m_pBoundDesc_Original);

	return S_OK;
}

void CBounding_AABB::Update(_fmatrix WorldMatrix)
{
	_matrix		TransformMatrix = WorldMatrix;

	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(TransformMatrix.r[2]);

	m_pBoundDesc_Original->Transform(*m_pBoundDesc, TransformMatrix);
	m_fCenter = m_pBoundDesc->Center;
	m_fExtents = m_pBoundDesc->Extents;

	
}

_bool CBounding_AABB::Intersect(CCollider::TYPE eType, CBounding* pTargetBounding)
{
	_bool		isColl = { false };

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		isColl = m_pBoundDesc->Intersects(*static_cast<CBounding_AABB*>(pTargetBounding)->Get_Desc());
		break;
	case CCollider::TYPE_OBB:
		isColl = m_pBoundDesc->Intersects(*static_cast<CBounding_OBB*>(pTargetBounding)->Get_Desc());
		break;
	case CCollider::TYPE_SPHERE:
		isColl = m_pBoundDesc->Intersects(*static_cast<CBounding_Sphere*>(pTargetBounding)->Get_Desc());
		break;
	}
	return isColl;
}
#ifdef _DEBUG
HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pBoundDesc, vColor);

	return S_OK;
}

#endif

CBounding_AABB* CBounding_AABB::Create(const BOUND_DESC* pBoundDesc)
{
	CBounding_AABB* pInstance = new CBounding_AABB();

	if (FAILED(pInstance->Initialize(pBoundDesc)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundDesc_Original);
	Safe_Delete(m_pBoundDesc);
}
