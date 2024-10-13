#include "..\Public\Picking_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"

CPicking_Manager::CPicking_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);

}

HRESULT CPicking_Manager::Initialize()
{
	return S_OK;
}

// 뷰포트에서 투영까지
_float3 CPicking_Manager::Get_MousePos_NDC(HWND hWnd, const unsigned int g_iWinSizeX, const unsigned int	g_iWinSizeY)
{

    GetCursorPos(&m_ptMouse);
    ScreenToClient(hWnd, &m_ptMouse);

    // NDC 계산, 정규화된 장치 
    m_vMousePos.x = _float(m_ptMouse.x / (g_iWinSizeX * 0.5f) - 1.f);
    m_vMousePos.y = _float(m_ptMouse.y / -(g_iWinSizeY * 0.5f) + 1.f);
	m_vMousePos.z = 0.f;
    return m_vMousePos;
}

_float4 CPicking_Manager::Object_NDC_Cal(_float2 fPos, _float fSizeX, _float fSizeY, const unsigned int g_iWinSizeX, const unsigned int g_iWinSizeY)
{

	_float4 fObjectRange;
	// 오브젝트의 범위 계산 
	fObjectRange.w = fPos.x - (fSizeX / 2);
	fObjectRange.x = fPos.x + (fSizeX / 2);

	fObjectRange.y = fPos.y - (fSizeY / 2);
	fObjectRange.z = fPos.y + (fSizeY / 2);

	// NDC 로 변환
	_float4 fResult;
	fResult.w = fObjectRange.w / (g_iWinSizeX * 0.5f) - 1.f; // 왼
	fResult.x = fObjectRange.x / (g_iWinSizeX * 0.5f) - 1.f; // 오

	fResult.y = (fObjectRange.y / -(g_iWinSizeY * 0.5f) + 1.f); // 위
	fResult.z = (fObjectRange.z / -(g_iWinSizeY * 0.5f) + 1.f); // 아래

	return fResult;
}

void CPicking_Manager::Get_MouseRayDirection(_float3 fPosition,  XMMATRIX invProj, XMMATRIX invView, XMVECTOR* RayPos_Output, XMVECTOR* RayDir_Output)
{

    _vector vMousePos = XMLoadFloat3(&fPosition);
    vMousePos = XMVectorSetW(vMousePos, 1.f);

    vMousePos = XMVector3TransformCoord(vMousePos, invProj);

    _vector		vRayDir, vRayPos;
    vRayPos = { 0.f, 0.f, 0.f };
    vRayDir = vMousePos - vRayPos;
    
    vRayPos = XMVector3TransformCoord(vRayPos, invView);
    vRayDir = XMVector3TransformNormal(vRayDir, invView);
    
 
    *RayPos_Output = vRayPos;
    *RayDir_Output = vRayDir;
 
   //  레이 방향 위치 확인
    _float3 fRayPos, fRayDir;
     XMStoreFloat3(&fRayPos, vRayPos);
   XMStoreFloat3(&fRayDir, vRayDir);
   if(GetKeyState(VK_NUMPAD6))
       cout <<"레이 위치" << fRayPos.x << " " << fRayPos.y << " " << fRayPos.z << endl;
    

}

