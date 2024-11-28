#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;

END

BEGIN(Client)

class CDead_Model final : public CGameObject
{
public:
	enum DEAD_MODEL_TYPE { DEAD_TANK_BODY = 7, DEAD_TANK_TURRET, DEAD_MODEL_END };
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		LEVELID eLevel{};
		 _float4x4 matWorld{};
		_vector vecPos{};
		DEAD_MODEL_TYPE eModelType{};
	}DEADMODEL_DESC;


private:
	CDead_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDead_Model(const CDead_Model& Prototype);
	virtual ~CDead_Model() = default;


public:
	virtual HRESULT Initialize_Prototype() override;

	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };


private:
	LEVELID	m_eLevel = {};
	DEAD_MODEL_TYPE m_eModelType{};


	_float	m_fPower;
	_float  m_fLifeTime{};
	_float  m_fDissolve{};
	_bool   m_bDeadState = false;

	_float4x4 m_matWorld{};

public:
	static CDead_Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END