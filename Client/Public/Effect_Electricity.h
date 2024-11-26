#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)


class CEffect_Electricity : public CEffect
{
public:
	typedef struct : public CEffect::EFFECT_DESC
	{
		_uint	iTexNum{};
		_vector* vecPos = { nullptr };
	}EFFECT_ELECTRICITY_DESC;

private:
	CEffect_Electricity(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Electricity(const CEffect_Electricity& Prototype);
	virtual ~CEffect_Electricity() = default;


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
	_float4 CamPos{};
	_float						m_fDelay{};
	_vector* m_vecPos = { nullptr };

	_float						m_fMoveUV{};
	_uint						m_iTextureNum{};
	_float2						m_fMaxFrame{};

	_vector						m_vecFinalPos{};
public:
	static CEffect_Electricity* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END