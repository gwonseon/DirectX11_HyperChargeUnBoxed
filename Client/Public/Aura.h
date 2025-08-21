#pragma once

#include "Client_Defines.h"
#include "Effect.h"


BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)
class CCoin_Item;
class CCoin;
class CAura final: public CGameObject
{
public:
	enum AURA_TYPE{
		ITEM_AURA,COIN_AURA,AURA_END
	};
	typedef struct: public CGameObject::GAMEOBJ_DESC
	{
		AURA_TYPE	eType{};
		LEVELID eLevel{};
		_vector vecPos{};
		_bool* bInteration = {nullptr};
		CCoin* pCoin = nullptr;
		CCoin_Item* pCoin_Item = nullptr;
	}AURA_DESC;

private:
	CAura(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CAura(const CAura& Prototype);
	virtual ~CAura() = default;

public:
	/* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = {nullptr};
	CModel* m_pModelCom = {nullptr};
	CTexture* m_pTextureCom = {nullptr};
	CCoin* m_pCoin = nullptr;
	CCoin_Item* m_pCoin_Item = nullptr;
	_bool* m_bInteration = {nullptr};

private:
	AURA_TYPE	m_eType{};
	LEVELID m_eLevel{};
	_vector m_vecPos{};
	_float3 m_fScale{};
	_float  m_fUVMove{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CAura* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END