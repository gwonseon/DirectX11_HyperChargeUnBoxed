#include "stdafx.h"
#include "..\Public\Camera_Free.h"

#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype)
    : CCamera{ Prototype }
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
    CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);
    m_fMouseSensor = pDesc->fMouseSensor;
    m_fFovy = pDesc->fFovy;
    m_fAspect = pDesc->fAspect;
    m_fNearZ = pDesc->fNearZ;
    m_fFar = pDesc->fFar;
    m_eLevelID = pDesc->eLevel;                     // 현재 레벨(씬)
    m_matPlayerWorld = pDesc->matPlayerWorld;       // 플레이어의 월드 메트릭스
    m_fRotationPerSec =  pDesc->fRotationPerSec;    // 회전 속도
    m_vecTPSPos = pDesc->m_vecTPS_CamPos;           // 3인칭 카메라의 위차
    m_vecFPSPos = pDesc->m_vecFPS_CamPos;          // 1인칭 카메라의 위치
    m_iViewState = pDesc->iViewState;           // 인칭 변화
    m_iUpperMotion = pDesc->iUpperMotion;         // 상체 모션( 1인칭 일 때 특정 동작에서 카메라의 Dir를 고정시켜줘야함_)
    m_vecWeaponPos = pDesc->m_vecWeaponPos;     // 무기 위치
    m_vecWeaponDir = pDesc->m_vecWeaponDir;     // 무기 방향
    m_pShotNow = pDesc->bShotNow;   // 총 쏘는 타이밍
    m_pShotStart = pDesc->bShotStart; // 총 쏘는 시작 타이밍
    m_pWeaponState = pDesc->iWeaponState; // 어떤 총인지
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    
    m_bMouseLock = false;  // 마우스 멈춤
    m_vecPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    m_pTransformCom->Rotation(0.f, 0.f, 0.f);
    return S_OK;
}

void CCamera_Free::Priority_Update(_float fTimeDelta)
{
    m_vecPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    // 편집툴에서 카메라 조정
    if (m_eLevelID == LEVEL_IMGUI || m_eLevelID == LEVEL_NAVIGATION || m_eLevelID == LEVEL_MONSTERSPAWN)
    {
        if (m_bMouseLock == false)
        {
            _long   MouseMove = { 0 };
            _long   MouseMoveY = { 0 };
            if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
            {
                m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
            }
            MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y);
            if (MouseMoveY != 0 ) // y축 회전
            {
                m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMoveY * m_fMouseSensor);
            }

        }
    }

   if(m_eLevelID == LEVEL_GAMEPLAY)
        *m_pShotStart = false;

  
}

void CCamera_Free::Update(_float fTimeDelta)
{

    POINT clientPos{};
    switch (m_eLevelID)
    {
    case Client::LEVEL_STATIC:
        ShowCursor(TRUE);
        break;
    case Client::LEVEL_LOADING:
        ShowCursor(TRUE);
        break;
    case Client::LEVEL_LOGO:
        ShowCursor(TRUE);
        break;
    case Client::LEVEL_GAMEPLAY:
       ClientToScreen(g_hWnd, &clientPos);
        SetCursorPos(clientPos.x + g_iWinSizeX * 0.5f, clientPos.y + g_iWinSizeY * 0.5f);
        ShowCursor(FALSE);
        break;
    case Client::LEVEL_IMGUI:
        if (GetKeyState('S') & 0x8000)
        {
            m_pTransformCom->Go_Backward(fTimeDelta);
        }
        if (GetKeyState('W') & 0x8000)
        {
            m_pTransformCom->Go_Straight(fTimeDelta);
        }
        if (GetKeyState('A') & 0x8000)
        {
            m_pTransformCom->Go_Left(fTimeDelta);
        }
        if (GetKeyState('D') & 0x8000)
        {
            m_pTransformCom->Go_Right(fTimeDelta);
        }
        // 마우스 고정
        if (GetAsyncKeyState(VK_MBUTTON) & 0x0001)
        {
            if (m_bMouseLock == true)
                m_bMouseLock = false;
            else
                m_bMouseLock = true;

        }
        ShowCursor(TRUE);
        break;
    case Client::LEVEL_NAVIGATION:
        if (GetKeyState('S') & 0x8000)
        {
            m_pTransformCom->Go_Backward(fTimeDelta);
        }
        if (GetKeyState('W') & 0x8000)
        {
            m_pTransformCom->Go_Straight(fTimeDelta);
        }
        if (GetKeyState('A') & 0x8000)
        {
            m_pTransformCom->Go_Left(fTimeDelta);
        }
        if (GetKeyState('D') & 0x8000)
        {
            m_pTransformCom->Go_Right(fTimeDelta);
        }
        // 마우스 고정
        if (GetAsyncKeyState(VK_MBUTTON) & 0x0001)
        {
            if (m_bMouseLock == true)
                m_bMouseLock = false;
            else
                m_bMouseLock = true;

        }
        ShowCursor(TRUE);
        break;

    case Client::LEVEL_MONSTERSPAWN:
        if (GetKeyState('S') & 0x8000)
        {
            m_pTransformCom->Go_Backward(fTimeDelta);
        }
        if (GetKeyState('W') & 0x8000)
        {
            m_pTransformCom->Go_Straight(fTimeDelta);
        }
        if (GetKeyState('A') & 0x8000)
        {
            m_pTransformCom->Go_Left(fTimeDelta);
        }
        if (GetKeyState('D') & 0x8000)
        {
            m_pTransformCom->Go_Right(fTimeDelta);
        }
        // 마우스 고정
        if (GetAsyncKeyState(VK_MBUTTON) & 0x0001)
        {
            if (m_bMouseLock == true)
                m_bMouseLock = false;
            else
                m_bMouseLock = true;

        }
        ShowCursor(TRUE);
        break;

    default:
        break;
    }

 
}

