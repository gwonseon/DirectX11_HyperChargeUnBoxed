#include "stdafx.h"
#include "..\Public\Effect.h"

CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CEffect::CEffect(const CEffect& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CEffect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);
        m_fPosition = pDesc->fPosition;
        m_fScale    = pDesc->fScale;
        m_eLevel    = pDesc->eLevel;
    }
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fPosition.x, m_fPosition.y, m_fPosition.z, 1.f));


    return S_OK;
}

void CEffect::Priority_Update(_float fTimeDelta)
{
}

void CEffect::Update(_float fTimeDelta)
{
}

void CEffect::Late_Update(_float fTimeDelta)
{
}

HRESULT CEffect::Render()
{
    return S_OK;
}

void CEffect::Free()
{
    __super::Free();
}
