#include "stdafx.h"
#include "..\Public\InGameUI.h"

#include "GameInstance.h"

CInGameUI::CInGameUI(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
	: CUIObject{pDevice,pContext}
{

}

CInGameUI::CInGameUI(const CInGameUI& Prototype)
	: CUIObject{Prototype}
{}

HRESULT CInGameUI::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CInGameUI::Initialize(void* pArg)
{
	INGAMEUI_DESC* pDesc = (INGAMEUI_DESC*)pArg;
	m_eUIType = pDesc->eUITag;
	m_iIndex = pDesc->iIndex;
	m_fUIPosition = {pDesc->fX,pDesc->fY,0.f};
	m_pPlayer = pDesc->pPlayer;
	m_eLevel = pDesc->eLevel;
	m_pCircle = pDesc->pCircle;

	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if(FAILED(Add_Components(pDesc->iData)))
		return E_FAIL;

	if(m_eUIType == UI_CONVERSATIONBOX|| m_eUIType == UI_CHARACTER)
		m_bDraw = false;
	if(m_eUIType == UI_CONVERSATIONBOX_BACKGROUND)
		m_bDraw = true;

	return S_OK;
}


void CInGameUI::Priority_Update(_float fTimeDelta)
{}

void CInGameUI::Update(_float fTimeDelta)
{
	switch(m_eUIType)
	{
	case Client::CInGameUI::UI_BUILDMODE_F:
	{
		if(*m_pPlayer->Get_BuildMode() == true)
			m_bDraw = true;
		else
			m_bDraw = false;
		break;
	}
	case Client::CInGameUI::UI_BUILDMODE_CONVERSATIONBOX:
	{
		if(*m_pPlayer->Get_BuildMode() == true)
			m_bDraw = true;
		else
			m_bDraw = false;
		break;
	}
	case Client::CInGameUI::UI_MODECHANGE_ICON:
	{
		if(*m_pPlayer->Get_BuildMode() == true)
			m_iIndex = 1;
		else
			m_iIndex = 0;
		break;
	}
	case Client::CInGameUI::UI_CENTERICON:
	if(*m_pPlayer->Get_Reloading() == true)
	{
		m_bDraw = true;
		m_iIndex = 0;
	} 
	else if(m_pPlayer->Get_Build_Gauging() == true)
	{
		m_bDraw = true;

		m_iIndex = 1;
	} // 특정 조건들 가져와서 인덱스 2번으로 
	else if(m_pCircle->Get_Interaction() == true)
	{
		m_bDraw = true;
		m_iIndex = 2;
	} 
	else
		m_bDraw = false;

	m_iIndex;// 장전이냐 건축이냐에 따라서 모양이 바뀜
	break;

	case Client::CInGameUI::UI_SLICE:
	// 칼일 때 안그리기
	if(*m_pPlayer->Get_WeaponState() == CPlayer::WEAPON_KATANA)
		m_bDraw = false;
	else
		m_bDraw = true;
	break;
	
	default:
	break;
	}

}

void CInGameUI::Late_Update(_float fTimeDelta)
{
	
	if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI,this)))
		return;
}

HRESULT CInGameUI::Render()
{
	//if (*m_pPlayer->Get_Reloading() == true)
	/*m_pGameInstance->Set_BlendState(CGraphic_Device::BS_ALPHA);*/


	if(m_bDraw== true)
	{
		if(FAILED(Bind_ShaderResources()))
			return E_FAIL;

		if((UI_CONVERSATIONBOX == m_eUIType || UI_CONVERSATIONBOX_BACKGROUND == m_eUIType) && m_iIndex == 0)
		{
			// 점점 투명해짐
			if(FAILED(m_pShaderCom->Begin(3)))
				return E_FAIL;
		} 
		else if(UI_BUILDMODE_CONVERSATIONBOX == m_eUIType)
		{
			if(FAILED(m_pShaderCom->Begin(6)))
				return E_FAIL;

		}
		else
		{
			// 그냥 그림
			if(FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;
		}

		if(FAILED(m_pVIBufferCom->Bind_Buffers()))
			return E_FAIL;

		if(FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}
	return S_OK;
}




HRESULT CInGameUI::Add_Components(_int iNum)
{
	switch(m_eUIType)
	{
	
	case Client::CInGameUI::UI_BUILDMODE_F:
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_FIcon"),
		TEXT("Com_Texture"),reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	break;
	case Client::CInGameUI::UI_CHARACTER:
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_Character"),
		TEXT("Com_Texture"),reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	break;
	case Client::CInGameUI::UI_CONVERSATIONBOX:
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_UIBackGround"),
		TEXT("Com_Texture"),reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	break;
	case Client::CInGameUI::UI_CONVERSATIONBOX_BACKGROUND:
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_UIBackGround"),
		TEXT("Com_Texture"),reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	break;

	case Client::CInGameUI::UI_BUILDMODE_CONVERSATIONBOX:
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_UIBackGround"),
		TEXT("Com_Texture"),reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	break;
	
	case Client::CInGameUI::UI_MODECHANGE_ICON:
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_ModeChangeIcon"),
		TEXT("Com_Texture"),reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	break;
	
	case Client::CInGameUI::UI_CENTERICON:
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_CenterUI"),
		TEXT("Com_Texture"),reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	break;
	case Client::CInGameUI::UI_SLICE:
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_Slice"),
		TEXT("Com_Texture"),reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	break;
	default:
	break;
	}

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

HRESULT CInGameUI::Bind_ShaderResources()
{

	if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom,"g_WorldMatrix")))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;

	if(m_eUIType == UI_CHARACTER ||
		m_eUIType == UI_BUILDMODE_CONVERSATIONBOX ||
		m_eUIType == UI_CONVERSATIONBOX_BACKGROUND ||
		m_eUIType == UI_CONVERSATIONBOX ||
		m_eUIType == UI_MODECHANGE_ICON)
	{
		if(FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom,"g_Texture",m_iIndex)))
			return E_FAIL;
	} 
	else if(m_eUIType == UI_CENTERICON)
	{
		if(FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom,"g_Texture",m_iIndex)))
			return E_FAIL;
	} 
	else
	{
		if(FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom,"g_Texture",0)))
			return E_FAIL;
	}
	return S_OK;
}

CInGameUI* CInGameUI::Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
{
	CInGameUI* pInstance = new CInGameUI(pDevice,pContext);

	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInGameUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CInGameUI::Clone(void* pArg)
{
	CInGameUI* pInstance = new CInGameUI(*this);

	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CInGameUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CInGameUI::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}

