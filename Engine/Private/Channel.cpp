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


void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _float fCurrentPosition)
{
	LastKeyFrame = m_KeyFrames.back();

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
			*pCurrentKeyFrameIndex = 0;

		while (fCurrentPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition)
			++*pCurrentKeyFrameIndex;

		/* 선형보간하기위해서. */
		/* 왼쪽과 오른쪽사이에서 비율에 맞춰 보간하기위해서. */
		/* 왼쪽 오른쪽 키프레임사이에서의 비율을 구한다. */
		_float		fRatio = (fCurrentPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) /
			(m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);

		_vector		vSourScale, vDestScale;
		_vector		vSourRotation, vDestRotation;
		_vector		vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
		vSourPosition = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vPosition), 1.f);

		vDestScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation);
		vDestPosition = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vPosition), 1.f);
			
		vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, fRatio);
	}

	_matrix			TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformMatrix);

}

void CChannel::Update_TransformationMatrix_UpperBody(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _float fCurrentPosition)
{
	LastKeyFrame_UpperBody = m_KeyFrames.back();

	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	/* 선형 보간 없이 마지막 키프레임의 상태를 띄면 된다. */
	if (fCurrentPosition >= LastKeyFrame_UpperBody.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame_UpperBody.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame_UpperBody.vRotation);
		vPosition = XMVectorSetW(XMLoadFloat3(&LastKeyFrame_UpperBody.vPosition), 1.f);
	}

	/* 특정 키프레임들 사이에 있는 재생위치였기 때문에. 두 키프레임간 보간작업이 필요하다.  */
	else
	{
		if (0.f == fCurrentPosition)
			*pCurrentKeyFrameIndex = 0;

		while (fCurrentPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition)
			++*pCurrentKeyFrameIndex;

		/* 선형보간하기위해서. */
		/* 왼쪽과 오른쪽사이에서 비율에 맞춰 보간하기위해서. */
		/* 왼쪽 오른쪽 키프레임사이에서의 비율을 구한다. */
		_float		fRatio = (fCurrentPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) /
			(m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);

		_vector		vSourScale, vDestScale;
		_vector		vSourRotation, vDestRotation;
		_vector		vSourPosition, vDestPosition;




		vSourScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
		vSourPosition = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vPosition), 1.f);

		vDestScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation);
		vDestPosition = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vPosition), 1.f);

		vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, fRatio);


	}
	_matrix			TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	
	if (m_iBoneIndex <=  26)
		Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformMatrix);

}

void CChannel::Update_TransformationMatrix_LowerBody(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _float fCurrentPosition)
{
	LastKeyFrame_LowerBody = m_KeyFrames.back();

	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	/* 선형 보간 없이 마지막 키프레임의 상태를 띄면 된다. */
	if (fCurrentPosition >= LastKeyFrame_LowerBody.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame_LowerBody.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame_LowerBody.vRotation);
		vPosition = XMVectorSetW(XMLoadFloat3(&LastKeyFrame_LowerBody.vPosition), 1.f);
	}

	/* 특정 키프레임들 사이에 있는 재생위치였기 때문에. 두 키프레임간 보간작업이 필요하다.  */
	else
	{
		if (0.f == fCurrentPosition)
			*pCurrentKeyFrameIndex = 0;

		while (fCurrentPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition)
			++*pCurrentKeyFrameIndex;

		/* 선형보간하기위해서. */
		/* 왼쪽과 오른쪽사이에서 비율에 맞춰 보간하기위해서. */
		/* 왼쪽 오른쪽 키프레임사이에서의 비율을 구한다. */
		_float		fRatio = (fCurrentPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) /
			(m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);

		_vector		vSourScale, vDestScale;
		_vector		vSourRotation, vDestRotation;
		_vector		vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
		vSourPosition = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vPosition), 1.f);

		vDestScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation);
		vDestPosition = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vPosition), 1.f);

		vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, fRatio);
	}

	_matrix			TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	if( m_iBoneIndex >= 27)
		Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformMatrix);
	if (m_iBoneIndex == 2 || m_iBoneIndex == 3 || m_iBoneIndex == 0 )
		Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformMatrix);
}

