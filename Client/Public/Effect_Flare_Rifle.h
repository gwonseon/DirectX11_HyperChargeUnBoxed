#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CEffect_Flare_Rifle  : public CEffect
{ 
public:
	enum FLARE_TYPE { FLARE_PLAYER, FLARE_RIFLEMAN, FLARE_HELICOPTER,FLARE_TANK, FLARE_END};
	typedef struct : public CEffect::EFFECT_DESC
	{
		FLARE_TYPE	eType{};
		_vector* vecCamPos = { nullptr };
		_vector* vecWeaponPos = { nullptr };
		_vector* vecTargetPos = { nullptr };
	}EFFECT_RIFLE_FLARE_DESC;
private:
	CEffect_Flare_Rifle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Flare_Rifle(const CEffect_Flare_Rifle& Prototype);
	virtual ~CEffect_Flare_Rifle() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	FLARE_TYPE					m_eType{};
	_uint						m_iTexNum{};

	_float2						m_fMaxFrame{};
	_float2						m_fFrame{};
	_float3						m_fScale{};
	_vector*					m_vecCamPos		= { nullptr };
	_vector*					m_vecWeaponPos	= {nullptr};
	_vector*					m_vecTargetPos	= { nullptr };
public:
	static CEffect_Flare_Rifle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END