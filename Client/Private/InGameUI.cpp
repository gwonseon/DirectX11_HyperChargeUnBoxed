#include "stdafx.h"
#include "..\Public\InGameUI.h"

#include "GameInstance.h"

CInGameUI::CInGameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext } 
{

}

CInGameUI::CInGameUI(const CInGameUI& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CInGameUI::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CInGameUI::Initialize(void* pArg)
{
    UIOBJECT_DESC		Desc{};

    UIOBJECT_DESC* pDesc = (UIOBJECT_DESC*)pArg;

    Desc.fX = pDesc->fX;
    Desc.fY = pDesc->fY;
    Desc.fSizeX = pDesc->fSizeX;
    Desc.fSizeY = pDesc->fSizeY;
    m_iKind = pDesc->iData;
    m_iCount = pDesc->m_iCount; // ArmCannon Count

    Desc.iData = 10;
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotationPerSec = 0.f;
  
    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Add_Components(Desc.iData)))
        return E_FAIL;

    return S_OK;
}


void CInGameUI::Priority_Update(_float fTimeDelta)
{
}

void CInGameUI::Update(_float fTimeDelta)
{

    HP_UI();
    ArmCannon_UI();
    
}

void CInGameUI::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
        return;


    //if (m_iKind == 0 && (GetAsyncKeyState(VK_F5) & 0x0001))
    //    m_iHp++;

    //
    //if (m_iKind == 1 && GetAsyncKeyState(VK_F4) & 0x0001)
    //    m_iCount-=1;
   
}

HRESULT CInGameUI::Render()
{

    

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(1)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CInGameUI::HP_UI()
{
    if (m_iHp >= 6)
        m_iHp = 0;

    
}

void CInGameUI::ArmCannon_UI()
{
   if (m_iCount < 0)
        m_iCount = 2;
 
}

HRESULT CInGameUI::Add_Components(_int iNum)
{


    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI0"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI1"),
        TEXT("Com_ArmTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom_ArmCannon))))
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

HRESULT CInGameUI::Bind_ShaderResources()
{

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;


    switch (m_iKind)
    {
    case 0:

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iHp)))
            return E_FAIL;
        break;

    case 1:
        if (FAILED(m_pTextureCom_ArmCannon->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iCount)))
            return E_FAIL;
        break;
    default:
        break;
    }

    return S_OK;
}

CInGameUI* CInGameUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInGameUI* pInstance = new CInGameUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CInGameUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInGameUI::Clone(void* pArg)
{
    CInGameUI* pInstance = new CInGameUI(*this);
    
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CInGameUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInGameUI::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom_ArmCannon);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
