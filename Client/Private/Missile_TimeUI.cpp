#include "stdafx.h"
#include "..\Public\Missile_TimeUI.h"

#include "GameInstance.h"

CMissile_TimeUI::CMissile_TimeUI(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
	: CUIObject{pDevice,pContext}
{}

CMissile_TimeUI::CMissile_TimeUI(const CMissile_TimeUI & Prototype)
	: CUIObject{Prototype}
{}

HRESULT CMissile_TimeUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMissile_TimeUI::Initialize(void * pArg)
{
	MISSILE_TIMER_UI_DESC* pDesc = (MISSILE_TIMER_UI_DESC*)pArg;
	m_iIndex = pDesc->iIndex;
	m_eLevel = pDesc->eLevel;
	m_fTimer = pDesc->fTimer;
	m_iRound = pDesc->iRound;
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if(FAILED(Add_Components(pDesc->iData)))
		return E_FAIL;
    return S_OK;
}

void CMissile_TimeUI::Priority_Update(_float fTimeDelta)
{}

void CMissile_TimeUI::Update(_float fTimeDelta)
{
	if(*m_fTimer <= 0.f)
		m_bDead = true;

	if(*m_iRound == MISSILEROUND)
	{
		m_bDraw = true;
	} else
	{
		m_bDraw = false;
	}
}

void CMissile_TimeUI::Late_Update(_float fTimeDelta)
{
	if(m_bDraw)
	{
		if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI,this)))
			return;
	}
}

HRESULT CMissile_TimeUI::Render()
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

HRESULT CMissile_TimeUI::Add_Components(_int iNum)
{
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_Missile_Timer"),
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

HRESULT CMissile_TimeUI::Bind_ShaderResources()
{
	if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom,"g_WorldMatrix")))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;
	if(FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom,"g_Texture",0)))
		return E_FAIL;
	_float fGauge = *m_fTimer * 0.5f;
	if(FAILED(m_pShaderCom->Bind_RawValue("g_fGageAmount",&fGauge,sizeof(float))))
		return E_FAIL;
	return S_OK;
}

CMissile_TimeUI * CMissile_TimeUI::Create(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
{
	CMissile_TimeUI* pInstance = new CMissile_TimeUI(pDevice,pContext);
	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMissile_TimeUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMissile_TimeUI::Clone(void * pArg)
{
	CMissile_TimeUI* pInstance = new CMissile_TimeUI(*this);
	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CMissile_TimeUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMissile_TimeUI::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