void CCamera_Free::Late_Update(_float fTimeDelta)
{
    if (m_eLevelID == LEVEL_GAMEPLAY)
    {
        XMMATRIX matWorld = XMLoadFloat4x4(m_matPlayerWorld); // 플레이어 월드 매트릭스
        // 카메라 회전
        _long MouseMoveY = { 0 };  _matrix RotationMatrix{};
        if (MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
        {
            if (*m_iViewState == PLAYER_TPS_VIEW)
            {
                if (m_fAngle_Y <= 8.f && m_fAngle_Y >= -8.f)
                    m_fAngle_Y += fTimeDelta * MouseMoveY * m_fMouseSensor * 4.f;
                if (m_fAngle_Y > 4.f)
                    m_fAngle_Y = 4.f;
                if (m_fAngle_Y < -6.f)
                    m_fAngle_Y = -6.f;
            }
            else
            {
                if (m_fAngle_Y <= 4.f && m_fAngle_Y >= -4.f)
                    m_fAngle_Y += fTimeDelta * MouseMoveY * m_fMouseSensor * 4.f;
                if (m_fAngle_Y > 2.f)
                    m_fAngle_Y = 2.f;
                if (m_fAngle_Y < -4.f)
                    m_fAngle_Y = -4.f;
            }
        }
        // At 설정
        if (*m_iViewState == PLAYER_TPS_VIEW) // 3인칭
        {
            // 카메라 위치 조정
            XMVECTOR vCamPos = *m_vecTPSPos;

            vCamPos = XMVectorSetY(vCamPos, XMVectorGetY(vCamPos) + m_fAngle_Y);
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPos);
#pragma region 카메라쉐이킹
            if (*m_pShotStart == true)
            {
                m_fStore_RandomValue = (float(rand() % 15) * 0.01f); // 반동 값 계산용 , 라이플 ( 총 마다 다르게 설정해야 할 듯한디 나중에 하자)
                m_fAngle_Y -= m_fStore_RandomValue;     // 앵글각도에서 빼주기'

            }

#pragma endregion 카메라쉐이킹
            // 바라보는 방향 조정
            vAt = *m_vecTPSPos + matWorld.r[2] * 7.f;
            if (m_fAngle_Y > 0)
            {
                vAt = vAt + matWorld.r[0] * (m_fAngle_Y * 0.3f);
                vAt = XMVectorSetY(vAt, XMVectorGetY(vAt) - m_fAngle_Y / 2);
            }
            else
            {
                vAt = XMVectorSetY(vAt, XMVectorGetY(vAt) - m_fAngle_Y);
            }
            m_pTransformCom->LookAt(vAt);


        }
        else if (*m_iViewState == PLAYER_FPS_VIEW)// 1인칭
        {
            XMVECTOR vCamPos = *m_vecFPSPos;
            m_vecStore_Dir = *m_vecWeaponDir;
            vAt = *m_vecWeaponPos + XMVector3Normalize(*m_vecWeaponDir) * 100.f;
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPos);
            m_pTransformCom->LookAt(vAt);


        }
    }

    __super::Priority_Update(fTimeDelta);
}

HRESULT CCamera_Free::Render()
{
    return S_OK;
}



CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCamera_Free");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
    CCamera_Free* pInstance = new CCamera_Free(*this);

    if (FAILED(pInstance->Initialize(pArg)))
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
