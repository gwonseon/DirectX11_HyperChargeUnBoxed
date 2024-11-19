#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CEffect : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		enum LEVELID eLevel {};
	}EFFECT_DESC;

private:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& Prototype);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float3		m_fPosition{};
	_float3		m_fScale{};
	LEVELID		m_eLevel{};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END