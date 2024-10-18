#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon_Katana final : public CPartObject
{
public:
	typedef struct : CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };
		const _float4x4* pSocketMatrix = { nullptr };
	}KATANA_DESC;

private:
	CWeapon_Katana(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Katana(const CWeapon_Katana& Prototype);
	virtual ~CWeapon_Katana() = default;

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
	void Set_KatanaState(_bool bState) { m_bKatanaState = bState; }

private:
	CCollider* m_pColliderCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };


	const _float4x4* m_pSocketMatrix = { nullptr };
	const _uint* m_pParentState = { nullptr };



private:
	_float Scale{};
	_float3 Position{};
	_float3	Rotation{};
	_bool m_bKatanaState{};

	_uint* m_iViewState{};
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CWeapon_Katana* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END