_bool CChannel::Update_LinearInterPolation(const vector<class CBone*>& Bones, vector<string> szName, _float fTimeDelta)
{
	KEYFRAME		FirstKeyFrame = m_KeyFrames.front(); // 현재 애니메이션의 첫 번째 키 프레임

	_vector			vScale{};
	_vector			vRotation{};
	_vector			vPosition{};

	if (m_bInitOnce == false)
	{
		fTemp = 0.f;
		m_bInitOnce = true;
	}
	m_fInterPolation_TargetTime = 0.15f;

	fTemp += fTimeDelta;
	_float	fRatio = fTemp / m_fInterPolation_TargetTime;
	if (fRatio >= 1.f)
		fRatio = 1.f;

	_float4x4 fCurrent_TransformMatrix = Bones[m_iBoneIndex]->Get_TransformationMatrix();
	_matrix currentTransformMatrix = XMLoadFloat4x4(&fCurrent_TransformMatrix);
	_vector vCurrentScale, vCurrentRotation, vCurrentPosition;
	XMMatrixDecompose(&vCurrentScale, &vCurrentRotation, &vCurrentPosition, currentTransformMatrix);

	_vector		vDestScale{}, vDestPosition{}, vDestRotation{};
	vDestPosition = XMVectorSetW(XMLoadFloat3(&FirstKeyFrame.vPosition), 1.f);
	vPosition = XMVectorLerp(vCurrentPosition, vDestPosition, fRatio);
	
	vDestRotation = XMLoadFloat4(&FirstKeyFrame.vRotation);
	vRotation = XMQuaternionSlerp(vCurrentRotation, vDestRotation, fRatio);
	
	vDestScale = XMLoadFloat3(&FirstKeyFrame.vScale);
	vScale = XMVectorLerp(vCurrentScale, vDestScale, fRatio);

	_matrix	TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformMatrix);

	if (fTemp >= m_fInterPolation_TargetTime)
	{
		m_bInitOnce = false;
		return true;
	}

	return false;
}

_bool CChannel::Update_LinearInterPolation_UpperBody(const vector<class CBone*>& Bones, vector<string> szName, _float fTimeDelta)
{
	KEYFRAME		FirstKeyFrame = m_KeyFrames.front(); // 현재 애니메이션의 첫 번째 키 프레임

	_vector			vScale{};
	_vector			vRotation{};
	_vector			vPosition{};

	if (m_bInitOnce_UpperBody == false)
	{
		fTemp_UpperBody = 0.f;
		m_bInitOnce_UpperBody = true;
	}
	m_fInterPolation_TargetTime = 0.15f;

	fTemp_UpperBody += fTimeDelta;
	_float	fRatio = fTemp_UpperBody / m_fInterPolation_TargetTime;
	if (fRatio >= 1.f)
		fRatio = 1.f;

	_float4x4 fCurrent_TransformMatrix = Bones[m_iBoneIndex]->Get_TransformationMatrix();
	_matrix currentTransformMatrix = XMLoadFloat4x4(&fCurrent_TransformMatrix);
	_vector vCurrentScale, vCurrentRotation, vCurrentPosition;
	XMMatrixDecompose(&vCurrentScale, &vCurrentRotation, &vCurrentPosition, currentTransformMatrix);

	_vector		vDestScale{}, vDestPosition{}, vDestRotation{};
	vDestPosition = XMVectorSetW(XMLoadFloat3(&FirstKeyFrame.vPosition), 1.f);
	vPosition = XMVectorLerp(vCurrentPosition, vDestPosition, fRatio);

	vDestRotation = XMLoadFloat4(&FirstKeyFrame.vRotation);
	vRotation = XMQuaternionSlerp(vCurrentRotation, vDestRotation, fRatio);

	vDestScale = XMLoadFloat3(&FirstKeyFrame.vScale);
	vScale = XMVectorLerp(vCurrentScale, vDestScale, fRatio);

	_matrix	TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	if (m_iBoneIndex <= 26)
		Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformMatrix);
	if (fTemp_UpperBody >= m_fInterPolation_TargetTime)
	{
		m_bInitOnce_UpperBody = false;
		return true;
	}

	return false;
}

