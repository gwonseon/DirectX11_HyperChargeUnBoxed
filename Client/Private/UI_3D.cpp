#include "stdafx.h"
#include "..\Public\UI_3D.h"

#include "GameInstance.h"

CUI_3D::CUI_3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CUI_3D::CUI_3D(const CUI_3D& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CUI_3D::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_3D::Initialize(void* pArg)
{
    UIOBJ_DESC* pDesc = (UIOBJ_DESC*)pArg;
    m_eLevel = pDesc->m_eLevel;
    m_pCamera = pDesc->pCamera;
    m_pPlayer= pDesc->pPlayer;
    m_pMissile_Truck = pDesc->m_pMissile_Truck;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pTracker = m_pMissile_Truck->Get_Tracker();
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f));
    m_pTransformCom->Set_Scaling(pDesc->fScale.x, pDesc->fScale.y, pDesc->fScale.z);
 
    return S_OK;
}

void CUI_3D::Priority_Update(_float fTimeDelta)
{

    m_vecPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _vector vCamPos{};
    // 위치 정해주기
    switch (m_eUIType)
    {
    case Client::CUI_3D::UI_NUCLEAR:
    {
        _vector vecTrackerPos = m_pTracker->Get_Pos();// Tracker 위치 받아서
        m_vecPosition = vecTrackerPos;
        m_vecPosition = XMVectorSetY(m_vecPosition, XMVectorGetY(m_vecPosition) + 5.f); // Tracker 위쪽에 배치
        vCamPos =*m_pCamera->Get_Camera_Pos();
        m_fDistance = m_pTransformCom->Cal_Distance_vec(m_vecPosition, vCamPos);
        break;
    }
    case Client::CUI_3D::UI_HP:
        break;
    case Client::CUI_3D::UI_OBJ_END:
        break;
    default:
        break;
    }

    // 위치 지정 및 카메라 바라보기
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);
    m_pTransformCom->LookAt(vCamPos); // 카메라 바라보기
}

void CUI_3D::Update(_float fTimeDelta)
{
    switch (m_eUIType)
    {
    case Client::CUI_3D::UI_NUCLEAR:
        Nuclear(fTimeDelta);
        break;
    case Client::CUI_3D::UI_HP:
        break;
    case Client::CUI_3D::UI_OBJ_END:
        break;
    default:
        break;
    }
}

void CUI_3D::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
        return;
}

HRESULT CUI_3D::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;
    switch (m_eUIType)
    {
    case Client::CUI_3D::UI_NUCLEAR:
        if (FAILED(m_pShaderCom->Begin(7)))
            return E_FAIL;
        break;
    case Client::CUI_3D::UI_HP:
        break;
    case Client::CUI_3D::UI_OBJ_END:
        break;
    default:
        break;
    }


    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_3D::Add_Components()
{
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_Nuclear"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_3D::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    return S_OK;
}

void CUI_3D::Nuclear(_float fTimeDelta)
{
        m_fScale = 2.5f * (m_fDistance / 16000.f);
        if (m_fScale < 2.5f)
            m_fScale = 2.5f;
        if (m_fScale > 5.5f)
            m_fScale = 5.5f;
    m_pTransformCom->Set_Scaling(m_fScale, m_fScale, m_fScale);
}

CUI_3D* CUI_3D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_3D* pInstance = new CUI_3D(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CUI_3D");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_3D::Clone(void* pArg)
{
    CUI_3D* pInstance = new CUI_3D(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CUI_3D");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_3D::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
