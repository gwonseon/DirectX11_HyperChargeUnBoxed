#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public: // 셀은 삼각형 단위이기 때문에 점도 선도 각각 3개 씩이다.
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	// POINT 이넘 값을 넣음ㄴ 해당 포인트의 위치를 반환한다.
	_vector Get_Point(POINT ePoint) const {
		return XMLoadFloat3(&m_vPoints[ePoint]);
	}

	// LINE이넘값과 셀을 넣음 입력 받은 셀의 인덱스를 해당 라인에 이웃이라고 판단, 이웃배열에 해당하는 자리에 넣어준다.
	void Set_Neighbor(LINE eLine, CCell* pNeighbor) {
		m_iNeighbors[eLine] = pNeighbor->m_iIndex;
	}

public:
	HRESULT Initialize(const _float3* pPoints, _uint iIndex);
	_bool isIn(_vector& vLocalPos, _int* pNeighborIndex, _vector& fSlidePosition, const _bool& bCalcSlide);
	_bool Compare_Points(_fvector vSour, _fvector vDest);

	_uint	Get_CellIndex() { return m_iIndex; }
	void	Set_CellIndex(_uint iIndex) { m_iIndex = iIndex; }
	_float3 Get_Cell_CenterPos() { return m_vCenterPoints; }
#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	_float3					m_vPoints[POINT_END] = {}; // 3개의 포인트를 보관해야하니 3짜리 배열
	_uint					m_iIndex = {};
	_int					m_iNeighbors[LINE_END] = { -1, -1, -1 }; // 이웃셀의 인덱스 넘버 3개를 보관한다. 이웃이 없으면 -1로 두고 나중에 처리한다.

    

	// 길찾기
private:
	_float3					m_vCenterPoints{};  // 셀의 중점
	_float					G;
	_float					H;
	_float					F;
	CCell*					m_pParent= {nullptr};


public:
	void					Astar_Reset()
	{
		G = 0;  // 현재 노드까지 이동하는 데 소요된 실제 비용
		H = 0;	// 현재 노드에서 목표 노드까지의 예상 비용
		F = 0;	// 현재 노드가 목표에 도달하는데 필요한 전체 비용
		m_pParent = nullptr; // 현재 노드로 오기 직전에 방문한 이전 노드
	}

	void					Set_G(_float fG) { G = fG; }
	void					Set_H(_float fH) { H = fH; }
	void					Set_F(_float fF) { F = fF; }
	void					Set_Parent(CCell* pCell) { m_pParent = pCell; }

	_float					Get_G()							{ return G; }
	_float					Get_H()							{ return H; }
	_float					Get_F()							{ return F; }
	CCell*					Get_Parent()					{ return m_pParent;	}
	_uint					Get_NeighborCell(_uint index)	{ return m_iNeighbors[index]; }
	_float3					Get_CenterPoints()				{ return m_vCenterPoints; }
#ifdef _DEBUG
private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
#endif
public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex);
	virtual void Free() override;
};

END
