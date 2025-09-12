#pragma once
#include "Client_Defines.h"
#include "Player2_Parts.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPlayer2_Head final: public CPlayer2_Parts
{
public:
	struct PLAYER2_HEAD_DESC:public CPlayer2_Parts::PLAYER2_PARTS_DESC
	{
		const _uint* pParentState = {nullptr};
		const _float4x4* pSocketMatrix = {nullptr};

	};

private:
	CPlayer2_Head(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CPlayer2_Head(const CPlayer2_Head& Prototype);
	virtual ~CPlayer2_Head() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	//virtual HRESULT Render_Shadow() override;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


private:
	CShader* m_pShaderCom = {nullptr};
	CModel* m_pModelCom = {nullptr};

	const _float4x4* m_pSocketMatrix = {nullptr};

private:
	_uint	m_iShaderPassNum = 0;
	
	LEVELID m_eLevelID{};

	_bool m_bTPSState = false;

public:
	static CPlayer2_Head* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END