_bool CChannel::Update_LinearInterPolation_LowerBody(const vector<class CBone*>& Bones, vector<string> szName, _float fTimeDelta)
{
	KEYFRAME		FirstKeyFrame = m_KeyFrames.front(); // 현재 애니메이션의 첫 번째 키 프레임

	_vector			vScale{};
	_vector			vRotation{};
	_vector			vPosition{};

	if (m_bInitOnce_LowerBody == false)
	{
		fTemp_LowerBody = 0.f;
		m_bInitOnce_LowerBody = true;
	}
	m_fInterPolation_TargetTime = 0.15f;

	fTemp_LowerBody += fTimeDelta;
	_float	fRatio = fTemp_LowerBody / m_fInterPolation_TargetTime;
	if (fRatio >= 1.f)
		fRatio = 1.f;

	_float4x4 fCurrent_TransformMatrix = Bones[m_iBoneIndex]->Get_TransformationMatrix();
	_matrix currentTransformMatrix = XMLoadFloat4x4(&fCurrent_TransformMatrix);
	_vector vCurrentScale, vCurrentRotation, vCurrentPosition;
	XMMatrixDecompose(&vCurrentScale, &vCurrentRotation, &vCurrentPosition, currentTransformMatrix);

	_vector		vDestScale{}, vDestPosition{}, vDestRotation{};
	vDestPosition = XMVectorSetW(XMLoadFloat3(&FirstKeyFrame.vPosition), 1.f);
	vPosition = XMVectorLerp(vCurrentPosition, vDestPosition, fRatio);

	vDestRotation = XMLoadFloat4(&FirstKeyFrame.vRotation);
	vRotation = XMQuaternionSlerp(vCurrentRotation, vDestRotation, fRatio);

	vDestScale = XMLoadFloat3(&FirstKeyFrame.vScale);
	vScale = XMVectorLerp(vCurrentScale, vDestScale, fRatio);

	_matrix	TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	if (m_iBoneIndex >= 27)
		Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformMatrix);
	if (m_iBoneIndex == 2 || m_iBoneIndex == 3 || m_iBoneIndex == 0 )
		Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformMatrix);

	if (fTemp_LowerBody >= m_fInterPolation_TargetTime)
	{
		m_bInitOnce_LowerBody = false;
		return true;
	}

	return false;
}

void CChannel::Free()
{
	__super::Free();

}

CChannel* CChannel::Create(CModel* pModel, HANDLE hFileRead)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pModel, hFileRead)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CChannel::Initialize(CModel* pModel, HANDLE hFileRead)
{
	_uint iChannelNameLen = 0;
	ReadFile(hFileRead, &iChannelNameLen, sizeof(_uint), &dwByte, nullptr);
	char* szName = new char[iChannelNameLen + 1]; // +1 for null terminator
	ReadFile(hFileRead, szName, iChannelNameLen, &dwByte, nullptr);
	szName[iChannelNameLen] = '\0';

	strcpy_s(m_szName, szName);
//	cout << strChannelName << endl;
	delete[] szName;

	m_iBoneIndex = pModel->Get_BoneIndex(m_szName);
//	cout << m_szName << "   :     " << m_iBoneIndex << endl;

	_uint iNumScalingKeys{}, iNumRotationKeys{}, iNumPositionKeys{};
	ReadFile(hFileRead, &iNumScalingKeys, sizeof(_uint), &dwByte, nullptr);		// for Export 
	ReadFile(hFileRead, &iNumRotationKeys, sizeof(_uint), &dwByte, nullptr);		// for Export 
	ReadFile(hFileRead, &iNumPositionKeys, sizeof(_uint), &dwByte, nullptr);		// for Export 


	m_iNumKeyFrames = max(iNumScalingKeys, iNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, iNumPositionKeys);
//	cout << m_iNumKeyFrames << endl;
	_float3		vScale{};
	_float4		vRotation{};
	_float3		vPosition{};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame{};

		if (iNumScalingKeys > i)
		{
			ReadFile(hFileRead, &vScale, sizeof(_float3), &dwByte, nullptr);						// for Export 
			ReadFile(hFileRead, &KeyFrame.fTrackPosition, sizeof(_float), &dwByte, nullptr);		// for Export 

		}

		if (iNumRotationKeys > i)
		{
			ReadFile(hFileRead, &vRotation.x, sizeof(_float), &dwByte, nullptr);						// for Export 
			ReadFile(hFileRead, &vRotation.y, sizeof(_float), &dwByte, nullptr);						// for Export 
			ReadFile(hFileRead, &vRotation.z, sizeof(_float), &dwByte, nullptr);						// for Export 
			ReadFile(hFileRead, &vRotation.w, sizeof(_float), &dwByte, nullptr);						// for Export 
			ReadFile(hFileRead, &KeyFrame.fTrackPosition, sizeof(_float), &dwByte, nullptr);			// for Export 
			
		}

		if (iNumPositionKeys > i)
		{
			ReadFile(hFileRead, &vPosition, sizeof(_float3), &dwByte, nullptr);							// for Export 
			// cout << vPosition.x << "            " << vPosition.y << "                     " << vPosition.z << endl;
			ReadFile(hFileRead, &KeyFrame.fTrackPosition, sizeof(_float), &dwByte, nullptr);			// for Export 
//			cout << KeyFrame.fTrackPosition << endl;

		}
		
	
		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;
		//if(i < 4)
		//{
		//	cout << "vScale" << endl << "X : " << vScale.x << "Y : " << vScale.y << "Z : " << vScale.z << endl;
		//	cout << "vRotation" << endl << "X : " << vRotation.x << "Y : " << vRotation.y << "Z : " << vRotation.z << endl;
		//	cout << "vPosition" << endl << "X : " << vPosition.x << "Y : " << vPosition.y << "Z : " << vPosition.z << endl;
		//}


		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}
