#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)


class CCoin final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		LEVELID eID = {};
	}COIN_DESC;

private:
	CCoin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCoin(const CCoin& Prototype);
	virtual ~CCoin() = default;


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

public:
	_vector		Get_Pos()
	{
		if (m_pTransformCom)
			return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		return { 0.f,0.f, 0.f, 0.f };
	}
	_float3		Get_Scale() { return m_fScale; }
	LEVELID		Get_Level() { return m_eLevel; }

public:
	void	Set_Scale(_float fTimeDelta, _float PosX, _float PosY, _float PosZ)
	{
		m_pTransformCom->Set_Scaling(PosX, PosY, PosZ);
		m_fScale.x = PosX;
		m_fScale.y = PosY;
		m_fScale.z = PosZ;
	}
	void	MovePos( _float PosX, _float PosY, _float PosZ)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { PosX,PosY,PosZ,1 });
	}

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	LEVELID	m_eLevel = {};
	_uint	m_iModelIndex = 0;

	_float3	m_fScale = {};

public:
	static CCoin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};


END

