#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };

protected:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;



public:
	virtual HRESULT Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg) override;

public:
	_bool Intersect(CCollider* pTargetCollider);
	_float3 Get_Center();
	_float3 Get_Extents();
	float Get_Radius();
	_bool Intersect_Mouse(_vector rayOrigin, _vector rayDirection, float& fDistance);
#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };

#endif

public:
	void Update(_fmatrix WorldMatrix);

private:
	TYPE				m_eColliderType = { TYPE_END };

	class CBounding* m_pBounding = { nullptr };
	_bool				m_isColl = { false };



public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END