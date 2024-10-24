#include "..\Public\Navigation.h"

#include "Cell.h"
#include "Shader.h"
#include "GameInstance.h"

// 월드 매트릭스가 스태틱
// 네비 메쉬를 변환시킬 수 있는 특정 객체가 업데이트를 통해 월드 매트릭스를 반환시키고
// 다른 객체들은 변환된 월드 매트릭스를 통해 네비게이션 메쉬를 활용하는 구조
_float4x4 CNavigation::m_WorldMatrix = {};


CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
    
}

CNavigation::CNavigation(const CNavigation& Prototype)
    : CComponent{ Prototype }
    , m_Cells{ Prototype.m_Cells }

#ifdef _DEBUG
    , m_pShader{ Prototype.m_pShader } 
#endif
{
    for (auto& pCell : m_Cells)
        Safe_AddRef(pCell);

#ifdef _DEBUG
    Safe_AddRef(m_pShader);
#endif

}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationFilePath)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	_ulong				dwByte = {};
	HANDLE				hFile = CreateFile(pNavigationFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
	{

	}
	else
	{

		while (true)
		{
			_float3		vPoints[3];
			ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

			if (0 == dwByte)
				break;
			_vector vA = XMVectorSet(vPoints[0].x, vPoints[0].y, vPoints[0].z, 1.f);
			_vector vB = XMVectorSet(vPoints[1].x, vPoints[1].y, vPoints[1].z, 1.f);
			_vector vC = XMVectorSet(vPoints[2].x, vPoints[2].y, vPoints[2].z, 1.f);
			_vector vCross = XMVector3Cross(vB - vA, vC - vB);
			_float fDot{};
			_vector vUp = { 0.f,1.f,0.f,0.f };

			XMVECTOR vDot = XMVector3Dot(vCross, vUp);
			XMStoreFloat(&fDot, vDot);
			if (fDot < 0)
			{
				_float3 fNewB{}, fNewC{};
				XMStoreFloat3(&fNewB, vC);
				XMStoreFloat3(&fNewC, vB);
				vPoints[1] = fNewB;
				vPoints[2] = fNewC;
			}
			if (vPoints[0].y == 0)
			{
				vPoints[0].y = 0.1f;
			}
			if (vPoints[1].y == 0)
			{
				vPoints[1].y = 0.1f;
			}
			if (vPoints[2].y == 0)
			{
				vPoints[2].y = 0.1f;
			}
			CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
			if (nullptr == pCell)
				return E_FAIL;

			m_Cells.push_back(pCell);
		}
		CloseHandle(hFile);
	}


#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	SetUp_Neighbor();

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);

	m_iCurrentCellIndex = pDesc->iCurrentCellIndex;

	return S_OK;
}


void CNavigation::Create_Cell(_float3 vPoints[3])
{
	CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
	m_Cells.push_back(pCell);
	SetUp_Neighbor();

}

void CNavigation::Delete_Cell(_uint iIndex)
{
	bool bDel = false;

	for (auto it = m_Cells.begin(); it != m_Cells.end();)
	{
		_uint iCellIndex = (*it)->Get_CellIndex();

		if (bDel)
		{
			(*it)->Set_CellIndex(iCellIndex - 1); // 하나 삭제됐으니까 그 뒤에 애들은 인덱스 1씩 줄어야 함
			++it; // 다음 요소로 이동
		}
		else if (iCellIndex == iIndex)
		{
			Safe_Release(*it); // 삭제할 셀을 안전하게 해제
			it = m_Cells.erase(it); // 삭제 후 반복자를 재설정
			bDel = true; // 삭제가 완료되었음을 표시
		}
		else
		{
			++it; // 조건이 맞지 않으면 다음 요소로 이동
		}
	}
}


void CNavigation::SetUp_Neighbor()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
				pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
				pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
				pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
		}
	}
}

