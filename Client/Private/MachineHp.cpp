#include "stdafx.h"
#include "..\Public\MachineHp.h"

#include "GameInstance.h"
CMachineHp::CMachineHp(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
	: CUIObject{pDevice,pContext}
{}

CMachineHp::CMachineHp(const CMachineHp & Prototype)
	: CUIObject{Prototype}
{}

HRESULT CMachineHp::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMachineHp::Initialize(void * pArg)
{
	MACHINE_HP_UI_DESC* pDesc = (MACHINE_HP_UI_DESC*)pArg;
	m_fMachineHp = pDesc->fMachineHP;
	m_iIndex = pDesc->iIndex;
	m_eLevel = pDesc->eLevel;
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if(FAILED(Add_Components(pDesc->iData)))
		return E_FAIL;
    return S_OK;
}

void CMachineHp::Priority_Update(_float fTimeDelta)
{}

void CMachineHp::Update(_float fTimeDelta)
{}

void CMachineHp::Late_Update(_float fTimeDelta)
{
	if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI,this)))
		return;
}

HRESULT CMachineHp::Render()
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

HRESULT CMachineHp::Add_Components(_int iNum)
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

HRESULT CMachineHp::Bind_ShaderResources()
{
	if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom,"g_WorldMatrix")))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;
	if(FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom,"g_Texture",m_iIndex)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_RawValue("g_fGageAmount",m_fMachineHp,sizeof(float))))
		return E_FAIL;
	return S_OK;
}

CMachineHp * CMachineHp::Create(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
{
	CMachineHp* pInstance = new CMachineHp(pDevice,pContext);
	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMachineHp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMachineHp::Clone(void * pArg)
{
	CMachineHp* pInstance = new CMachineHp(*this);
	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CMachineHp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMachineHp::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
