#include "stdafx.h"
#include "..\Public\PlayerHp.h"

#include "GameInstance.h"

CPlayerHp::CPlayerHp(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
	: CUIObject{pDevice,pContext}
{}

CPlayerHp::CPlayerHp(const CPlayerHp & Prototype)
	: CUIObject{Prototype}
{}

HRESULT CPlayerHp::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayerHp::Initialize(void * pArg)
{
	PLAYEYER_HP_UI_DESC* pDesc = (PLAYEYER_HP_UI_DESC*)pArg;
	m_fPlayerHp = pDesc->fPlayerHP;
	m_iIndex = pDesc->iIndex;
	m_eLevel = pDesc->eLevel;
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if(FAILED(Add_Components(pDesc->iData)))
		return E_FAIL;

    return S_OK;
}

void CPlayerHp::Priority_Update(_float fTimeDelta)
{}

void CPlayerHp::Update(_float fTimeDelta)
{}

void CPlayerHp::Late_Update(_float fTimeDelta)
{
	if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI,this)))
		return;
}

HRESULT CPlayerHp::Render()
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

HRESULT CPlayerHp::Add_Components(_int iNum)
{
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_UIBar"),
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

HRESULT CPlayerHp::Bind_ShaderResources()
{
	if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom,"g_WorldMatrix")))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;

	if(FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom,"g_Texture",m_iIndex)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_RawValue("g_fGageAmount",m_fPlayerHp,sizeof(float))))
		return E_FAIL;
    return S_OK;
}

CPlayerHp * CPlayerHp::Create(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
{
	CPlayerHp* pInstance = new CPlayerHp(pDevice,pContext);
	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerHp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayerHp::Clone(void * pArg)
{
	CPlayerHp* pInstance = new CPlayerHp(*this);
	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayerHp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerHp::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
