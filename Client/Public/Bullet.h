#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CBullet final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		LEVELID eID = {};
		_vector m_vecWeaponPos{};
		_vector m_vecWeaponDir{};
		_vector m_vecWeaponRight{};
		_vector m_vecCameraAt{};
		_vector m_vecCameraPos{};
		
	}BULLET_DESC;
private:
	CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBullet(const CBullet& Prototype);
	virtual ~CBullet() = default;

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
	CModel* m_pModelCom = { nullptr };

	_vector m_vecWeaponPos{};
	_vector m_vecWeaponDir{};
	_vector m_vecWeaponRight{};
	_vector m_vecCameraAt{};
	_vector m_vecCameraPos{};
	_vector vTargetPos{};

	_uint iRand{};
	_bool m_bChange_Root = false;
	_float  m_fBullet_Move{};
	float distance{};
private:
	LEVELID m_eLevel{};

public:
	static CBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END