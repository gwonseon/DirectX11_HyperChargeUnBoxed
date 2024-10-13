#include "stdafx.h"
#include "..\Public\ButtonUI.h"

#include "GameInstance.h"

#include "Level_Loading.h"

CButtonUI::CButtonUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CButtonUI::CButtonUI(const CButtonUI& Prototype)
	: CUIObject{ Prototype } 
{
}

HRESULT CButtonUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CButtonUI::Initialize(void* pArg)
{
    BUTTONUI_DESC		Desc{};

    BUTTONUI_DESC* pDesc = (BUTTONUI_DESC*)pArg;

    m_vObjectPos.x =  Desc.fX = pDesc->fX;
    m_vObjectPos.y =  Desc.fY = pDesc->fY;
    m_fSizeX = Desc.fSizeX = pDesc->fSizeX;
    m_fSizeY = Desc.fSizeY = pDesc->fSizeY;

    Desc.iData = 10;
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotationPerSec = 0.f;
    Desc.fDepth = pDesc->fDepth;
    Desc.eTag = pDesc->eTag;
    m_eTag = Desc.eTag;

    m_pGameInstance->GetInstance();
    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;


    if (FAILED(Add_Components(Desc.iData)))
        return E_FAIL;

	return S_OK;
}

void CButtonUI::Priority_Update(_float fTimeDelta)
{
}

void CButtonUI::Update(_float fTimeDelta)
{
    // 정규화된 마우스 좌표 받아오기
    m_vMousePos = m_pGameInstance->Get_MousePos_NDC(g_hWnd, g_iWinSizeX, g_iWinSizeY);
    m_fButtonRange = m_pGameInstance->Object_NDC_Cal(m_vObjectPos, m_fSizeX, m_fSizeY, g_iWinSizeX, g_iWinSizeY);

    if( GetKeyState(VK_LBUTTON) & 0x8000  )
    {
        if (m_vMousePos.x >= m_fButtonRange.w && m_vMousePos.x <= m_fButtonRange.x)
        {
            if (m_vMousePos.y <= m_fButtonRange.y && m_vMousePos.y >= m_fButtonRange.z)
            {
                m_bClick = true;
            }
       /*     m_bClick = true;
            m_pGameInstance->GetInstance()->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY));
            m_pGameInstance->GetInstance()->Close_Level(LEVEL_LOADING);*/
        }
    }
}

void CButtonUI::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
        return;
}

HRESULT CButtonUI::Render()
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

HRESULT CButtonUI::Add_Components(_int iNum)
{
    if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Button0"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom_Button0))))
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

HRESULT CButtonUI::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom_Button0->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
        return E_FAIL;

    return S_OK;

}

CButtonUI* CButtonUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CButtonUI* pInstance = new CButtonUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CButtonUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CButtonUI::Clone(void* pArg)
{
    CButtonUI* pInstance = new CButtonUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CButtonUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CButtonUI::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom_Button0);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
}
