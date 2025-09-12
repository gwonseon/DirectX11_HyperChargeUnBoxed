#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
END

BEGIN(Client)

class CPlayer2_Parts: public CPartObject
{
public:
	struct PLAYER2_PARTS_DESC :public CPartObject::PARTOBJECT_DESC
	{
		LEVELID eLevelID{};

	};

protected:
	CPlayer2_Parts(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CPlayer2_Parts(const CPlayer2_Parts& Prototype);
	virtual ~CPlayer2_Parts() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	static CPlayer2_Parts* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END