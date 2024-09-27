#pragma once

#include "Model.h"
#include "Shader.h"
#include "Texture.h"

#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"

/* 보관하는역활. */
/* 컴포넌트 원형을 보관한다. */
/* 컴포넌트 원형은 객체 원형과 달리 덩치가 크다. 레벨별로 구분하여 저장할께. */

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	void Clear(_uint iLevelIndex);




private:
	_uint											m_iNumLevels = { 0 };


	map<const _wstring, class CComponent*>* m_pPrototypes = { nullptr };
	typedef map<const _wstring, class CComponent*>	PROTOTYPES;


private:
	class CComponent* Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag);

public:
	static CComponent_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END