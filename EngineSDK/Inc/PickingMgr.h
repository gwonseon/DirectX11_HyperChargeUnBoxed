#pragma once
#include "Base.h"

BEGIN(Engine)

class CPickingMgr final : public CBase
{
private:
	CPickingMgr(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPickingMgr() = default;

public:
	HRESULT Initialize();


public:
	_float2 Get_MousePos_NDC(HWND hWnd, const unsigned int g_iWinSizeX, const unsigned int	g_iWinSizeY);

private:
	class CGameInstance* m_pGameInstance = { nullptr };

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };


private:
	POINT	m_ptMouse{};
	_float2	m_vMousePos = {};
public:
	static CPickingMgr* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END