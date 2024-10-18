#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	typedef struct : public BOUND_DESC
	{
		_float		fRadius;
	}BOUND_SPHERE_DESC;
private:
	CBounding_Sphere();
	virtual ~CBounding_Sphere() = default;

public:
	const BoundingSphere* Get_Desc() const {
		return m_pBoundDesc;
	}

public:
	virtual HRESULT Initialize(const BOUND_DESC* pBoundDesc) override;
	virtual void Update(_fmatrix WorldMatrix) override;
	virtual _bool Intersect(CCollider::TYPE eType, CBounding* pTargetBounding) override;
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif

private:
	/* 충돌체를 위한 데이터. */
	/* 충돌응ㄹ 수행하려면 이 데이터들이 최소 월드 스페이스 가지는변환이 필요하다. \*/
	BoundingSphere* m_pBoundDesc_Original = { nullptr };
	BoundingSphere* m_pBoundDesc = { nullptr };

public:
	static CBounding_Sphere* Create(const BOUND_DESC* pBoundDesc);
	virtual void Free() override;
};

END