#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct
	{
		_int			iCurrentCellIndex = { -1 };
	}NAVIGATION_DESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationFilePath);
	virtual HRESULT Initialize(void* pArg) override;

	// 월드 매트릭스를 변경시키는 함수( 특정 객체들이 호출)
	void Update(const _float4x4* pWorldMatrix) {
		m_WorldMatrix = *pWorldMatrix;
	}


	//임시
public:
	void Create_Cell(_float3 vPoints[3]);
	void Delete_Cell(_uint iIndex);


public:
	void SetUp_Neighbor();
	_bool isMove(_fvector vWorldPos);


#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif

private:
	_int					m_iCurrentCellIndex = { -1 };
	vector<class CCell*>	m_Cells;
	static _float4x4		m_WorldMatrix;

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};


END