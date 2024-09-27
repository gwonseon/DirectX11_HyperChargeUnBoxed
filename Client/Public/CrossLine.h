#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CCrossLine final : public CUIObject
{
//public:
//	typedef struct : public CGameObject::GAMEOBJ_DESC
//	{
//		_float			fX{}, fY{}, fSizeX{}, fSizeY{};
//	}CROSSLINE_DESC;


private:
	CCrossLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCrossLine(const CCrossLine& Prototype);
	virtual ~CCrossLine() = default;

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
	//_float						m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};
	//_float4x4					m_ViewMatrix, m_ProjMatrix;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	_int iChangeNum = 0;

public:
	static CCrossLine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END