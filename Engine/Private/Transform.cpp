#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"
#include "GameInstance.h"

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

void CTransform::Go_Straight_Nav(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	if (m_iCurrent_JumpState == 0 || m_iCurrent_JumpState == 3)
	{
		if (m_fJumpSpeed < 1.0f)
		{
			m_fJumpSpeed += fTimeDelta;
		}
	}
	else
	{
		if (m_fJumpSpeed > 0.1f)
			m_fJumpSpeed -= (fTimeDelta * 0.2f);
	}

	_vector vCurrentPos = Get_State(CTransform::STATE_POSITION);
	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta * m_fJumpSpeed;
	_vector vecSlidingPos{};

	if (nullptr != pNavigation && false == pNavigation->isMove(vPosition, vCurrentPos, vecSlidingPos))
	{
		vPosition = vecSlidingPos;
	}
	
	Set_State(CTransform::STATE_POSITION, vPosition);
}
void CTransform::Go_Backward_Nav(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	if (m_iCurrent_JumpState == 0 || m_iCurrent_JumpState == 3)
	{
		if (m_iCurrent_JumpState == 0 || m_iCurrent_JumpState == 3)
		{
			if (m_fJumpSpeed < 1.0f)
			{
				m_fJumpSpeed += fTimeDelta;
			}
		}
		else
		{
			if (m_fJumpSpeed > 0.1f)
				m_fJumpSpeed -= (fTimeDelta * 0.2f);
		}
	}
	_vector vCurrentPos = Get_State(CTransform::STATE_POSITION);
	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta * m_fJumpSpeed;
	_vector vecSlidingPos{};
	if (nullptr != pNavigation && false == pNavigation->isMove(vPosition, vCurrentPos, vecSlidingPos))
	{
		vPosition = vecSlidingPos;
	}
	Set_State(CTransform::STATE_POSITION, vPosition);
}
void CTransform::Go_Straight_Nav_Type2(_float fTimeDelta, _vector vPos, CNavigation* pNavigation)
{
	_vector vCurrentPos = Get_State(CTransform::STATE_POSITION);
	_vector vecSlidingPos{};
	if (nullptr != pNavigation && false == pNavigation->isMove(vPos, vCurrentPos, vecSlidingPos))
	{
		vPos = vecSlidingPos;
	}
	Set_State(CTransform::STATE_POSITION, vPos);
}



void CTransform::Go_Right_Nav(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	if (m_iCurrent_JumpState == 0 || m_iCurrent_JumpState == 3)
	{
		if (m_iCurrent_JumpState == 0 || m_iCurrent_JumpState == 3)
		{
			if (m_fJumpSpeed < 1.0f)
			{
				m_fJumpSpeed += fTimeDelta;
			}
		}
		else
		{
			if (m_fJumpSpeed > 0.1f)
				m_fJumpSpeed -= (fTimeDelta * 0.2f);
		}
	}

	_vector vCurrentPos = Get_State(CTransform::STATE_POSITION);
	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta * m_fJumpSpeed;
	_vector vecSlidingPos{};


	if (nullptr != pNavigation && false == pNavigation->isMove(vPosition, vCurrentPos, vecSlidingPos))
	{
		vPosition = vecSlidingPos;
	}
	Set_State(CTransform::STATE_POSITION, vPosition);

}
void CTransform::Go_Left_Nav(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);

	if (m_iCurrent_JumpState == 0 || m_iCurrent_JumpState == 3)
	{
		if (m_iCurrent_JumpState == 0 || m_iCurrent_JumpState == 3)
		{
			if (m_fJumpSpeed < 1.0f)
			{
				m_fJumpSpeed += fTimeDelta;
			}
		}
		else
		{
			if (m_fJumpSpeed > 0.1f)
				m_fJumpSpeed -= (fTimeDelta * 0.2f);
		}
	}
	_vector vCurrentPos = Get_State(CTransform::STATE_POSITION);
	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta * m_fJumpSpeed;
	_vector vecSlidingPos{};

	if (nullptr != pNavigation && false == pNavigation->isMove(vPosition, vCurrentPos, vecSlidingPos))
	{
		vPosition = vecSlidingPos;
	}
	Set_State(CTransform::STATE_POSITION, vPosition);

}


/*
	
*/
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

