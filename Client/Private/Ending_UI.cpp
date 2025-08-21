#include "stdafx.h"
#include "..\Public\Ending_UI.h"

#include "GameInstance.h"
CEnding_UI::CEnding_UI(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
	: CUIObject{pDevice,pContext}
{}

CEnding_UI::CEnding_UI(const CEnding_UI & Prototype)
	: CUIObject{Prototype}
{}

HRESULT CEnding_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEnding_UI::Initialize(void * pArg)
{
	ENDING_UI_DESC* pDesc = (ENDING_UI_DESC*)pArg;
	m_iIndex = pDesc->iIndex;
	m_eLevel = pDesc->eLevel;
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if(FAILED(Add_Components(pDesc->iData)))
		return E_FAIL;
    return S_OK;
}

void CEnding_UI::Priority_Update(_float fTimeDelta)
{}

void CEnding_UI::Update(_float fTimeDelta)
{
	if(m_bRoundEnd == true)
	{
		m_bDraw = true;
	} else
	{
		m_bDraw = false;
	}
}

void CEnding_UI::Late_Update(_float fTimeDelta)
{
	if(m_bDraw)
	{
		if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI,this)))
			return;
	}
	
}

HRESULT CEnding_UI::Render()
{
	
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;
	if(FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if(FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CEnding_UI::Add_Components(_int iNum)
{
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_Victory"),
		TEXT("Com_Texture"),reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	/* For.Com_Shader */
	if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"),reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"),reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	return S_OK;
}

HRESULT CEnding_UI::Bind_ShaderResources()
{
	if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom,"g_WorldMatrix")))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;
	if(FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom,"g_Texture",0)))
		return E_FAIL;

	return S_OK;
}

CEnding_UI * CEnding_UI::Create(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
{
	CEnding_UI* pInstance = new CEnding_UI(pDevice,pContext);
	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnding_UI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEnding_UI::Clone(void * pArg)
{
	CEnding_UI* pInstance = new CEnding_UI(*this);
	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CEnding_UI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEnding_UI::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
