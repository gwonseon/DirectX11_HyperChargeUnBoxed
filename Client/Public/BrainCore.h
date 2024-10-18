#pragma once
#include "Client_Defines.h"
#include "Player_Build.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CBrainCore final :  public CPlayer_Build
{
public:
	typedef struct : public CPlayer_Build::PLAYER_BUILD_DESC
	{

		_int	iModelComponentIndex{};
	}BRAIN_CORE_DESC;


private:
	CBrainCore(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBrainCore(const CBrainCore& Prototype);
	virtual ~CBrainCore() = default;

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
	_vector* Get_BrainPos() { return &m_vecPos; }
	_float* Get_BrainHp() { return &m_fHp; }
	_float* Get_BrainEnergy() { return &m_fEnergy; }


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	_vector m_vecPos{};



public:
	static CBrainCore* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END

