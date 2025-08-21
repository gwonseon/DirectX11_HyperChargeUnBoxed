#pragma once

/* 애견, 게임(롤), 술(2~3병), 노래(코인), 운동(축구, 클라이밍, 헬스), 책, 유튜브, 영화(연인?), 요리(베이커리 등등) */
/* 니네 가지고는 안되겠다. */

#include "Base.h"

/* 다양한 컴포넌트들의 부모가 되는 클래스. */
BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CComponent(const CComponent& Prototype);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render() {
		return S_OK;
	}
protected:
	ID3D11Device* m_pDevice = {nullptr};
	ID3D11DeviceContext* m_pContext = {nullptr};
	class CGameInstance* m_pGameInstance = {nullptr};

	_bool						m_isCloned = {false};

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END