#pragma once
#include "Base.h"
#include "VIBuffer_Terrain.h"


BEGIN(Engine)
class CTerrain_Manager final : public CBase
{
private:
	CTerrain_Manager();
	virtual ~CTerrain_Manager() = default;

public:
	CVIBuffer_Terrain* Create_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath);
	HRESULT	Load_Level(_uint iLevelID, const _wstring& TerrainFileName);


	void	Picking_Create_Env(_float3 fRayDir, XMMATRIX invView);

private:
	void	Load_Terrain(const _wstring& TerrainFileName);

};

END