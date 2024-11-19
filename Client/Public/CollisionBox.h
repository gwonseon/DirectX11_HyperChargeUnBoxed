#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "VIBuffer_Box.h"

BEGIN(Engine)
class CShader;
class CModel;

END

BEGIN(Client)


class CCollisionBox final : public CGameObject
{
public:
	typedef struct : CGameObject::GAMEOBJ_DESC
	{
		LEVELID eLevel{};
		_uint iPoint_Number{};
		_uint iImGuiMode{};
		_uint iIndexNumber{};
		_uint iCell_Idx{};
		_uint iCellType = 0;
	}COLLISIONBOX_DESC;




private:
	CCollisionBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollisionBox(const CCollisionBox& Prototype);
	virtual ~CCollisionBox() = default;


public:
	/* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
	virtual HRESULT Initialize_Prototype() override;

	/* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
	/* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한ㄴ다. */
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	void	Set_Position(_vector _vPos) { 
		m_vecPosition = _vPos;
		XMStoreFloat3(&m_fPickingPos, m_vecPosition);
	
	}
	void	Set_Height(_float fHeight)
	{
		m_vecPosition = XMVectorSetY(m_vecPosition, fHeight);
	}
	float	Get_Height() { return XMVectorGetY(m_vecPosition); }
	void	Set_PickingCheck(_bool bCheck) { m_bChecking = bCheck; }
	void	Set_Scale(_float3 fSize) { m_fScale = fSize; }

	void	Set_ImGuiMode(_uint iMode) { m_iCurrentImGuiMode = iMode; }

public:
	void Set_BoundingBos(DirectX::BoundingBox Box) { BoundingBox = Box; }
	
	
	
	DirectX::BoundingBox Get_BoundingBox() { return BoundingBox; }
	_uint	Get_CellIdx() { return m_iCellIdx; }
	_uint   Get_CellType() { return m_iCellType; }
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CVIBuffer_Box* m_pVIBufferCom = { nullptr };
	const _float4x4* m_pSocketMatrix = { nullptr };
	const _uint* m_pParentState = { nullptr };
	_float3* VtxPos{};


private:
	_float3 m_fScale{};
	_float3 m_fClolor{255.f,0.f,0.f};
	_vector m_vecPosition{0.f,0.f,0.f,1.f};

	_uint m_iImGuiMode = 0;
	_uint m_iCurrentImGuiMode{};
	_uint m_iCellType{};


	_uint m_iCellIdx{};
	// navigation
private:
	_uint m_iPointNumber{};  // 배열에 들어가는 인덱스 넘버
	_uint m_iIndexNumber{};	 // 네비에서 찍은 전체 포인트 중에서 인덱스 넘버
	LEVELID	m_eLevel{};
	_bool m_bnavigationMode = false;
	_bool m_bChecking = false;
	_bool m_bBouncing = false;
public:
	_uint Get_IndexNumber() { return m_iIndexNumber; } // 전체 인덱스 번호 
	_uint Get_ArrayNumber() { return m_iPointNumber; } // 배열 인덱스 번호 



	void Set_IndexNumber(_uint iIndex) { m_iIndexNumber = iIndex; } // 전체 인덱스 번호


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


	DirectX::BoundingBox BoundingBox;


public:
	static CCollisionBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
END
