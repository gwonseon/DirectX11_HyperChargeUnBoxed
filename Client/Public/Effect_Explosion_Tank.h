#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Explosion_Tank : public CEffect
{
public:
	enum EXPLOSION_TYPE{EXPLOSION_TANK, EXPLOSION_MISSILE, EXPLOSION_END};
	typedef struct : public CEffect::EFFECT_DESC
	{
		EXPLOSION_TYPE eType{};
	}EFFECT_Tank_Explosion_DESC;
private:
	CEffect_Explosion_Tank(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Explosion_Tank(const CEffect_Explosion_Tank& Prototype);
	virtual ~CEffect_Explosion_Tank() = default;


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
	_float2						m_fFrame{};
	_float3						m_fScale{};

	_float						m_fDelay{};
	EXPLOSION_TYPE m_eType{};

	_uint						m_iTextureNum{};
	_float						m_fMaxFrame{};
public:
	static CEffect_Explosion_Tank* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END