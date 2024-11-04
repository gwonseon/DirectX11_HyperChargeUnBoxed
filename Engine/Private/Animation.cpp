#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{

}

CAnimation::CAnimation(const CAnimation& Prototype)
	: m_fDuration{ Prototype.m_fDuration }
	, m_fTickPerSecond{ Prototype.m_fTickPerSecond }
	, m_fCurrentPosition{ Prototype.m_fCurrentPosition }
	, m_iNumChannels{ Prototype.m_iNumChannels }
	, m_Channels{ Prototype.m_Channels }
	, m_iChannelKeyFrameIndices{ Prototype.m_iChannelKeyFrameIndices }
{

	strcpy_s(m_szName, Prototype.m_szName);

	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);

}
 
_bool CAnimation::Update_TransformationMatrix(const vector<class CBone*>& Bones, _bool isLoop, _float fTimeDelta)
{
	//if (m_fCurrentPosition <= 0.f && bChangeAnim == true)
	//{
	//	m_fLerptime += fTimeDelta;
	//	for (size_t i = 0; i < m_iNumChannels; i++)
	//	{
	//		m_Channels[i]->Update_LinearInterPolation2(Bones, m_fLerptime, bChangeAnim);
	//	}
	//} 
	//else
	//{
	//	m_fLerptime = 0.f;
	//	
	//}

	if (m_fCurrentPosition == 0)
	{
		m_vecName.resize(m_iNumChannels);
		LastKeyFrame = nullptr; // LastKeyFrame을 nullptr로 초기화
	}

	m_fCurrentPosition += m_fTickPerSecond * fTimeDelta;
	if (m_fCurrentPosition >= m_fDuration &&
		true == isLoop)
	{
		m_fCurrentPosition = 0.f;
	}

	if (m_fCurrentPosition >= (m_fDuration) &&
		false == isLoop)
	{
		return true;
	}

	/* 이 애니메이션이 사용하는 모든 뼈의 상태를 시간에 맞게 변경하낟.*/
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		/* 채널이 가지고 있는 재생위치당 상태(KeyFrame)를 활용하여 현재 재생위치에 맞는 뼈(채널)의 상태를 만들어준다.  */
		/* 상태행렬을 현재 채널과 이름이 같은 뼈에게 전달하여 뼈의 상태를 갱신할 수 있도록 하낟. */

		m_Channels[i]->Update_TransformationMatrix(Bones, &m_iChannelKeyFrameIndices[i], m_fCurrentPosition);
		LastKeyFrame = &m_Channels[i]->Get_LastKeyFrame();
		m_vecName[i] = m_Channels[i]->Get_strName();
	}

	return false;
}

_bool CAnimation::Update_TransformationMatrix_Player(const vector<class CBone*>& Bones, _bool isLoop, _float fTimeDelta, _bool bUpper, _uint iUpperMotion, _bool& bShot )
{
	if(bUpper == true)
	{
		if (m_fCurrentPosition_UpperBody == 0)
		{
			m_vecName_UpperBody.resize(m_iNumChannels);
			LastKeyFrame = nullptr; // LastKeyFrame을 nullptr로 초기화
		}
		if(iUpperMotion != 0)
		{
			m_fCurrentPosition_UpperBody += m_fTickPerSecond * fTimeDelta;
			
		}


		if (m_fCurrentPosition_UpperBody >= m_fDuration &&
			true == isLoop)
		{
			bShot = false;
			m_fCurrentPosition_UpperBody = 0.f;
		}

		if (m_fCurrentPosition_UpperBody >= (m_fDuration) &&
			false == isLoop)
		{
			return true;
		}
	}

	else
	{
		if (m_fCurrentPosition_LowerBody == 0)
		{

			m_vecName_LowerBody.resize(m_iNumChannels);
			LastKeyFrame = nullptr; // LastKeyFrame을 nullptr로 초기화
		}

		m_fCurrentPosition_LowerBody += m_fTickPerSecond * fTimeDelta;
		if (m_fCurrentPosition_LowerBody >= m_fDuration &&
			true == isLoop)
		{
			m_fCurrentPosition_LowerBody = 0.f;
		}

		if (m_fCurrentPosition_LowerBody >= (m_fDuration) &&
			false == isLoop)
		{
			return true;
		}
	}
	
	/* 이 애니메이션이 사용하는 모든 뼈의 상태를 시간에 맞게 변경하낟.*/
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		/* 채널이 가지고 있는 재생위치당 상태(KeyFrame)를 활용하여 현재 재생위치에 맞는 뼈(채널)의 상태를 만들어준다.  */
		/* 상태행렬을 현재 채널과 이름이 같은 뼈에게 전달하여 뼈의 상태를 갱신할 수 있도록 하낟. */
		if(bUpper == true) // 상체일 때
		{
			m_Channels[i]->Update_TransformationMatrix_UpperBody(Bones, &m_iChannelKeyFrameIndices[i], m_fCurrentPosition_UpperBody);
			LastKeyFrame_UpperBody = &m_Channels[i]->Get_LastKeyFrame();
			m_vecName_UpperBody[i] = m_Channels[i]->Get_strName();

		}
		else	// 하체 일 때
		{
			m_Channels[i]->Update_TransformationMatrix_LowerBody(Bones, &m_iChannelKeyFrameIndices[i], m_fCurrentPosition_LowerBody);
			LastKeyFrame_LowerBody = &m_Channels[i]->Get_LastKeyFrame();
			m_vecName_LowerBody[i] = m_Channels[i]->Get_strName();
			
			
		}
	}

	return false;
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);
	m_Channels.clear();
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::CurrentPosition_Init(const vector<class CBone*>& Bones)
{
	m_fCurrentPosition = 0.f;
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_iChannelKeyFrameIndices[i] = 0;
	}	

}

