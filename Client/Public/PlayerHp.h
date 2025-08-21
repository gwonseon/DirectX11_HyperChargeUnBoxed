#pragma once
#include "Client_Defines.h"
#include "UIObject.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPlayerHp final : public CUIObject
{
public:
	typedef struct: public CUIObject::UIOBJECT_DESC
	{
		_uint	iIndex{};
		_float* fPlayerHP		= {nullptr};
	}PLAYEYER_HP_UI_DESC;

private:
	CPlayerHp(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CPlayerHp(const CPlayerHp& Prototype);
	virtual ~CPlayerHp() = default;

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

private:
	HRESULT Add_Components(_int iNum);
	HRESULT Bind_ShaderResources();

private:
	// 플레이어 HP
	float*						m_fPlayerHp;
private:
	CShader* m_pShaderCom = {nullptr};
	CTexture* m_pTextureCom = {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = {nullptr};
public:
	static CPlayerHp* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END