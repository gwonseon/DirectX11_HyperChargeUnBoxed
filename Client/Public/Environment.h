#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

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
	}ENVIRONMENT_DESC;

private:
	CEnvironment(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnvironment(const CEnvironment& Prototype);
	virtual ~CEnvironment() = default;


public:
	/* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
	virtual HRESULT Initialize_Prototype() override;

	/* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
	/* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한ㄴ다. */
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

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
	_int		Get_ModelIndex()	{		return m_iModelIndex;	}
	LEVELID		Get_Level()			{		return m_eLevel;		}
	_float3		Get_Scale()			{		return m_fScale;		}
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
	void		Picking();
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	LEVELID	m_eLevel = {};
	_int	m_iModelIndex = {};
	_float3	m_fScale = {};
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CEnvironment* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};


END