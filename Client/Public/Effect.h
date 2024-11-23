#pragma once
#include "Client_Defines.h"
#include "BlendObject.h"
#include "Camera_Free.h"

BEGIN(Client)

class CEffect abstract : public CBlendObject
{
public:
	typedef struct : public CBlendObject::BLEND_DESC
	{
		class	CCamera_Free* pCamera = { nullptr };
		enum	LEVELID eLevel{};
	}EFFECT_DESC;

protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& Prototype);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Initialize_Prototype()override ;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float3		m_fPosition{};
	_float3		m_fScale{};
	_float		m_fLifeTime{};

	LEVELID					m_eLevel{};
	class CCamera_Free*		m_pCamera = { nullptr };


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END