#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

// Trail 버퍼용 정점 구조체
typedef struct {
	_float3 vPosition ;  // 월드 공간의 tip 위치
	_float   fAlpha;       // 0.0f ~ 1.0f
}TRAILVERTEX_DESC;

class ENGINE_DLL CVIBuffer_Trail: public CVIBuffer {
protected:
	CVIBuffer_Trail(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;
	void Update_TrailPos(const _float3& tipPos,_float fDeltaTime);

private:
	_uint								m_MaxSegments;
	float								m_FadeSpeed;
	vector<TRAILVERTEX_DESC>		m_Buffer;  // 순환 버퍼
	vector<TRAILVERTEX_DESC> m_Centers;          // 중심점 순환 버퍼
public:
	static CVIBuffer_Trail* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext,
		_uint maxSegments = 30, float fadeSpeed = 1.0f);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END
