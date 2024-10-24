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

    
#ifdef _DEBUG
private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
#endif
public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex);
	virtual void Free() override;
};

END
