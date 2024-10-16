#pragma once
#include "Base.h"

BEGIN(Engine)

class CVIBuffer_Terrain;

class CPicking_Manager final : public CBase
{
private:
	CPicking_Manager();
	virtual ~CPicking_Manager() = default;

public:
	HRESULT Initialize();

public:
	// 마우스 위치 가져오기
	_float3 Get_MousePos_NDC(HWND hWnd, const unsigned int g_iWinSizeX, const unsigned int	g_iWinSizeY);
	// 물체의 범위를 NDC로 변환해주기
	_float4 Object_NDC_Cal(_float2 fPos, _float fSizeX, _float fSizeY, const unsigned int g_iWinSizeX, const unsigned int g_iWinSizeY);
	// 마우스 레이 방향
	void Get_MouseRayDirection(_float3 fPosition ,XMMATRIX invProj, XMMATRIX invView, XMVECTOR* RayPos_Output, XMVECTOR* RayDir_Output);


	// 터레인 피킹
	_float3 Picking_Terrain(XMVECTOR  RayPos, XMVECTOR  RayDir, const _float3* VtxPos, _uint VtxCountX, _uint VtxCountZ);
	_float3 Picking_Terrain_Quad(XMVECTOR  RayPos, XMVECTOR  RayDir, const _float3* VtxPos, _uint VtxCountX, _uint VtxCountZ);

	_float3 Picking_Box_FAILED(XMVECTOR  RayPos, XMVECTOR  RayDir, const _float3* VtxPos);



	void CreateBoundingBox(const _float3& center, const _float3& size, _float3& fMinPoint, _float3& fMaxPoint);
	bool Picking_Box(const _vector& rayOrigin, const _vector& rayDirection, const _float3& fMinPoint, const _float3& fMaxPoint, float& distance, DirectX::BoundingBox box);
	



private:
	class CGameInstance* m_pGameInstance = { nullptr };

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };


private:
	POINT	m_ptMouse{};
	_float3	m_vMousePos = {};
public:
	static CPicking_Manager* Create();
	virtual void Free() override;
};

END