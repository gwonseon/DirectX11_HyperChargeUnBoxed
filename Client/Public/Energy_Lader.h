#pragma once
#include "Client_Defines.h"
#include "Player_Build.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CEnergy_Lader final : public CPlayer_Build
{
public:
	typedef struct : public CPlayer_Build::PLAYER_BUILD_DESC
	{
	
	}ENERGYLADER_DESC;


private:
	CEnergy_Lader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnergy_Lader(const CEnergy_Lader& Prototype);
	virtual ~CEnergy_Lader() = default;

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

public:
	static CEnergy_Lader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
