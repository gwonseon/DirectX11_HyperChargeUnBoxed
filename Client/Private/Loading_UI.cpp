#include "stdafx.h"
#include "..\Public\Loading_UI.h"

#include "GameInstance.h"
CLoading_UI::CLoading_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CLoading_UI::CLoading_UI(const CLoading_UI& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CLoading_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLoading_UI::Initialize(void* pArg)
{
    m_fIndex.y = 0.f;
    m_fIndex.x = 0.f;

    LOADINGUI_DESC		Desc{};

    LOADINGUI_DESC* pDesc = (LOADINGUI_DESC*)pArg;

    Desc.fX = pDesc->fX;
    Desc.fY = pDesc->fY;
    Desc.fSizeX = pDesc->fSizeX;
    Desc.fSizeY = pDesc->fSizeY;
   
    Desc.iData = 10;
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotationPerSec = 0.f;
    Desc.fDepth = pDesc->fDepth;
    Desc.eTag = pDesc->eTag;

    m_eTag = Desc.eTag;


    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;


    if (FAILED(Add_Components(Desc.iData)))
        return E_FAIL;

    return S_OK;
}

void CLoading_UI::Priority_Update(_float fTimeDelta)
{
    
}

void CLoading_UI::Update(_float fTimeDelta)
{
    // 애니메이션
    if(m_eTag == LOADING_GAGE || m_eTag == LOADING_LOGO)
    {
        m_fIndex.x = 7;
        if (m_fIndex.y < 6)
        {
            if (int(fTimeDelta * 10) % 4 == 0)
            {
                m_fIndex.y += 1;

            }
        }

        if (m_fPercent == 100 && m_fIndex.y == 6)
        {
            m_fIndex.y += 1;
        }

    }


    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
        return;
}

void CLoading_UI::Late_Update(_float fTimeDelta)
{

}

HRESULT CLoading_UI::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;
    if (m_eTag == LOADING_GAMENAME)
    {
        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;
    }
    else
    {
        if (FAILED(m_pShaderCom->Begin(2)))
            return E_FAIL;
    }

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoading_UI::Add_Components(_int iNum)
{
    switch (m_eTag)
    {
    case Client::CLoading_UI::LOADING_LOGO:
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading0"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom_Loading0))))
            return E_FAIL;
        break;
    case Client::CLoading_UI::LOADING_GAGE:
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading1"),
            TEXT("Com_ArmTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom_Loading1))))
            return E_FAIL;
        break;
    case Client::CLoading_UI::LOADING_GAMENAME:
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GameTitle"),
            TEXT("Com_ArmTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom_Loading2))))
            return E_FAIL;
        break;
    case Client::CLoading_UI::LOADING_END:
        break;
    default:
        break;
    }





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

HRESULT CLoading_UI::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_Index", &m_fIndex, sizeof(_float2))))
        return E_FAIL;

    switch (m_eTag)
    {
    case Client::CLoading_UI::LOADING_LOGO:
        if (FAILED(m_pTextureCom_Loading0->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;
        break;
    case Client::CLoading_UI::LOADING_GAGE:

        if (FAILED(m_pTextureCom_Loading1->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;
        break;
    case Client::CLoading_UI::LOADING_GAMENAME:
        if (FAILED(m_pTextureCom_Loading2->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        break;
    case Client::CLoading_UI::LOADING_END:
        break;
    default:
        break;
    }



    return S_OK;
}

CLoading_UI* CLoading_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLoading_UI* pInstance = new CLoading_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CLoading_UI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLoading_UI::Clone(void* pArg)
{
    CLoading_UI* pInstance = new CLoading_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CLoading_UI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLoading_UI::Free()
{
    __super::Free();
    switch (m_eTag)
    {
    case Client::CLoading_UI::LOADING_LOGO:
        Safe_Release(m_pTextureCom_Loading0);
        break;
    case Client::CLoading_UI::LOADING_GAGE:
        Safe_Release(m_pTextureCom_Loading1);
        break;
    case Client::CLoading_UI::LOADING_GAMENAME: 
        Safe_Release(m_pTextureCom_Loading2);
        break;
    case Client::CLoading_UI::LOADING_END:
        break;
    default:
        break;
    }

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
}
