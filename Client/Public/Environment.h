#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "VIBuffer_Box.h"
#include "CollisionBox.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CEnvironment final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		LEVELID eID = {};
		_int	iModelComponentIndex{};
		_uint	iImGuiMode{};
		_float3 CollisionBoxScale{}, CollisionBoxPos{};
		CPlayer* pPlayer = { nullptr };
	}ENVIRONMENT_DESC;

private:
	CEnvironment(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnvironment(const CEnvironment& Prototype);
	virtual ~CEnvironment() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Height() override;
	virtual HRESULT Render_Shadow() override;
public:
	void		Picking();

public:
	void	MovePos(_float fTimeDelta, _float PosX, _float PosY, _float PosZ)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { PosX,PosY,PosZ,1 });
	}
	void	Set_Scale(_float fTimeDelta, _float PosX, _float PosY, _float PosZ)
	{
		m_pTransformCom->Set_Scaling(PosX, PosY, PosZ);
		m_fScale.x = PosX;
		m_fScale.y = PosY;
		m_fScale.z = PosZ;
	}
	void Set_DeadEnviron() {
		if (m_eLevel == LEVEL_IMGUI)
			static_cast<CCollisionBox*>(m_pCollisionBox)->Set_Dead();
		m_bChecking = false;
		m_bDead = true;
	}
	void	Set_Turn(_float fTimeDelta, _fvector vAxis)
	{
		m_pTransformCom->Turn(vAxis, fTimeDelta);
	}
	_vector		Get_Pos()
	{
		if (m_pTransformCom)
			return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		return { 0.f,0.f, 0.f, 0.f };
	}

	void		Get_Rotation(_vector&	vRight, _vector&	vUp, _vector&	vLook) {
		vRight = m_pTransformCom-> Get_State(CTransform::STATE_RIGHT);
		vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
		vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	}
	void		Set_Rotaion(_vector	vRight, _vector	vUp, _vector	vLook) {
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
		m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	}


	_int		Get_ModelIndex() { return m_iModelIndex; }
	LEVELID		Get_Level() { return m_eLevel; }
	_float3		Get_Scale() { return m_fScale; }


private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CPlayer* m_pPlayer = { nullptr };

private:
	LEVELID	m_eLevel = {};
	_int	m_iModelIndex = {};
	_float3	m_fScale = {};

	

public:	// CollisionBox 
	void	Set_PickingCheck(_bool bCheck) { m_bChecking = bCheck; }
	_bool	Get_PickingCheck() { return m_bChecking; }
	void	Set_CollisionBox(_float fSizeX, _float fSizeY, _float fSizeZ, _float fPosX, _float fPosY, _float fPosZ) {
		m_fCollisionBoxScale.x = fSizeX; 		m_fCollisionBoxScale.y = fSizeY; 		m_fCollisionBoxScale.z = fSizeZ;
		_float3 fCollisionPos{}; _vector vecCollisionPos{};
		fCollisionPos.x = fPosX; fCollisionPos.y = fPosY; fCollisionPos.z = fPosZ;
		vecCollisionPos = XMLoadFloat3(&fCollisionPos);
		m_vecCollisionBoxPos = vecCollisionPos;
	}
	_float3 Get_CollisionBoxScale() { return m_fCollisionBoxScale; }
	_vector Get_CollisionBoxPos() { return m_vecCollisionBoxPos; }
	_uint Get_ImGuiMode() { return m_iImGuiMode; }
	void	Set_ImGuiMode(_uint iMode) { m_iCurrentImGuiMode = iMode; }
private:	// CollisionBox 
	CGameObject* m_pCollisionBox = nullptr;
	_bool m_bChecking = false;
	_float3 m_fCollisionBoxScale{};
	_vector m_vecCollisionBoxPos{};
	_uint	m_iImGuiMode = 0;
	_uint m_iCurrentImGuiMode = 0;


private:
	DirectX::BoundingBox BoundingBox;

public:
	void Set_BoundingBos(DirectX::BoundingBox Box) { BoundingBox = Box; }
	DirectX::BoundingBox Get_BoundingBox() { return BoundingBox;	}

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();





public:
	static CEnvironment* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};


END