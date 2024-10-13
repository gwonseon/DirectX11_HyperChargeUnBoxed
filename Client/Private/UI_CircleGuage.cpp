
#include "stdafx.h"
#include "..\Public\UI_CircleGuage.h"

#include "GameInstance.h"

CUI_CircleGuage::CUI_CircleGuage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_CircleGuage::CUI_CircleGuage(const CUI_CircleGuage& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_CircleGuage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_CircleGuage::Initialize(void* pArg)
{
	CIRCLEGAUGE_DESC* pDesc = (CIRCLEGAUGE_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(1)))
		return E_FAIL;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f));

	return S_OK;
}

void CUI_CircleGuage::Priority_Update(_float fTimeDelta)
{
}

void CUI_CircleGuage::Update(_float fTimeDelta)
{
	if (m_bCharging == true)
	{
		m_fGuaging_Time += fTimeDelta * 10;
	}
	else
	{
		m_fGuaging_Time = 0.f;
		m_bCharging = false;
	}
}

void CUI_CircleGuage::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
		return;
}

HRESULT CUI_CircleGuage::Render()
{
	if (m_bCharging == true)
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
	}

	return S_OK;
}

HRESULT CUI_CircleGuage::Add_Components(_int iNum)
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CircleGuage"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCircleGuage"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	return S_OK;

}

HRESULT CUI_CircleGuage::Bind_ShaderResources()
{

	_float2 Winsize = { g_iWinSizeX,g_iWinSizeY };
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_fGuaging_Time, sizeof(float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Winsize", &Winsize,	sizeof(_float2))))
		return E_FAIL;



	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;


	return S_OK;
}

CUI_CircleGuage* CUI_CircleGuage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CircleGuage* pInstance = new CUI_CircleGuage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_CircleGuage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_CircleGuage::Clone(void* pArg)
{
	CUI_CircleGuage* pInstance = new CUI_CircleGuage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CUI_CircleGuage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_CircleGuage::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
