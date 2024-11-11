#include "stdafx.h"
#include "..\Public\Tracker.h"

#include "GameInstance.h"

CTracker::CTracker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTracker::CTracker(const CTracker& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CTracker::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CTracker::Initialize(void* pArg)
{
    TRACKER_DESC* pTracker = static_cast<TRACKER_DESC*>(pArg);
    m_eLevel = pTracker->eID;
    m_iRound = pTracker->iRound;
    m_pPlayer = pTracker->pPlayer;
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pTracker->fPosition.x, pTracker->fPosition.y, pTracker->fPosition.z, 1.f));
    m_pTransformCom->Set_Scaling(pTracker->fScale.x, pTracker->fScale.y, pTracker->fScale.z);
    m_fTimer = 100.f;
    m_bPickUp_Player = m_pPlayer->Get_Visible_Tracker();
 
    


    return S_OK;
}

void CTracker::Priority_Update(_float fTimeDelta)
{
    m_vecPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    if (m_bMissile_Fall == true)
    {   // 미사일 떨어지면 플레이어 손에서 놓기
        *m_bPickUp_Player = false;
        if(*m_pPlayer->Get_WeaponState() == CPlayer::TRACKER)
        {
            m_pPlayer->Set_Explosion(true);
        }
        m_bMissile_Fall = false;
    }

    _vector vecPlayerPos = m_pPlayer->Get_Position();
    if (*m_bPickUp_Player == true) // 플레이어가 들었을 때 위치값 변경
    {
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPlayerPos); // 드는 순간 플레이어 위치로 변경
    }
    else  // 플레이어가 내려놓았을 때(혹은 처음 시작할 때 ) 떨어지기
    {
      if(*m_iRound == 1)
      {
          if (XMVectorGetY(m_vecPosition) > 0.f)
          {
              m_vecPosition = XMVectorSetY(m_vecPosition, XMVectorGetY(m_vecPosition) - (fTimeDelta * m_fFallingSpeed));
              m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);
              m_fFallingSpeed += 1.f;
          }
          else
          {
              m_fFallingSpeed = 10.f;
              m_vecPosition = XMVectorSetY(m_vecPosition, 0.f);
              m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);
          }
      }
    }
    if (m_pTransformCom->Cal_Distance_vec(vecPlayerPos, m_vecPosition) <= 80.f) // 플레이어와 상호작용
    {
        // 들기 성공
        if (m_fPickUpTimer >= 1.f)
        {
            m_bInteraction = false;
            m_fPickUpTimer = 0.f;
            m_pPlayer->PickUp_Battery(CPlayer::TRACKER); // 12번, 같은 함수라 재사용함, 플레이어가 드는 무기 Tracker로 변경
            *m_bPickUp_Player = true; // 들었다!! 
        }

        if(*m_bPickUp_Player == false)
        {
            // 근처에 있을 때 상호작용
            if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_E))
            {
                m_bInteraction = true;
                m_fPickUpTimer += fTimeDelta;
            }
            else
            {
                m_bInteraction = false;
                m_fPickUpTimer = 0.f;
            }
        }
    }
    else
        m_bInteraction = false;

    if (*m_iRound == 1 && m_bOnce == false) // 미사일 라운드 시작
    {
        m_bOnce = true;
        m_eTrackerState = TRACKER_TURN_OFF;
        m_fMissileTimer = 0.f;
    }
    else if (*m_iRound != 1)    // 미사일 라운드가 아닐 때 안그리기 & 작동 안함
    {
        m_eTrackerState = TRACKER_IDLE;
    }

    if (m_eTrackerState == TRACKER_TURN_OFF) // 미사일 떨어지고 Tracker 새 위치로 낙하
    {

        // 낙하 시간 기다려주기 위함
        m_fMissileTimer += fTimeDelta;
        if (m_fMissileTimer >= 5.f)
        {
            m_fMissileTimer = 0.f;
            m_eTrackerState = TRACKER_TURN_ON;
        }
    }
}

void CTracker::Update(_float fTimeDelta)
{
    // 트래커 작동 시작
    if (TRACKER_TURN_ON == m_eTrackerState)
    {
        m_fMissileTimer += fTimeDelta;
        if (m_fMissileTimer >= 10.f)
        {
            m_bStart_Shot = true;
            m_fMissileTimer = 0.f;
            m_eTrackerState = TRACKER_TURN_OFF;

        }
    }

    // 전체 시간 줄어들기
    if (TRACKER_TURN_ON == m_eTrackerState || TRACKER_TURN_OFF == m_eTrackerState)
    {
        m_fTimer -= fTimeDelta;
    }

}

void CTracker::Late_Update(_float fTimeDelta)
{
    // 미사일 라운드일 때만 그리기, 단, 플레이어가 들고 있을 땐 안그림
    if(*m_iRound == 1)
    {
        if(*m_bPickUp_Player == false)
        {
            if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
                return;
        }
    }
}

HRESULT CTracker::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CTracker::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    const _wstring Model_Component = TEXT("Prototype_Component_Model_Bullet");
    const _wstring Model_Component_Result = Model_Component + to_wstring(5);
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;


    return S_OK;
}

HRESULT CTracker::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CTracker* CTracker::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTracker* pInstance = new CTracker(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTracker");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CTracker::Clone(void* pArg)
{
    CTracker* pInstance = new CTracker(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CTracker");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTracker::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
