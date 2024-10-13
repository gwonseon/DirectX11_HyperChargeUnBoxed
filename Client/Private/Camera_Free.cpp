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
    m_eLevelID = pDesc->eLevel;
    m_matPlayerWorld = pDesc->matPlayerWorld;
    m_fRotationPerSec =  pDesc->fRotationPerSec;
    m_vecTPSPos = pDesc->m_vecTPS_CamPos;
    m_vecFPSPos = pDesc->m_vecFPS_CamPos;
    m_iViewState = pDesc->iViewState;
    m_bMouseLock = false;


    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;


    m_pTransformCom->Rotation(0.f, 0.f, 0.f);
    return S_OK;
}

void CCamera_Free::Priority_Update(_float fTimeDelta)
{
    if(m_eLevelID == LEVEL_GAMEPLAY)
    {
        XMMATRIX matWorld = XMLoadFloat4x4(&*m_matPlayerWorld);

        if (*m_iViewState == PLAYER_TPS_VIEW) // 3인칭
        {
            _long MouseMoveY = { 0 };  _matrix RotationMatrix{};
            if (MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
            {
                if (m_fAngle_Y <= 4.f && m_fAngle_Y >= -4.f)
                    m_fAngle_Y += fTimeDelta * MouseMoveY * m_fMouseSensor * 2.f;
                if (m_fAngle_Y > 4.f)
                    m_fAngle_Y = 4.f;
                if (m_fAngle_Y < -4.f)
                    m_fAngle_Y = -4.f;
            }
            // 카메라 위치 조정
            XMVECTOR vCamPos = *m_vecTPSPos;
            vCamPos = XMVectorSetY(vCamPos, XMVectorGetY(vCamPos) + m_fAngle_Y);
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPos);
            // 바라보는 방향 조정
            vAt = *m_vecTPSPos + matWorld.r[2] * 5.f;
            m_pTransformCom->LookAt(vAt);
        }
        else if (*m_iViewState == PLAYER_FPS_VIEW)// 1인칭
        {
            _long MouseMoveY = { 0 };  _matrix RotationMatrix{};
            if (MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
            {
                if (m_fAngle_Y <= 4.f && m_fAngle_Y >= -4.f)
                    m_fAngle_Y += fTimeDelta * MouseMoveY * m_fMouseSensor * 2.f;
                if (m_fAngle_Y > 4.f)
                    m_fAngle_Y = 4.f;
                if (m_fAngle_Y < -4.f)
                    m_fAngle_Y = -4.f;
            }
            // 카메라 위치 조정
            XMVECTOR vCamPos = *m_vecFPSPos;
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPos);
            // 바라보는 방향 조정
            vAt = *m_vecFPSPos + matWorld.r[2] * 8.f;
            vAt = XMVectorSetY(vAt, XMVectorGetY(vAt) - m_fAngle_Y);
            m_pTransformCom->LookAt(vAt);
        }
    }

    if (m_eLevelID == LEVEL_IMGUI)
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


    __super::Priority_Update(fTimeDelta);
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
  /*      ClientToScreen(g_hWnd, &clientPos);
        SetCursorPos(clientPos.x + g_iWinSizeX * 0.5f, clientPos.y + g_iWinSizeY * 0.5f);
        ShowCursor(FALSE);*/
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
    default:
        break;
    }


}

void CCamera_Free::Late_Update(_float fTimeDelta)
{

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
