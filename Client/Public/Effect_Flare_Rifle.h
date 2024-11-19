#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"
#include <Camera_Free.h>
BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CEffect_Flare_Rifle final : public CBlendObject
{ 
public:
	typedef struct : public CBlendObject::BLEND_DESC
	{
		CCamera_Free* pCamera = { nullptr };
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
	_float						m_fFrame = { 0.f };
	_float3						m_fScale{};
	CCamera_Free* m_pCamera = { nullptr };

public:
	static CEffect_Flare_Rifle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END