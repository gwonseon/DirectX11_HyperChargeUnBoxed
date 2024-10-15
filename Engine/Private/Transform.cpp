#include "..\Public\Transform.h"
#include "Shader.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}



HRESULT CTransform::Initialize_Prototype(void* pTransformDesc)
{
	if (nullptr != pTransformDesc)
	{
		TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pTransformDesc);

		m_fSpeedPerSec = pDesc->fSpeedPerSec;
		m_fRotationPerSec = pDesc->fRotationPerSec;
	}

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	return S_OK;
}

void CTransform::Set_Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * fScaleX);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * fScaleY);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * fScaleZ);
}

void CTransform::LookAt(_fvector vAt)
{
	_float3	vScaled = Get_Scaled();

	_vector vLook = vAt - Get_State(STATE_POSITION);
	_vector	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector	vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);

}

void CTransform::Go_Straight(_float fTimeDelta)
{
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
	Set_State(CTransform::STATE_POSITION, vPosition);

}

void CTransform::Go_Straight(_float fTimeDelta, _float AddfSpeed)
{
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);

	vPosition += XMVector3Normalize(vLook) * (m_fSpeedPerSec * AddfSpeed) * fTimeDelta;
	Set_State(CTransform::STATE_POSITION, vPosition);

}
void CTransform::Go_Left(_float fTimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;
	Set_State(CTransform::STATE_POSITION, vPosition);

}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	
	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;
	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
	Set_State(CTransform::STATE_POSITION, vPosition);

}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector	vRight = Get_State(STATE_RIGHT);
	_vector	vUp = Get_State(STATE_UP);
	_vector	vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn(_bool bX, _bool bY, _bool bZ, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_float		fRotationSpeed = m_fRotationPerSec * fTimeDelta;

	_vector		vQuaternion = XMQuaternionRotationRollPitchYaw(bX * fRotationSpeed, bY * fRotationSpeed, bZ * fRotationSpeed);

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_float fX, _float fY, _float fZ)
{
	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_vector		vQuaternion = XMQuaternionRotationRollPitchYaw(fX, fY, fZ);

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Jump(_float fTimeDelta, _float& fHeight, _float& fPower, _uint iJumpState)
{
	
	_vector		vLook = Get_State(CTransform::STATE_UP);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);

	float fHeight_ = XMVectorGetY(vPosition);
	if(fHeight_ >= 0.f )
	{
		if(iJumpState == 2)
			fPower -= 0.4f;
		
		vPosition += XMVector3Normalize(vLook) * fPower * fTimeDelta;
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
	else
	{
		fPower = 0.f;
		vPosition = XMVectorSetY(vPosition, 0.f);
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
	
	fHeight = fHeight_;
	
}

void CTransform::Set_Min_Height()
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	vPosition = XMVectorSetY(vPosition, 0.f);
	Set_State(CTransform::STATE_POSITION, vPosition);
}

_float CTransform::Cal_Distance(_float3 fObj, _float3 fTarget)
{
	_float fDistance = ((fObj.x - fTarget.x) * (fObj.x - fTarget.x)) +
		((fObj.y - fTarget.y) * (fObj.y - fTarget.y)) +
		((fObj.z - fTarget.z) * (fObj.z - fTarget.z));
	return fDistance;
}

_float CTransform::Cal_Distance_vec(_vector vObj, _vector vTarget)
{
	_float3 fObj{}, fTarget{};
	XMStoreFloat3(&fObj, vObj);
	XMStoreFloat3(&fTarget, vTarget);
	_float fDistance = ((fObj.x - fTarget.x) * (fObj.x - fTarget.x)) +
		((fObj.y - fTarget.y) * (fObj.y - fTarget.y)) +
		((fObj.z - fTarget.z) * (fObj.z - fTarget.z));
	
	return fDistance;
}

_float CTransform::Cal_Distance_No_Height(_float3 fObj, _float3 fTarget)
{
	_float fDistance = ((fObj.x - fTarget.x) * (fObj.x - fTarget.x)) +
		((fObj.z - fTarget.z) * (fObj.z - fTarget.z));
	return fDistance;
}

_float CTransform::Cal_Distance_vec_No_Height(_vector vObj, _vector vTarget)
{
	_float3 fObj{}, fTarget{};
	XMStoreFloat3(&fObj, vObj);
	XMStoreFloat3(&fTarget, vTarget);
	_float fDistance = ((fObj.x - fTarget.x) * (fObj.x - fTarget.x)) +
		((fObj.z - fTarget.z) * (fObj.z - fTarget.z));

	return fDistance;
}



HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
	
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pTransformDesc)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTransformDesc)))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	return nullptr;
}


void CTransform::Free()
{
	__super::Free();
}
