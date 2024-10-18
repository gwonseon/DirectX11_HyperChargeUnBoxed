#pragma once
#include "Base.h"
#include "Layer.h"
#include "GameObject.h"
#include "Collider.h"

BEGIN(Engine)


class CCollisionMgr final : public CBase
{

private:
	CCollisionMgr();
	virtual ~CCollisionMgr() = default;

public:
	HRESULT Initialize();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	void Collision_Layer(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID = 0, _uint iDstPartObjID = 0);
	void Collision_Layer_Coin(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID = 0, _uint iDstPartObjID = 0);

public:
	virtual void Free() override;
};

END