#include "stdafx.h"
#include "..\Public\DeadUI.h"

#include "GameInstance.h"

CDeadUI::CDeadUI(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
	: CUIObject{pDevice,pContext}
{}

CDeadUI::CDeadUI(const CDeadUI & Prototype)
	: CUIObject{Prototype}
{}

HRESULT CDeadUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDeadUI::Initialize(void * pArg)
{
	DEAD_UI_DESC* pDesc = (DEAD_UI_DESC*)pArg;
	m_iIndex = pDesc->iIndex;
	m_eLevel = pDesc->eLevel;
	m_pPlayer = pDesc->pPlayer;
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if(FAILED(Add_Components(pDesc->iData)))
		return E_FAIL;
    return S_OK;
}

void CDeadUI::Priority_Update(_float fTimeDelta)
{}

void CDeadUI::Update(_float fTimeDelta)
{
	if(m_pPlayer->Get_knockdown() == true)
		m_bDraw = true;
	else
		m_bDraw = false;
}

void CDeadUI::Late_Update(_float fTimeDelta)
{
	if(m_bDraw)
	{
		if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI,this)))
			return;
	}
}

HRESULT CDeadUI::Render()
{
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Begin(5)))
		return E_FAIL;
	if(FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if(FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CDeadUI::Add_Components(_int iNum)
{
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_Death"),
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

HRESULT CDeadUI::Bind_ShaderResources()
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

CDeadUI * CDeadUI::Create(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
{
	CDeadUI* pInstance = new CDeadUI(pDevice,pContext);
	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDeadUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDeadUI::Clone(void * pArg)
{
	CDeadUI* pInstance = new CDeadUI(*this);
	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CDeadUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDeadUI::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
