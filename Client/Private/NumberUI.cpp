#include "stdafx.h"
#include "..\Public\NumberUI.h"

#include "GameInstance.h"
CNumberUI::CNumberUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext } 
{
}

CNumberUI::CNumberUI(const CNumberUI& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CNumberUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNumberUI::Initialize(void* pArg)
{
    NUMBERUI_DESC* pDesc = (NUMBERUI_DESC*)pArg;
    m_eLevel = pDesc->eLevel;
    m_iDigit = pDesc->eDigit;
    m_pPlayer = pDesc->pPlayer;
    m_iUsage = pDesc->eTypeUsage;
    m_fDepth = pDesc->fDepth;
    m_fScaleX = pDesc->fSizeX;
    m_fScaleY = pDesc->fSizeY;
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Add_Components(pDesc->iData)))
        return E_FAIL;
    vFirstPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
   
    return S_OK;
}

void CNumberUI::Priority_Update(_float fTimeDelta)
{
   
    switch (m_iUsage)
    {
        case TYPE_COIN:
        {
            m_fDistance = 13.f; // 글자 사이의 거리
            Coin(fTimeDelta);
            break;
        }
        case TYPE_BULLET:
        {
   
            m_fDistance = 16.f; // 글자 사이의 거리
            Bullet(fTimeDelta);
            if (*m_pPlayer->Get_WeaponState() == CPlayer::WEAPON_KATANA)
                m_bDraw = false;

            break;
        }
        case TYPE_FULLBULLET:
        {
         
            m_fDistance = 15.f; // 글자 사이의 거리
            FullBullet(fTimeDelta);
            if (*m_pPlayer->Get_WeaponState() == CPlayer::WEAPON_KATANA)
                m_bDraw = false;
 
            break;
        }
        
    default:
        break;
    }
   
}

void CNumberUI::Update(_float fTimeDelta)
{

    // 자리수에 따른 위치와 값 업데이트
    switch (m_iDigit)
    {
    case Client::CNumberUI::ONE_DIGIT:
    {
        vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        vPos = XMVectorSetX(vPos, XMVectorGetX(vFirstPos) + (m_iLastDigit) *m_fDistance);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
        m_iNumber = m_iGetNum % 10;
        
        break;
    }
    case Client::CNumberUI::TEN_DIGIT:
        vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        vPos = XMVectorSetX(vPos, XMVectorGetX(vFirstPos) + (m_iLastDigit - 1) * m_fDistance);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
        m_iNumber = m_iGetNum / 10;
        m_iNumber %= 10;
        break;
    case Client::CNumberUI::HUNDREDS_DIGIT:
        vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        vPos = XMVectorSetX(vPos, XMVectorGetX(vFirstPos) + (m_iLastDigit - 2) * m_fDistance);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
        m_iNumber = m_iGetNum / 100;
        m_iNumber %= 10;
        break;
    case Client::CNumberUI::THOUSANDS_DIGIT:
        vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        vPos = XMVectorSetX(vPos, XMVectorGetX(vFirstPos) + (m_iLastDigit - 3) * m_fDistance);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
        m_iNumber = m_iGetNum / 1000;
        m_iNumber %= 10;
        break;
    case Client::CNumberUI::TENS_OF_THOUSANDS_DIGIT:
        vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        vPos = XMVectorSetX(vPos, XMVectorGetX(vFirstPos) + (m_iLastDigit - 4) * m_fDistance);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
        m_iNumber = m_iGetNum / 10000;
        m_iNumber %= 10;
        break;
    case Client::CNumberUI::HUNDREDS_OF_THOUSANDS_DIGIT:
        vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        vPos = XMVectorSetX(vPos, XMVectorGetX(vFirstPos) + (m_iLastDigit - 5) * m_fDistance);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
        m_iNumber = m_iGetNum / 100000;
        m_iNumber %= 10;
        break;
    default:
        break;
    }

}

void CNumberUI::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
        return;
}

HRESULT CNumberUI::Render()
{
    if(m_bDraw == true)
    {
        if (FAILED(Bind_ShaderResources()))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;
    }

    return S_OK;
}

void CNumberUI::Coin(_float fTimeDelta)
{
    m_iGetNum = m_pPlayer->Get_Coin();
    _uint iCoin_For_Digit = m_iGetNum;
    _uint iDigit_Check = 0;
    if (iCoin_For_Digit == 0) // 0일 때는 1의 자리로 판단
        iDigit_Check = 1;
    while (iCoin_For_Digit > 0)
    {
        iCoin_For_Digit /= 10;
        iDigit_Check += 1;
    }

    m_iLastDigit = iDigit_Check - 1; // 가장 큰 자리수
    // 자리수에 따라서 그릴지 말지 결정
    if (m_iDigit <= m_iLastDigit)
        m_bDraw = true;
    else
        m_bDraw = false;
}

void CNumberUI::Bullet(_float fTimeDelta)
{
    _uint* pBullet =  m_pPlayer->Get_CurrentBullet();
    // m_pPlayer->Get_FullBullet();
    m_iGetNum = *pBullet;
    _uint Bullet_For_Digit = *pBullet;
    _uint iDigit_Check = 0;
    if (Bullet_For_Digit == 0) // 0일 때는 1의 자리로 판단
        iDigit_Check = 1;
    while (Bullet_For_Digit > 0)
    {
        Bullet_For_Digit /= 10;
        iDigit_Check += 1;
    }

    m_iLastDigit = iDigit_Check - 1; // 가장 큰 자리수
    // 자리수에 따라서 그릴지 말지 결정
    if (m_iDigit <= m_iLastDigit)
        m_bDraw = true;
    else
        m_bDraw = false;
    
    if (*m_pPlayer->Get_ShotNow() == true)
    {
        fShot_Size = 5.f;
    }
    else
        fShot_Size = 0.f;
    m_pTransformCom->Set_Scaling(m_fScaleX + fShot_Size, m_fScaleY+ fShot_Size, m_fDepth);

}

void CNumberUI::FullBullet(_float fTimeDelta)
{
    _uint* pBullet = m_pPlayer->Get_FullBullet();
    // m_pPlayer->Get_FullBullet();
    m_iGetNum = *pBullet;
    _uint Bullet_For_Digit = *pBullet;
    _uint iDigit_Check = 0;
    if (Bullet_For_Digit == 0) // 0일 때는 1의 자리로 판단
        iDigit_Check = 1;
    while (Bullet_For_Digit > 0)
    {
        Bullet_For_Digit /= 10;
        iDigit_Check += 1;
    }

    m_iLastDigit = iDigit_Check - 1; // 가장 큰 자리수
    // 자리수에 따라서 그릴지 말지 결정
    if (m_iDigit <= m_iLastDigit)
        m_bDraw = true;
    else
        m_bDraw = false;
}

HRESULT CNumberUI::Add_Components(_int iNum)
{
    if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_Number"),
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

HRESULT CNumberUI::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iNumber)))
        return E_FAIL;

    return S_OK;

}

CNumberUI* CNumberUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNumberUI* pInstance = new CNumberUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CNumberUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CNumberUI::Clone(void* pArg)
{
    CNumberUI* pInstance = new CNumberUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CNumberUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNumberUI::Free()
{
    __super::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
