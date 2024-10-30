#pragma once
#include "Client_Defines.h"
#include "Player_Build.h"


BEGIN(Engine)
class CShader;
class CModel;
END

class CBroken_Bricks final : public CPlayer_Build
{
private:
	CBroken_Bricks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBroken_Bricks(const CBroken_Bricks& Prototype);
	virtual ~CBroken_Bricks() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	_float3	m_fDestroy_Pos{};
	_bool  m_bDestroy_Pos_Mgr = false;

	_float	m_fDelete_Time{};
public:
	static CBroken_Bricks* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