void CAnimation::CurrentPosition_UpperBody_Init(const vector<class CBone*>& Bones)
{
	m_fCurrentPosition_UpperBody = 0.f;
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_iChannelKeyFrameIndices[i] = 0;
	}

}

void CAnimation::CurrentPosition_LowerBody_Init(const vector<class CBone*>& Bones)
{
	m_fCurrentPosition_LowerBody = 0.f;
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_iChannelKeyFrameIndices[i] = 0;
	}

}

_bool CAnimation::Update_LinearInterPolation(KEYFRAME* _PrevKeyFrame, const vector<class CBone*>& Bones, vector<string> szName, _float fTimeDelta)
{
	
	m_bLinearInterpolation = false;
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_bLinearInterpolation = m_Channels[i]->Update_LinearInterPolation( Bones, szName, fTimeDelta);
	}
	if (m_bLinearInterpolation == true)
	{
		return true;
	}

	return false;
}

_bool CAnimation::Update_LinearInterPolation_Player(KEYFRAME* _PrevKeyFrame, const vector<class CBone*>& Bones, vector<string> szName, _float fTimeDelta, _bool bUpper)
{
	m_bLinearInterpolation_UpperBody = false;
	m_bLinearInterpolation_LowerBody = false;
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		if(bUpper == true)
			m_bLinearInterpolation_UpperBody = m_Channels[i]->Update_LinearInterPolation_UpperBody(Bones, szName, fTimeDelta);
		else
			m_bLinearInterpolation_LowerBody = m_Channels[i]->Update_LinearInterPolation_LowerBody(Bones, szName, fTimeDelta);
	}
	if (m_bLinearInterpolation_LowerBody == true && bUpper == false)
	{
		return true;
	}
	if(m_bLinearInterpolation_UpperBody == true && bUpper == true)
	{
		return true;
	}
	return false;
}


CAnimation* CAnimation::Create(CModel* pModel, HANDLE hFileRead)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pModel, hFileRead)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CAnimation::Initialize(CModel* pModel, HANDLE hFileRead)
{

	_uint iAnimationNameLen = 0;
	ReadFile(hFileRead, &iAnimationNameLen, sizeof(_uint), &dwByte, nullptr);
	char* m_szName = new char[iAnimationNameLen + 1]; // +1 for null terminator
	ReadFile(hFileRead, m_szName, iAnimationNameLen * sizeof(_char), &dwByte, nullptr);
	m_szName[iAnimationNameLen] = '\0';
	string strAnimationName(m_szName);
	 cout << strAnimationName << endl;
	delete[] m_szName;

	ReadFile(hFileRead, &m_fDuration, sizeof(_float), &dwByte, nullptr);			// for Export 
//	cout << m_fDuration << endl;
	ReadFile(hFileRead, &m_fTickPerSecond, sizeof(_float), &dwByte, nullptr);		// for Export 


	/* 이 애님을 표현하기위해 사용해야하는 뼈의 갯수. */
	ReadFile(hFileRead, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);		// for Export 
	m_iChannelKeyFrameIndices.resize(m_iNumChannels);

//	cout << m_iNumChannels << endl;
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		/* 각각의 뼈 정보(행렬을 구성하기위한 정보)를 저장한다 .*/
		CChannel* pChannel = CChannel::Create(pModel, hFileRead);
		if (nullptr == pChannel)
			return E_FAIL;
		
		m_Channels.push_back(pChannel);
	}
	return S_OK;
}

