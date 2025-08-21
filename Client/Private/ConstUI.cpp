#include "stdafx.h"
#include "..\Public\ConstUI.h"

#include "GameInstance.h"

CConstUI::CConstUI(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
	: CUIObject{pDevice,pContext}
{}

CConstUI::CConstUI(const CConstUI & Prototype)
	: CUIObject{Prototype}
{}

HRESULT CConstUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CConstUI::Initialize(void * pArg)
{
	CONST_UI_DESC* pDesc = (CONST_UI_DESC*)pArg;
	m_iIndex = pDesc->iIndex;
	m_eLevel = pDesc->eLevel;
	m_eUIType = pDesc->eUITag;
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if(FAILED(Add_Components(pDesc->iData)))
		return E_FAIL;
	return S_OK;
}

void CConstUI::Priority_Update(_float fTimeDelta)
{}

void CConstUI::Update(_float fTimeDelta)
{}

void CConstUI::Late_Update(_float fTimeDelta)
{
	if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI,this)))
		return;
}

HRESULT CConstUI::Render()
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

HRESULT CConstUI::Add_Components(_int iNum)
{
	_wstring strTextureTag{};
	switch(m_eUIType)
	{
	case Client::CConstUI::UI_SHIFT:
	strTextureTag = TEXT("Prototype_Component_Texture_Shift");
	break;
	case Client::CConstUI::UI_RBUTTON:
	strTextureTag = TEXT("Prototype_Component_Texture_RButton");
	break;
	case Client::CConstUI::UI_LBUTTON:
	strTextureTag = TEXT("Prototype_Component_Texture_LButton");
	break;
	case Client::CConstUI::UI_SPACE:
	strTextureTag = TEXT("Prototype_Component_Texture_Space");
	break;
	case Client::CConstUI::UI_ENERGY_ICON:
	strTextureTag = TEXT("Prototype_Component_Texture_EnergyIcon");
	break;
	case Client::CConstUI::UI_HP_ICON:
	strTextureTag = TEXT("Prototype_Component_Texture_HpIcon");
	break;
	case Client::CConstUI::UI_CREDIT_ICON:
	strTextureTag = TEXT("Prototype_Component_Texture_CreditIcon");
	break;
	case Client::CConstUI::UI_RUN_ICON:
	strTextureTag = TEXT("Prototype_Component_Texture_RunIcon");
	break;
	case Client::CConstUI::UI_JUMP_ICON:
	strTextureTag = TEXT("Prototype_Component_Texture_JumpIcon");
	break;
	case Client::CConstUI::UI_VIEWCHANGE_ICON:
	strTextureTag = TEXT("Prototype_Component_Texture_ViewChangeIcon");
	break;
	case Client::CConstUI::UI_PUNCH_ICON:
		strTextureTag = TEXT("Prototype_Component_Texture_PuchIcon");
	break;
	case Client::CConstUI::UI_V:
	strTextureTag = TEXT("Prototype_Component_Texture_VIcon");
	break;
	case Client::CConstUI::UI_F:
	strTextureTag = TEXT("Prototype_Component_Texture_FIcon");
	break;
	case Client::CConstUI::UI_C:
	strTextureTag = TEXT("Prototype_Component_Texture_CIcon");
	break;
	case Client::CConstUI::UI_NUCLEAR:
	strTextureTag = TEXT("Prototype_Component_Texture_Nuclear");
	break;
	default:
	break;
	}
	
	if(FAILED(__super::Add_Component(m_eLevel,strTextureTag,
		TEXT("Com_Texture"),reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"),reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"),reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	return S_OK;
}

HRESULT CConstUI::Bind_ShaderResources()
{
	if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom,"g_WorldMatrix")))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;
	if(FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom,"g_Texture",m_iIndex)))
		return E_FAIL;
	return S_OK;
}

CConstUI * CConstUI::Create(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
{
	CConstUI* pInstance = new CConstUI(pDevice,pContext);
	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CConstUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CConstUI::Clone(void * pArg)
{
	CConstUI* pInstance = new CConstUI(*this);
	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CConstUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CConstUI::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
