#include "Terrain_Manager.h"



CVIBuffer_Terrain* CTerrain_Manager::Create_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath)
{
    return CVIBuffer_Terrain::Create(pDevice, pContext, pHeightMapFilePath);
}

HRESULT CTerrain_Manager::Load_Level(_uint iLevelID, const _wstring& TerrainFileName)
{

	return E_NOTIMPL;
}

void CTerrain_Manager::Picking_Create_Env(_float3 fRayDir, XMMATRIX invView)
{




}

void CTerrain_Manager::Load_Terrain(const _wstring& TerrainFileName)
{
    wstring FilePath = L"../Bin/Data/" + TerrainFileName + L".dat";

    HANDLE hFile = CreateFile(FilePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        MessageBox(NULL, L"Load Terrain Failed", L"Error", MB_OK);
        return;
    }
    DWORD dwByte = 0;
    _float3 position;
    _ulong textureIndex;
    _float fAngle;

    while (ReadFile(hFile, &position, sizeof(_float3), &dwByte, nullptr) && dwByte > 0)
    {
        ReadFile(hFile, &textureIndex, sizeof(_ulong), &dwByte, nullptr);
        ReadFile(hFile, &fAngle, sizeof(_float), &dwByte, nullptr);

    }

    CloseHandle(hFile);
    MessageBox(NULL, L"Tiles Loaded Successfully", L"Success", MB_OK);
}
