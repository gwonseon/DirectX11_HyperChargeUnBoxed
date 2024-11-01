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
	_bool Collision_Bullet(CLayer* Target, const _wstring& strTargetComponentTag, _vector vRayDir, _vector vRayPos, _bool* bShot,_float fDamage ,_uint iTargetPartObjID = 0);
	void Collision_Layer(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID = 0, _uint iDstPartObjID = 0);
	void Collision_Layer_Coin(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID = 0, _uint iDstPartObjID = 0);
	void Collision_Trap(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID = 0, _uint iDstPartObjID = 0);

	void Anti_OverLapping(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID = 0, _uint iDstPartObjID = 0);

public:
	virtual void Free() override;
};

END