_float3 CPicking_Manager::Picking_Terrain(XMVECTOR RayPos, XMVECTOR RayDir, const _float3* VtxPos, _uint VtxCountX, _uint VtxCountZ)
{
    const _float3* pTerrainVtx = VtxPos;
    
    _ulong dwVtxIdx[3]{};
    float closestDist = 0.f;  // 가장 가까운 충돌 거리를 저장할 변수
    _float3 hitPoint = _float3(0.f, 0.f, 0.f);  // 충돌 지점을 저장할 변수

    // 레이 위치랑 방향 확인
    //_float3 fRayPos, fRayDir;
    //XMStoreFloat3(&fRayPos, RayDir);
    //cout << fRayPos.x << "  " << fRayPos.z << "  " << fRayPos.y << endl;

    RayDir = XMVector3Normalize(RayDir);  
  
    for (_ulong i = 0; i < VtxCountZ - 1; ++i)
    {
        for (_ulong j = 0; j < VtxCountX - 1; ++j)
        {
            _ulong dwIndex = i * VtxCountX + j;
           
            dwVtxIdx[0] = dwIndex + VtxCountX;
            dwVtxIdx[1] = dwIndex + VtxCountX + 1;
            dwVtxIdx[2] = dwIndex + 1;

            _float3 v0 = pTerrainVtx[dwVtxIdx[0]];
            _float3 v1 = pTerrainVtx[dwVtxIdx[1]];
            _float3 v2 = pTerrainVtx[dwVtxIdx[2]];

            float dist = 0.0f;
            // 충돌 검사
            if (DirectX::TriangleTests::Intersects(
                RayPos,
                RayDir,  
                XMLoadFloat3(&v0),
                XMLoadFloat3(&v1),
                XMLoadFloat3(&v2),
                dist))
            {
                XMVECTOR xmHitPoint = RayPos + RayDir * dist;
                XMStoreFloat3(&hitPoint, xmHitPoint);
   //             cout << "1 : " << hitPoint.x << "  " << hitPoint.z << "  " << hitPoint.y << endl;
            }

            dwVtxIdx[0] = dwIndex + VtxCountX;
            dwVtxIdx[1] = dwIndex + 1;
            dwVtxIdx[2] = dwIndex;

            v0 = pTerrainVtx[dwVtxIdx[0]];
            v1 = pTerrainVtx[dwVtxIdx[1]];
            v2 = pTerrainVtx[dwVtxIdx[2]];

            dist = 0.0f;
            if (DirectX::TriangleTests::Intersects(
                RayPos,
                RayDir,  // 정규화된 방향 벡터 사용
                XMLoadFloat3(&v0),
                XMLoadFloat3(&v1),
                XMLoadFloat3(&v2),
                dist))
            {
                XMVECTOR xmHitPoint = RayPos + RayDir * dist;
                XMStoreFloat3(&hitPoint, xmHitPoint);
     //           cout << "2 : " << hitPoint.x << "  " << hitPoint.z << "  " << hitPoint.y << endl;
            }
        }
    }

    return hitPoint;  // 가장 가까운 충돌 지점을 반환
}

_float3 CPicking_Manager::Picking_Box_FAILED(XMVECTOR RayPos, XMVECTOR RayDir, const _float3* VtxPos)
{
    const _float3* pBoxVtx = VtxPos;
    float closestDist = 0.f;  // 가장 가까운 충돌 거리를 저장할 변수
    _float3 hitPoint = _float3(0.f, 0.f, 0.f);  // 충돌 지점을 저장할 변수
    RayDir = XMVector3Normalize(RayDir);

    for (_ulong i = 0; i < 12; ++i)
    {
            _ulong dwIndex = i * 3;

            _float3 v0 = pBoxVtx[dwIndex];
            _float3 v1 = pBoxVtx[dwIndex + 1];
            _float3 v2 = pBoxVtx[dwIndex + 2];

            float dist = 0.0f;
            // 충돌 검사
            if (DirectX::TriangleTests::Intersects(
                RayPos,
                RayDir,
                XMLoadFloat3(&v0),
                XMLoadFloat3(&v1),
                XMLoadFloat3(&v2),
                dist))
            {
                XMVECTOR xmHitPoint = RayPos + RayDir * dist;
                XMStoreFloat3(&hitPoint, xmHitPoint);
 //                 cout << "충돌 위치 : " << hitPoint.x << "  " << hitPoint.z << "  " << hitPoint.y << endl;
            }
    }
    return hitPoint;  // 가장 가까운 충돌 지점을 반환
}

void CPicking_Manager::CreateBoundingBox(const _float3& center, const _float3& size, _float3& fMinPoint, _float3& fMaxPoint)
{
    fMinPoint = _float3(center.x - size.x * 0.5f, center.y - size.y * 0.5f, center.z - size.z * 0.5f); 
    fMaxPoint = _float3(center.x + size.x * 0.5f, center.y + size.y * 0.5f, center.z + size.z * 0.5f);
}
bool CPicking_Manager::Picking_Box(const _vector& rayOrigin, const _vector& rayDirection, const _float3& fMinPoint, const _float3& fMaxPoint, float& distance, DirectX::BoundingBox box)
{
    // 넓이
    box.Extents = _float3((fMaxPoint.x - fMinPoint.x), (fMaxPoint.y - fMinPoint.y) , (fMaxPoint.z - fMinPoint.z) );
    // 중점
    box.Center = _float3((fMaxPoint.x + fMinPoint.x) * 0.5f, (fMaxPoint.y + fMinPoint.y) * 0.5f, (fMaxPoint.z + fMinPoint.z) * 0.5f);

    return box.Intersects(rayOrigin, rayDirection, distance);
}



CPicking_Manager* CPicking_Manager::Create()
{
	CPicking_Manager* pInstance = new CPicking_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPicking_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPicking_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

}
