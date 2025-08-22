#include "stdafx.h"
#include "..\Public\Camera_Free.h"

#include "GameInstance.h"

namespace 
{
constexpr float kMouseSens = 0.1f; // 감도

constexpr float kTPSMinPitch = -6.f; // 3인칭 Y축 최소 범위
constexpr float kTPSMaxPitch = 4.f; // 3인칭 Y축 최대 범위

constexpr float kFPSMinPitch = -4.f; // 1인칭 Y축 최대 범위
constexpr float kFPSMaxPitch = 2.f;  // 1인칭 Y축 최소 범위
}

CCamera_Free::CCamera_Free(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
	: CCamera{pDevice,pContext}
{}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype)
	: CCamera{Prototype}
{}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);
	m_matPlayerWorld = pDesc->matPlayerWorld;       // 플레이어의 월드 메트릭스
	m_vecTPSPos = pDesc->m_vecTPS_CamPos;           // 3인칭 카메라의 위차
	m_vecFPSPos = pDesc->m_vecFPS_CamPos;			// 1인칭 카메라의 위치
	m_iViewState = pDesc->iViewState;				// 인칭 변화
	m_vecWeaponPos = pDesc->m_vecWeaponPos;     // 무기 위치
	m_vecWeaponDir = pDesc->m_vecWeaponDir;     // 무기 방향
	m_pShotStart = pDesc->bShotStart; // 총 쏘는 시작 타이밍

	if(FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
	m_pGameInstance->Set_CameraFar(pDesc->fFar);
	return S_OK;
}

void CCamera_Free::Priority_Update(_float fTimeDelta)
{
	*m_pShotStart = false;
}

void CCamera_Free::Update(_float fTimeDelta)
{
	POINT clientPos{};
	ClientToScreen(g_hWnd,&clientPos);
	ShowCursor(false);
	SetCursorPos(clientPos.x + g_iWinSizeX * 0.5f,clientPos.y + g_iWinSizeY * 0.5f);
	InGame_Camera(fTimeDelta);

	m_vecPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	__super::Priority_Update(fTimeDelta);
}

void CCamera_Free::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

void CCamera_Free::InGame_Camera(_float fTimeDelta)
{
	XMMATRIX matWorld = XMLoadFloat4x4(m_matPlayerWorld); // 플레이어 월드 매트릭스
	// 카메라 회전
	_long MouseMoveY = {0};  _matrix RotationMatrix{};
	if(MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
	{
		if(*m_iViewState == PLAYER_TPS_VIEW)
		{
			if(m_fAngle_Y <= (2 * kTPSMaxPitch) && m_fAngle_Y >= -(2 * kTPSMaxPitch))
				m_fAngle_Y += fTimeDelta * MouseMoveY * kMouseSens * 4.f;
			if(m_fAngle_Y > kTPSMaxPitch)
				m_fAngle_Y = kTPSMaxPitch ;
			if(m_fAngle_Y < kTPSMinPitch)
				m_fAngle_Y = kTPSMinPitch;
		} else
		{
			if(m_fAngle_Y <= -kFPSMinPitch && m_fAngle_Y >= kFPSMinPitch)
				m_fAngle_Y += fTimeDelta * MouseMoveY * kMouseSens * 4.f;
			if(m_fAngle_Y > kFPSMaxPitch)
				m_fAngle_Y = kFPSMaxPitch ;
			if(m_fAngle_Y < kFPSMinPitch)
				m_fAngle_Y = kFPSMinPitch ;
		}
	}

	// At 설정
	if(*m_iViewState == PLAYER_TPS_VIEW) // 3인칭
	{
		// 카메라 위치 조정
		XMVECTOR vCamPos = *m_vecTPSPos;

		vCamPos = XMVectorSetY(vCamPos,XMVectorGetY(vCamPos) + m_fAngle_Y);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION,vCamPos);
		#pragma region 카메라쉐이킹
		if(*m_pShotStart == true)
		{
			m_fStore_RandomValue = (float(rand() % 15) * 0.01f); // 반동 값 계산용
			m_fAngle_Y -= m_fStore_RandomValue;     // 앵글각도에서 빼주기'
		}
		#pragma endregion 카메라쉐이킹

		// 바라보는 방향 조정
		m_vecAt = *m_vecTPSPos + matWorld.r[2] * 7.f;
		if(m_fAngle_Y > 0)
		{
			m_vecAt = m_vecAt + matWorld.r[0] * (m_fAngle_Y * 0.3f);
			m_vecAt = XMVectorSetY(m_vecAt,XMVectorGetY(m_vecAt) - m_fAngle_Y / 2);
		} 
		else
		{
			m_vecAt = XMVectorSetY(m_vecAt,XMVectorGetY(m_vecAt) - m_fAngle_Y);
		}
		m_pTransformCom->LookAt(m_vecAt);
	} 
	else if(*m_iViewState == PLAYER_FPS_VIEW)// 1인칭
	{
		XMVECTOR vCamPos = *m_vecFPSPos;
		m_vecStore_Dir = *m_vecWeaponDir;
		m_vecAt = *m_vecWeaponPos + XMVector3Normalize(*m_vecWeaponDir) * 100.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION,vCamPos);
		m_pTransformCom->LookAt(m_vecAt);
	}
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(pDevice,pContext);
	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);
	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();

}