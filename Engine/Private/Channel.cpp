#include "..\Public\Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

//HRESULT CChannel::Initialize(CModel* pModel, const aiNodeAnim* pAIChannel)
//{
//	strcpy_s(m_szName, pAIChannel->mNodeName.data);
//
//	m_iBoneIndex = pModel->Get_BoneIndex(m_szName);
//
//	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
//	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);
//
//	_float3		vScale{};
//	_float4		vRotation{};
//	_float3		vPosition{};
//
//	for (size_t i = 0; i < m_iNumKeyFrames; i++)
//	{
//		KEYFRAME			KeyFrame{};
//
//		if (pAIChannel->mNumScalingKeys > i)
//		{
//			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
//			KeyFrame.fTrackPosition = pAIChannel->mScalingKeys[i].mTime;
//		}
//
//		if (pAIChannel->mNumRotationKeys > i)
//		{
//			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
//			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
//			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
//			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
//			KeyFrame.fTrackPosition = pAIChannel->mRotationKeys[i].mTime;
//		}
//
//		if (pAIChannel->mNumPositionKeys > i)
//		{
//			memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
//			KeyFrame.fTrackPosition = pAIChannel->mPositionKeys[i].mTime;
//		}
//
//		KeyFrame.vScale = vScale;
//		KeyFrame.vRotation = vRotation;
//		KeyFrame.vPosition = vPosition;
//
//		m_KeyFrames.push_back(KeyFrame);
//	}
//
//	return S_OK;
//}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentPosition)
{
	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	/* 선형 보간 없이 마지막 키프레임의 상태를 띄면 된다. */
	if (fCurrentPosition >= LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vPosition = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);
	}

	/* 특정 키프레임들 사이에 있는 재생위치였기 때문에. 두 키프레임간 보간작업이 필요하다.  */
	else
	{
		if (0.f == fCurrentPosition)
			m_iCurrentKeyFrameIndex = 0;

		while (fCurrentPosition >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].fTrackPosition)
			++m_iCurrentKeyFrameIndex;

		/* 선형보간하기위해서. */
		/* 왼쪽과 오른쪽사이에서 비율에 맞춰 보간하기위해서. */
		/* 왼쪽 오른쪽 키프레임사이에서의 비율을 구한다. */
		_float		fRatio = (fCurrentPosition - m_KeyFrames[m_iCurrentKeyFrameIndex].fTrackPosition) /
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[m_iCurrentKeyFrameIndex].fTrackPosition);

		_vector		vSourScale, vDestScale;
		_vector		vSourRotation, vDestRotation;
		_vector		vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation);
		vSourPosition = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vPosition), 1.f);

		vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation);
		vDestPosition = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vPosition), 1.f);

		vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, fRatio);
	}

	_matrix			TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformMatrix);
}

//CChannel* CChannel::Create(CModel* pModel, const aiNodeAnim* pAIChannel)
//{
//	CChannel* pInstance = new CChannel();
//
//	if (FAILED(pInstance->Initialize(pModel, pAIChannel)))
//	{
//		MSG_BOX("Failed to Created : CChannel");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}

void CChannel::Free()
{
	__super::Free();

}
