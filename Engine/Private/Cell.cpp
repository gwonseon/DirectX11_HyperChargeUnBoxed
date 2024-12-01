#include "..\Public\Cell.h"

#include "VIBuffer_Cell.h"

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3* pPoints, _uint iIndex, CELL_TYPE eType)
{
	m_iIndex = iIndex; // 셀의 인덱스
	_float fCenterX{}, fCenterY{}, fCenterZ{};
	for (size_t i = 0; i < POINT_END; i++)
	{
		m_vPoints[i] = pPoints[i];
		fCenterX = fCenterX + m_vPoints[i].x;
		fCenterY = fCenterY + m_vPoints[i].y;
		fCenterZ = fCenterZ + m_vPoints[i].z;
	
	}
	m_vCenterPoints = {fCenterX/3, fCenterY/3, fCenterZ/3}; // 중점 좌표 , 3점을 더해서 3으로 나눠줌
	// m_iNeighbors 이 이웃셀
	m_eCellType = eType;
#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

  
_bool CCell::isIn(_vector& vLocalPos, _int* pNeighborIndex, _vector& fSlidePosition,  const _bool& bCalcSlide)
{
	for (size_t i = 0; i < LINE_END; i++)
	{
		_vector		vLine = XMLoadFloat3(&m_vPoints[(i + 1) % POINT_END]) - XMLoadFloat3(&m_vPoints[i]);
		_vector		vNormal = XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f);
		// _vector vNormal = XMVector3Cross(vLine, XMVectorSet(0.f, 1.f, 0.f, 0.f)); // Y축 포함한 법선 계산
		_vector		vDir = vLocalPos - XMLoadFloat3(&m_vPoints[i]);
		_vector		vDot = XMVector3Dot(XMVector3Normalize(vNormal), (vDir));
		if (0 < XMVectorGetX(vDot))
		{
			if(pNeighborIndex)
				*pNeighborIndex = m_iNeighbors[i];



			if (m_iNeighbors[i] == -1)
			{
				_vector vSlidePos = XMLoadFloat3(&m_vPoints[i]) + vDir + -1.f * XMVectorGetX(vDot) * XMVector3Normalize(vNormal);
				_vector vSlideDir = vSlidePos - XMLoadFloat3(&m_vPoints[i]);
				if (XMVectorGetX(XMVector3Length(vLine)) < XMVectorGetX(XMVector3Length(vSlideDir)))
				{
					// 이웃셀이 없으면 이웃셀을 끝점으로 
					if (m_iNeighbors[i] == -1)
						*pNeighborIndex = m_iNeighbors[(i + 1) % 3];
					vLocalPos = XMVectorSet(XMVectorGetX(vSlidePos), XMVectorGetY(vSlidePos), XMVectorGetZ(vSlidePos), 1.f);
					vSlidePos = XMLoadFloat3(&m_vPoints[(i + 1) % 3]);
				}
				else if (XMVectorGetX(XMVector3Dot(vSlideDir, vLine)) < 0.f)
				{
					if (m_iNeighbors[i] == -1)
						*pNeighborIndex = m_iNeighbors[(i - 1 + 3) % 3];
					vLocalPos = XMVectorSet(XMVectorGetX(vSlidePos), XMVectorGetY(vSlidePos), XMVectorGetZ(vSlidePos), 1.f);
					vSlidePos = XMLoadFloat3(&m_vPoints[(i)]);
				}
				fSlidePosition = XMVectorSet(XMVectorGetX(vSlidePos), XMVectorGetY(vSlidePos), XMVectorGetZ(vSlidePos), 1.f);
			}
			return false;
		}
	}

	return true;
}


//_bool CCell::Sliding(_fvector& vLocalPos, _int* pNeighborIndex, _vector vEntervector, _vector& vSlidingPos, _vector vCurrentPos, _bool& bSliding, _float fDistanc)
//{
//	for (size_t i = 0; i < LINE_END; i++)
//	{
//		// 현재 점과 다음 점을 연결하는 벡터 계산
//		_vector vLine = XMLoadFloat3(&m_vPoints[(i + 1) % POINT_END]) - XMLoadFloat3(&m_vPoints[i]);
//		// 법선 벡터 계산
//		_vector vNormal = XMVector3Cross(vLine, XMVectorSet(0.f, 1.f, 0.f, 0.f));
//
//		// 슬라이딩 벡터 계산
//		_vector SlidingVector = vEntervector - XMVectorGetX(XMVector3Dot(vEntervector, vNormal)) * vNormal;
//		SlidingVector = XMVector3Normalize(SlidingVector);
//
//		// 슬라이딩 적용 후 새로운 위치 계산
//		_vector SlidingPos = vCurrentPos + SlidingVector * -1.f; // 슬라이딩 적용 후 위치
//
//		// 슬라이딩 방향 검사
//		_vector Sliding_Dir = SlidingPos - XMLoadFloat3(&m_vPoints[i]);
//
//		// 슬라이딩 방향이 법선 방향과의 내적이 양수인지 검사
//		if (XMVectorGetX(XMVector3Dot(XMVector3Normalize(vNormal), XMVector3Normalize(Sliding_Dir))) > 0)
//		{
//			cout << i << "번 째 위치 선에선 밖으로 나갔네 " << endl;
//			
//			*pNeighborIndex = m_iNeighbors[i]; // 이웃 셀로 이동
//			return false; // 슬라이딩 중단
//		}
//		cout  << "슬라이딩 위치다 : "  << XMVectorGetX(vSlidingPos) << "  " << XMVectorGetY(vSlidingPos) << "  " << XMVectorGetZ(vSlidingPos) << "  " << endl;
//		if (m_iNeighbors[i] != -1)
//		{
//			// 슬라이딩 위치 업데이트
//			vSlidingPos = SlidingPos;
//		}
//	}
//	return true; // 슬라이딩 성공
//}


_bool CCell::Compare_Points(_fvector vSour, _fvector vDest)
{
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vSour))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDest))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDest))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vSour))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDest))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDest))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vSour))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDest))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDest))
			return true;
	}
	return false;
}
#ifdef _DEBUG
HRESULT CCell::Render()
{
	m_pVIBuffer->Bind_Buffers();

	return m_pVIBuffer->Render();

}
#endif
CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex, CELL_TYPE eType)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex, eType)))
	{
		MSG_BOX("Failed To Created : CCell");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCell::Free()
{
	__super::Free();

#ifdef _DEBUG
    Safe_Release(m_pVIBuffer);
#endif
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	
}
