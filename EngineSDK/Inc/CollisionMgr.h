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


public:
	_bool Collision_Bullet(CLayer* Target, const _wstring& strTargetComponentTag, _vector vRayDir, _vector vRayPos, _bool* bShot,_float fDamage ,_uint iTargetPartObjID = 0);
	void Collision_Layer(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID = 0, _uint iDstPartObjID = 0);
	void Collision_Layer_Coin(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID = 0, _uint iDstPartObjID = 0);
	void Collision_Trap(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID = 0, _uint iDstPartObjID = 0);

	void Collision_Explosion(CLayer* pExplosionLayer, CLayer* pAttackedLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag,_uint iCount ,_uint iSrcPartObjID = 0, _uint iDstPartObjID = 0);
	//void Collision_Explosion(CLayer* pExplosionLayer,const _wstring& strExplosionComponentTag,_uint iCount, _uint irExplosionPartObjID = 0, CCollider* pCollider);

	// 밀어내기, 파파고 영어 어렵넹
	void Anti_OverLapping(CLayer* pSrcLayer, CLayer* pDstLayer, const _wstring& strSrcComponentTag, const _wstring& strDstComponentTag, _uint iSrcPartObjID = 0, _uint iDstPartObjID = 0);
	void Anti_OverLapping_SameLayer(CLayer* pSrcLayer, const _wstring& strSrcComponentTag, _uint iSrcPartObjID = 0);

public:
	virtual void Free() override;
};

END