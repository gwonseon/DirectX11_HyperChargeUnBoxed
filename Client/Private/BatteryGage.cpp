#include "stdafx.h"
#include "..\Public\BatteryGage.h"

#include "GameInstance.h"
CBatteryGage::CBatteryGage(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
	: CUIObject{pDevice,pContext}
{}

CBatteryGage::CBatteryGage(const CBatteryGage & Prototype)
	: CUIObject{Prototype}
{}

HRESULT CBatteryGage::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBatteryGage::Initialize(void * pArg)
{
	BATTERYGAGE_UI_DESC* pDesc = (BATTERYGAGE_UI_DESC*)pArg;
	m_iIndex = pDesc->iIndex;
	m_eLevel = pDesc->eLevel;
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if(FAILED(Add_Components(pDesc->iData)))
		return E_FAIL;
	return S_OK;
}

void CBatteryGage::Priority_Update(_float fTimeDelta)
{}

void CBatteryGage::Update(_float fTimeDelta)
{
	if(m_fBatteryGauge >= 70.f)
	{
		m_iBattery = 0;
	} else if(m_fBatteryGauge >= 50.f)
	{
		m_iBattery = 1;
	} else if(m_fBatteryGauge >= 30.f)
	{
		m_iBattery = 2;
	} else if(m_fBatteryGauge > 0.f)
	{
		m_iBattery = 3;
	} else
	{
		m_iBattery = 4;
	}
}

void CBatteryGage::Late_Update(_float fTimeDelta)
{
	if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI,this)))
		return;
}

HRESULT CBatteryGage::Render()
{
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Begin(4)))
		return E_FAIL;
	if(FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if(FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CBatteryGage::Add_Components(_int iNum)
{
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_UIBar"),
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

HRESULT CBatteryGage::Bind_ShaderResources()
{
	if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom,"g_WorldMatrix")))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;
	if(FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom,"g_Texture",m_iIndex)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_RawValue("g_fGageAmount",&m_fBatteryGauge,sizeof(float))))
		return E_FAIL;
	return S_OK;
}

CBatteryGage * CBatteryGage::Create(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
{
	CBatteryGage* pInstance = new CBatteryGage(pDevice,pContext);
	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBatteryGage");
		Safe_Release(pInstance);
	}
	return pInstance;

}

CGameObject * CBatteryGage::Clone(void * pArg)
{
	CBatteryGage* pInstance = new CBatteryGage(*this);
	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBatteryGage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBatteryGage::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
