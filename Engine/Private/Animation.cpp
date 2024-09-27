#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{

}

//HRESULT CAnimation::Initialize(CModel* pModel, const aiAnimation* pAIAnimation)
//{
//	strcpy_s(m_szName, pAIAnimation->mName.data);
//
//	m_fDuration = pAIAnimation->mDuration;
//	m_fTickPerSecond = pAIAnimation->mTicksPerSecond;
//
//	/* 이 애님을 표현하기위해 사용해야하는 뼈의 갯수. */
//	m_iNumChannels = pAIAnimation->mNumChannels;
//
//	for (size_t i = 0; i < m_iNumChannels; i++)
//	{
//		/* 각각의 뼈 정보(행렬을 구성하기위한 정보)를 저장한다 .*/
//		CChannel* pChannel = CChannel::Create(pModel, pAIAnimation->mChannels[i]);
//		if (nullptr == pChannel)
//			return E_FAIL;
//
//		m_Channels.push_back(pChannel);
//	}
//	return S_OK;
//}

_bool CAnimation::Update_TransformationMatrix(const vector<class CBone*>& Bones, _float* pCurrentPosition, _bool isLoop, _float fTimeDelta)
{
	*pCurrentPosition += m_fTickPerSecond * fTimeDelta;

	if (*pCurrentPosition >= m_fDuration &&
		true == isLoop)
	{
		*pCurrentPosition = 0.f;
	}

	if (*pCurrentPosition >= m_fDuration &&
		false == isLoop)
	{
		return true;
	}

	/* 이 애니메이션이 사용하는 모든 뼈의 상태를 시간에 맞게 변경하낟.*/
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		/* 채널이 가지고 있는 재생위치당 상태(KeyFrame)를 활용하여 현재 재생위치에 맞는 뼈(채널)의 상태를 만들어준다.  */
		/* 상태행렬을 현재 채널과 이름이 같은 뼈에게 전달하여 뼈의 상태를 갱신할 수 있도록 하낟. */
		m_Channels[i]->Update_TransformationMatrix(Bones, *pCurrentPosition);
	}

	return false;
}

//CAnimation* CAnimation::Create(CModel* pModel, const aiAnimation* pAIAnimation)
//{
//	CAnimation* pInstance = new CAnimation();
//
//	if (FAILED(pInstance->Initialize(pModel, pAIAnimation)))
//	{
//		MSG_BOX("Failed to Created : CAnimation");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);
	m_Channels.clear();
}

