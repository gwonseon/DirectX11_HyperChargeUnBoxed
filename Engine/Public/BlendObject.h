#pragma once
#include "GameObject.h"

/* 프로토타입을 통해 객체를 생성한다. */

BEGIN(Engine)

class ENGINE_DLL CBlendObject abstract : public CGameObject
{
public:
	typedef struct: public CGameObject::GAMEOBJ_DESC
	{}BLEND_DESC;
protected:
	CBlendObject(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CBlendObject(const CBlendObject& Prototype);
	virtual ~CBlendObject() = default;

public:
	_float Get_Depth() const {
		return m_fDepth;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float					m_fDepth = {};

protected:
	void Compute_Depth();



public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END