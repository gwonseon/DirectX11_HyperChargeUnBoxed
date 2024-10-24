#include "stdafx.h"
#include "..\Public\MenuUI.h"

#include "GameInstance.h"


CMenuUI::CMenuUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CMenuUI::CMenuUI(const CMenuUI& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CMenuUI::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CMenuUI::Initialize(void* pArg)
{
    MENUUI_DESC* pDesc = (MENUUI_DESC*)pArg;
    MENUUI_DESC Desc{};
    Desc.fX = pDesc->fX;
    Desc.fY = pDesc->fY;
    Desc.fSizeX = pDesc->fSizeX;
    Desc.fSizeY = pDesc->fSizeY;
    Desc.fDepth = pDesc->fDepth;
    Desc.eTag = pDesc->eTag;
    m_eTag = Desc.eTag;


    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CMenuUI::Priority_Update(_float fTimeDelta)
{
}

void CMenuUI::Update(_float fTimeDelta)
{
}

void CMenuUI::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
        return;
}

HRESULT CMenuUI::Render()
{

    m_pGameInstance->Set_BlendState(CGraphic_Device::BS_ALPHA);
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;

}

HRESULT CMenuUI::Add_Components()
{
    if (m_eTag == LOGO_BACKGOUND)
    {
        if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Menu_UI0"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
    }
    if(m_eTag == LOGO_GAMENAME)
    {
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GameTitle"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
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

HRESULT CMenuUI::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;
    return S_OK;
}

CMenuUI* CMenuUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMenuUI* pInstance = new CMenuUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMenuUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMenuUI::Clone(void* pArg)
{
    CMenuUI* pInstance = new CMenuUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CMenuUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMenuUI::Free()
{
    __super::Free();
  
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
