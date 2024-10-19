#pragma once

#include "Component.h"

/* 월드 공간에서의 객체의 상태를 표현하기위한 행렬. */
/* 표현 : 월드행렬을 들고 있음으로서 월드공간에서의 right, up, look, position을 저장하고 있는 기능. */
/* 표현 : 상태 벡터들을 이용해서 월드공간에서의 상태 변환을 수행하는 기능. */

BEGIN(Engine)
// ENGINE_DLL : 클라이언트가 직접 들고 있는 형태
class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

	typedef struct
	{
		_float		fSpeedPerSec = 3;
		_float		fRotationPerSec = 3;
		_float3		fPosition;
		_float3		fScale;
	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTransform() = default;

public:
	void Set_State(STATE eState, _fvector vState) {
		// 월드행렬에 XMVECTOR의 값을 넣는다.
		// XMStoreFloat4의 첫 인자는 XMFLOAT4이다.
		// m_WorldMatrix.m[eState][0] 은 주소니 _float4* 로 형변환해야한다. 
		XMStoreFloat4((_float4*)&m_WorldMatrix.m[eState][0], vState);
	}

	_vector Get_State(STATE eState) {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
		//XMLoadFloat4x4 행렬 데이터를 XMMATRIX로 변환한다. 
		// 이 행렬은 월드 변환 행렬을 저장하고 있는데 이 행렬에서 eState번째 행을 가져오는 역할을 한다.
		// r[0] : X축 방향 벡터 
		// r[1] : Y축 방향 벡터
		// r[2] : Z축 방향 벡터
		// r[3] : 위치 정보
	}

	const _float4x4* Get_WorldMatrixPtr() const {
		return &m_WorldMatrix;
	}
	_matrix Get_WorldMatrix_Inverse()
	{
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}
	_matrix Get_WorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}
	_float4x4* Get_WorldMatrixPtr_Camera() {
		return &m_WorldMatrix;
	}
	_float3 Get_Scaled()
	{
		return _float3(
			XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))),
			XMVectorGetX(XMVector3Length(Get_State(STATE_UP))),
			XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK)))
			);
	}
public:
	virtual HRESULT Initialize_Prototype(void* pTransformDesc);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Set_Scaling(_float fScaleX, _float fScaleY, _float fScaleZ);
	void LookAt(_fvector vAt);
	void Go_Straight(_float fTimeDelta); 
	void Go_Straight(_float fTimeDelta, _float AddfSpeed);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Turn(_fvector vAxis, _float fTimeDelta);
	void Turn(_bool bX, _bool bY, _bool bZ, _float fTimeDelta);
	void Rotation(_float fX, _float fY, _float fZ);
	void Jump(_float fTimeDelta, _float& fHeight, _float& fPower, _uint iJumpState, _uint iJumpCount);
	void Set_Min_Height();


	void Go_Left_Nav(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Right_Nav(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Straight_Nav(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Backward_Nav(_float fTimeDelta, class CNavigation* pNavigation = nullptr);


	_float Cal_Distance(_float3 fObj, _float3 fTarget);
	_float Cal_Distance_vec(_vector vObj, _vector vTarget);
	_float Cal_Distance_No_Height(_float3 fObj, _float3 fTarget);
	_float Cal_Distance_vec_No_Height(_vector vObj, _vector vTarget);

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

private:
	_float4x4					m_WorldMatrix = {};
	_float						m_fSpeedPerSec = {};
	_float						m_fRotationPerSec = {};
	_float3						m_fPosition = {};
	_float3						m_fScale = {};

	_uint						m_iCurrent_JumpState{};
	_float						m_fJumpSpeed{};
public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pTransformDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END