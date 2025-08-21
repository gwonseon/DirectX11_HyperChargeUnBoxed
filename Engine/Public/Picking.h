#pragma once
#include "Base.h"

BEGIN(Engine)

class CPicking final: public CBase
{
private:
	CPicking(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd,_uint iViewportWidth,_uint iViewportHeight);
	_bool isPicked(_float3* pOut);
	_bool isComputeHeight(_fvector vTargetPos,_float3* pOut);


private:
	HWND							m_hWnd = {};
	_uint							m_iViewportWidth{},m_iViewportHeight{};

	ID3D11Device* m_pDevice = {nullptr};
	ID3D11DeviceContext* m_pContext = {nullptr};
	/* 복사 받아오기위한 용도 */
	/* 락, 언락하면서 특정 픽셀의 정보를 얻어온다. */
	ID3D11Texture2D* m_pTexture2D = {nullptr};

	class CGameInstance* m_pGameInstance = {nullptr};

public:
	static CPicking* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext,HWND hWnd,_uint iViewportWidth,_uint iViewportHeight);
	virtual void Free() override;
};

END