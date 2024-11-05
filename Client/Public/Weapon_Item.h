#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon_Item final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		LEVELID eID = {};
		_int	iModelIndex{};
	}WEAPONITEM_DESC;

private:
	CWeapon_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Item(const CWeapon_Item& Prototype);
	virtual ~CWeapon_Item() = default;


public:
	virtual HRESULT Initialize_Prototype() override;

	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	_vector Get_Position() { return m_vecPos; }
	void	Set_Interation(_bool bInteraction) { m_bInteration = bInteraction; }
	void	Set_Charging(_bool bCharging) { m_bCharging = bCharging; }
	_bool	Get_Charging() { return m_bCharging; }
	void	Set_WeaponItem_Equip(_bool& bEquip, _uint& iEquipNumber) {
		bEquip = m_bEquip;
		iEquipNumber = m_iModelIndex ;
	}
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };


private:
	LEVELID	m_eLevel = {};
	_uint	m_iModelIndex = 0;
	_vector m_vecPos{};
	_bool	m_bInteration = false;
	_bool	m_bCharging = false;
	_bool	m_bEquip = false;
	_float	m_fCharging = 0.f;
	_float fRotation_value{};

	_float3 Rotation{};
	_float3 fScale{};

public:
	static CWeapon_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END