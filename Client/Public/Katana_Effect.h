#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CKatana_Effect final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		const _uint* pParentState = { nullptr };

		LEVELID eLevel{};
		_uint	iEffectNumber{};
		_float4x4 pMatrix{};
		_bool* bKatanaState = {nullptr};
	}EFFECT_KATANA_DESC;


private:
	CKatana_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKatana_Effect(const CKatana_Effect& Prototype);
	virtual ~CKatana_Effect() = default;


public:
	virtual HRESULT Initialize_Prototype() override;

	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_WorldMatrix(_float4x4 matWorld) { m_WorldMatrix = matWorld; }
	void Set_BlendValue(_float fValue) { m_fBlend_Value = fValue; }


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	const _uint* m_pParentState = { nullptr };

private:
	LEVELID m_eLevel{};
	_uint	m_iEffectNumber{};

	
	_float4x4 m_WorldMatrix{};
	_bool* m_bKatanaState = { nullptr };
	_float m_fBlend_Value{};
public:
	static CKatana_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END