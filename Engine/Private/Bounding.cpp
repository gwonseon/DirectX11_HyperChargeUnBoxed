#include "..\Public\Bounding.h"


CBounding::CBounding()
{
}

HRESULT CBounding::Initialize(const BOUND_DESC* pBoundDesc)
{
	return S_OK;
}

_bool CBounding::Intersect(CCollider::TYPE eType, CBounding* pTargetBounding)
{
	return _bool();
}


void CBounding::Free()
{
	__super::Free();
}