//_bool CNavigation::isMove_Slide(const _vector& vTargetPos, _fvector* vSlidePos)
//{
//	_fvector vLocal_TargetPos = XMVector3TransformCoord(vTargetPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
//	
//	_int iNeighborIndex = { -1 };
//
//	/* 현재 이동하고 난 결과위치가 원래 존재하고 있던 쎌 바깥으로 나갔다. */
//	_bool bNextJump{};
//	if (false == m_Cells[m_iCurrentCellIndex]->isIn(vLocal_TargetPos, &iNeighborIndex, vSlidePos, true, &bNextJump))
//	{
//		// 이웃이 없는 경우
//		if (-1 == iNeighborIndex) 
//			return false;
//		
//	}
//	// 다음 블럭이 점프 블럭이 아닌 경우
//	else
//	{
//		if(iNeighborIndex != -1)
//		{
//			while (true)
//			{
//				if (m_Cells[iNeighborIndex]->isIn(vLocal_TargetPos, &iNeighborIndex, vSlidePos, false, &bNextJump))
//				{
//
//					// 이미 슬라이드가 확정 된 상황에서, 오버한 지점에 대해 검사했더니 true인 경우
//					if (XMVectorGetX(XMVector3Length(*vSlidePos)))
//					{
//						vSlidePos = &vLocal_TargetPos;
//						m_iCurrentCellIndex = iNeighborIndex;
//						return false;
//					}
//
//					// 한번도 슬라이드 상황을 겪지 않고 isin한 경우,
//					break;
//				}
//
//				// 그냥 삼각형 내부에서 슬라이드한 경우.
//				if (-1 == iNeighborIndex) { return false; }
//			}
//		}
//	}
//
//	m_iCurrentCellIndex = iNeighborIndex;
//	return true;
//}


_bool CNavigation::isMove(_vector& vWorldPos, _vector vCurrentPos, _vector& vSlidingPos)
{
	_vector vLocalPos = XMVector3Transform(vWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
	_int iNeighborIndex = { -1 };

	// 현재 셀 내에 위치 확인, 현재 셀 안에 있음
	if (false == m_Cells[m_iCurrentCellIndex]->isIn(vLocalPos, &iNeighborIndex, vSlidingPos, true))
	{
		// 나간 쪽 선분의 이웃을 살펴보자 
		// 근데 그쪽 이웃이 없는데?
		if (-1 == iNeighborIndex)
			return false;

		_uint iInfinite_Check{};
		while (true)
		{
			
			if (m_Cells[iNeighborIndex]->isIn(vLocalPos, &iNeighborIndex, vSlidingPos, false))
			{
				// 이미 슬라이드가 확정 된 상황에서, 오버한 지점에 대해 검사했더니 true인 경우
				if (XMVectorGetX(XMVector3Length(vSlidingPos)))
				{
					vSlidingPos = vLocalPos;
					m_iCurrentCellIndex = iNeighborIndex;
					return false;
				}

				// 한번도 슬라이드 상황을 겪지 않고 isin한 경우,
				break;
			}

			// 그냥 삼각형 내부에서 슬라이드한 경우.
			if (-1 == iNeighborIndex) { return false; }

			iInfinite_Check++;
			if (iInfinite_Check > 100) // 무한루프시 제자리
			{
				vSlidingPos = vCurrentPos;
				return false;
			}
		}
		// 현재 셀을 이웃 셀로 업데이트
		m_iCurrentCellIndex = iNeighborIndex;
		
		
		return true;
	}
	
	return true;  // 현재 셀 내에 있을 경우
}


#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (-1 == m_iCurrentCellIndex)
	{
		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
		_float4 vColorGreen = _float4(0.f, 1.f, 0.f, 1.f);
		m_pShader->Bind_RawValue("g_vColor", &vColorGreen, sizeof(_float4));

		m_pShader->Begin(0);

		for (auto& pCell : m_Cells)
			pCell->Render();
	}

	else
	{
		_float4x4		WorldMatrix = m_WorldMatrix;
		WorldMatrix.m[3][1] += 0.1f;
		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;
		_float4 vColor = _float4(1.f, 0.f, 0.f, 1.f);
		m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

		m_pShader->Begin(0);

		m_Cells[m_iCurrentCellIndex]->Render();
	}

	m_pShader->Begin(0);

	for (auto& pCell : m_Cells)
		pCell->Render();

	return S_OK;
}
#endif

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationFilePath)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationFilePath)))
	{
		MSG_BOX("Failed To Created : CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif
}
