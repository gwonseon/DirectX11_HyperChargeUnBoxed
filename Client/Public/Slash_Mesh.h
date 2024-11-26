#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CSlash_Mesh final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		LEVELID eID = {};
		_vector* vecPlayerPos = { nullptr };
		const _float4x4* pSocketMatrix = { nullptr };
		const _float4x4* pParentMatrix = { nullptr };
	}SLASH_DESC;

private:
	CSlash_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSlash_Mesh(const CSlash_Mesh& Prototype);
	virtual ~CSlash_Mesh() = default;


public:
	virtual HRESULT Initialize_Prototype() override;

	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:

	 const _float4x4*	m_pSocketMatrix = { nullptr };
	 const _float4x4*	m_pParentMatrix = { nullptr };
	_float4x4			m_WorldMatrix{};


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	_vector* m_vecPlayerPos = { nullptr };
	_vector m_vecPos{};

	_float m_fLifeTime{};
	_float m_fUValue = -1.f;
private:
	LEVELID	m_eLevel = {};
	_uint	m_iModelIndex = 0;

public:
	static CSlash_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END