void CTransform::Go_Straight_Player(_float fTimeDelta)
{
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPos = Get_State(CTransform::STATE_POSITION);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	_float3 vHeight{};
	m_pGameInstance->isComputeHeight(vPosition, &vHeight);
	_float fPlayerHeight = XMVectorGetY(vPosition);
	_float fGroundHeight = vHeight.y;
	// 플레이어 높이가 더 높을 때( 둘의 차이가 많이 날 때)
	if (fPlayerHeight > fGroundHeight && (fPlayerHeight - fGroundHeight) > 1.f)
	{
		vPosition = XMVectorSet(XMVectorGetX(vPosition), fPlayerHeight, XMVectorGetZ(vPosition), 1.f);
	}
	// 장애물 높이가 많이 더 높을 때 슬라이딩
	else if (fGroundHeight > fPlayerHeight && (fGroundHeight - fPlayerHeight) > 1.f)
	{
		vPosition = vCurrentPos;
	}
	// 플레이어랑 장애물의 높이 차이가 얼마 안날 때
	else if (fGroundHeight > fPlayerHeight && (fGroundHeight - fPlayerHeight) <= 1.f)
	{
		vPosition = XMVectorSet(XMVectorGetX(vPosition), vHeight.y, XMVectorGetZ(vPosition), 1.f);
	}

	Set_State(CTransform::STATE_POSITION, vPosition);

}
void CTransform::Go_Straight_Player(_float fTimeDelta, _float AddfSpeed)
{

	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPos = Get_State(CTransform::STATE_POSITION);

	vPosition += XMVector3Normalize(vLook) * (m_fSpeedPerSec * AddfSpeed) * fTimeDelta;

	_float3 vHeight{};
	m_pGameInstance->isComputeHeight(vPosition, &vHeight);
	_float fPlayerHeight = XMVectorGetY(vPosition);
	_float fGroundHeight = vHeight.y;
	// 플레이어 높이가 더 높을 때( 둘의 차이가 많이 날 때)
	if (fPlayerHeight > fGroundHeight && (fPlayerHeight - fGroundHeight) > 1.f)
	{
		vPosition = XMVectorSet(XMVectorGetX(vPosition), fPlayerHeight, XMVectorGetZ(vPosition), 1.f);
	}
	// 장애물 높이가 많이 더 높을 때 
	else if (fGroundHeight > fPlayerHeight && (fGroundHeight - fPlayerHeight) > 1.f)
	{
		vPosition = vCurrentPos;
	}
	// 플레이어랑 장애물의 높이 차이가 얼마 안날 때
	else if ((fGroundHeight > fPlayerHeight) && ((fGroundHeight - fPlayerHeight) <= 1.f))
	{
		vPosition = XMVectorSet(XMVectorGetX(vPosition), vHeight.y, XMVectorGetZ(vPosition), 1.f);
	}

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Left_Player(_float fTimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPos = Get_State(CTransform::STATE_POSITION);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	_float3 vHeight{};
	m_pGameInstance->isComputeHeight(vPosition, &vHeight);
	_float fPlayerHeight = XMVectorGetY(vPosition);
	_float fGroundHeight = vHeight.y;
	// 플레이어 높이가 더 높을 때( 둘의 차이가 많이 날 때)
	if (fPlayerHeight > fGroundHeight && (fPlayerHeight - fGroundHeight) > 1.f)
	{
		vPosition = XMVectorSet(XMVectorGetX(vPosition), fPlayerHeight, XMVectorGetZ(vPosition), 1.f);
	}
	// 장애물 높이가 많이 더 높을 때 슬라이딩
	else if (fGroundHeight > fPlayerHeight && (fGroundHeight - fPlayerHeight) > 1.f)
	{
		vPosition = vCurrentPos;
	}
	// 플레이어랑 장애물의 높이 차이가 얼마 안날 때
	else if (fGroundHeight > fPlayerHeight && (fGroundHeight - fPlayerHeight) <= 1.f)
	{
		vPosition = XMVectorSet(XMVectorGetX(vPosition), vHeight.y, XMVectorGetZ(vPosition), 1.f);
	}
	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Right_Player(_float fTimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPos = Get_State(CTransform::STATE_POSITION);
	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;
	_float3 vHeight{};
	m_pGameInstance->isComputeHeight(vPosition, &vHeight);
	_float fPlayerHeight = XMVectorGetY(vPosition);
	_float fGroundHeight = vHeight.y;
	// 플레이어 높이가 더 높을 때( 둘의 차이가 많이 날 때)
	if (fPlayerHeight > fGroundHeight && (fPlayerHeight - fGroundHeight) > 1.f)
	{
		vPosition = XMVectorSet(XMVectorGetX(vPosition), fPlayerHeight, XMVectorGetZ(vPosition), 1.f);
	}
	// 장애물 높이가 많이 더 높을 때 슬라이딩
	else if (fGroundHeight > fPlayerHeight && (fGroundHeight - fPlayerHeight) > 1.f)
	{
		vPosition = vCurrentPos;
	}
	// 플레이어랑 장애물의 높이 차이가 얼마 안날 때
	else if (fGroundHeight > fPlayerHeight && (fGroundHeight - fPlayerHeight) <= 1.f)
	{
		vPosition = XMVectorSet(XMVectorGetX(vPosition), vHeight.y, XMVectorGetZ(vPosition), 1.f);
	}
	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Backward_Player(_float fTimeDelta)
{
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Gravity(_vector vPos, _float fTimeDelta, _float fMinHeight)
{
	_vector		vUp = Get_State(CTransform::STATE_UP);
	if (XMVectorGetY(vPos) > fMinHeight)
	{
		vPos += vUp * -9.8f * fTimeDelta * 2.f;
		if(XMVectorGetY(vPos) >= fMinHeight)
			Set_State(CTransform::STATE_POSITION, vPos);
		else
		{
			vPos = XMVectorSetY(vPos, fMinHeight);
			Set_State(CTransform::STATE_POSITION, vPos);
		}
	}
}

void CTransform::Jump(_float fTimeDelta, _float& fHeight, _float& fPower, _uint iJumpState, _uint iJumpCount, _float fMinHeight)
{
	_vector		vLook = Get_State(CTransform::STATE_UP);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	m_iCurrent_JumpState = iJumpCount;
	float fHeight_ = XMVectorGetY(vPosition);
	
 	if(fHeight_ >= fMinHeight)
	{
		if(iJumpCount == 1)
		{
			if (iJumpState == 2)
				fPower -= 0.8f;
		}
		if (iJumpCount == 2)
		{
			if (iJumpState == 2)
				fPower -= 1.f;
		}
		
		vPosition += XMVector3Normalize(vLook) * fPower * fTimeDelta;
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
	else
	{
		fPower = 0.f;
		m_iCurrent_JumpState = 0;
		vPosition = XMVectorSetY(vPosition, 0.f);
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
	
	fHeight = fHeight_;
	
}
void CTransform::Set_Min_Height()
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	if(XMVectorGetY(vPosition) < 0.f)
	{
		vPosition = XMVectorSetY(vPosition, 0.f);
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
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

_bool CTransform::KnockBack(_float fTimeDelta, _vector vKnockBackDir, _float& fPower, _float StartHeight)
{
	_vector		vLook = Get_State(CTransform::STATE_UP);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	float fHeight = XMVectorGetY(vPosition);
	if (fHeight <= StartHeight && fPower< 0)
	{
		fPower = 0.f;
		XMVectorSetY(vPosition, StartHeight); // 항상 높이가 일정한 경우의 넉백임
		return true;
	}
	else
	{
		vPosition += XMVector3Normalize(vLook) * fPower* 2.f * fTimeDelta;	// 위로 날아가기 
		
		vPosition += XMVector3Normalize(vKnockBackDir) *  0.6f; // 피격의 반대 방향으로 날아가기
		fPower -= 0.5f;
	}
	Set_State(CTransform::STATE_POSITION, vPosition);
	return false;
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



_bool CTransform::IsPass_TargetPosition(_vector prevPos, _vector currentPos, _vector targetPos)
{
	currentPos = XMVectorSetY(currentPos, 0.f);
	prevPos = XMVectorSetY(prevPos, 0.f);
	targetPos = XMVectorSetY(targetPos, 0.f);
	_vector directionToTarget = targetPos - currentPos;// XMVectorSubtract(targetPos, currentPos);
	_vector previousDirection = targetPos - prevPos;		// XMVectorSubtract(targetPos, prevPos);

	float dotProduct = XMVectorGetX(XMVector3Dot(directionToTarget, previousDirection));
	if (dotProduct < 0.0f) // 음수면 목표 지나침
	{
		return true;
	}
	return false; 


}

vector<_float3> CTransform::PathFind(_float fTimeDelta, CNavigation* pNavigation,_int StartCell_Idx, _int TargetCell_Idx)
{
	vector<_float3> vecPath = pNavigation->Find_Path_AStar(StartCell_Idx, TargetCell_Idx);

	return vecPath;
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
