#include "stdafx.h"
#include "..\Public\BulletUI.h"

#include "GameInstance.h"

CBulletUI::CBulletUI(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
	: CUIObject{pDevice,pContext}
{}

CBulletUI::CBulletUI(const CBulletUI & Prototype)
	: CUIObject{Prototype}
{}

HRESULT CBulletUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBulletUI::Initialize(void * pArg)
{
	BULLET_UI_DESC* pDesc = (BULLET_UI_DESC*)pArg;
	m_iIndex = pDesc->iIndex;
	m_eLevel = pDesc->eLevel;
	m_pPlayer = pDesc->pPlayer;
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if(FAILED(Add_Components(pDesc->iData)))
		return E_FAIL;
    return S_OK;
}

void CBulletUI::Priority_Update(_float fTimeDelta)
{}

void CBulletUI::Update(_float fTimeDelta)
{
	if(*m_pPlayer->Get_WeaponState() == CPlayer::WEAPON_KATANA)
		m_bDraw = false;
	else
		m_bDraw = true;
}

void CBulletUI::Late_Update(_float fTimeDelta)
{
	if(m_bDraw)
	{
		if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI,this)))
			return;
	}
}

HRESULT CBulletUI::Render()
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

HRESULT CBulletUI::Add_Components(_int iNum)
{
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_CenterUI"),
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

HRESULT CBulletUI::Bind_ShaderResources()
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

CBulletUI * CBulletUI::Create(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
{
	CBulletUI* pInstance = new CBulletUI(pDevice,pContext);
	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBulletUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBulletUI::Clone(void * pArg)
{
	CBulletUI* pInstance = new CBulletUI(*this);
	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBulletUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBulletUI::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
