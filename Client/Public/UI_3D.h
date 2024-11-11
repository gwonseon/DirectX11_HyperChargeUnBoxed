#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include <Player.h>
#include <Missile_Truck.h>
#include "Camera_Free.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_3D final : public CGameObject
{
public:
	enum UI_OBJECT_TYPE { UI_NUCLEAR, UI_HP, UI_OBJ_END};

	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		UI_OBJECT_TYPE eUIType{};
		LEVELID m_eLevel{};
		CPlayer* pPlayer = { nullptr };
		CCamera_Free* pCamera = { nullptr };
		CMissile_Truck* m_pMissile_Truck = { nullptr };
	}UIOBJ_DESC;

private:
	CUI_3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_3D(const CUI_3D& Prototype);
	virtual ~CUI_3D() = default;

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
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CPlayer* m_pPlayer = { nullptr };
	CMissile_Truck* m_pMissile_Truck = { nullptr };
	CTracker* m_pTracker = { nullptr };
	CCamera_Free* m_pCamera = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	void	Nuclear(_float fTimeDelta);
private:
	LEVELID m_eLevel{};
	UI_OBJECT_TYPE m_eUIType{};

	_float m_fDistance = 0.f;
	_float m_fScale{};
	XMFLOAT4X4 viewMatrixFloat4x4{};
public:
	static CUI_3D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END