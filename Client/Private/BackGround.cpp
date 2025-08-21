#include "stdafx.h"
#include "..\Public\BackGround.h"

#include "GameInstance.h"

CBackGround::CBackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUIObject{ pDevice, pContext }
{
}

CBackGround::CBackGround(const CBackGround & Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CBackGround::Initialize_Prototype()
{
	/* 패킷, 파일입ㅇ출력을 통한 초기화. */

	return S_OK;
}

HRESULT CBackGround::Initialize(void * pArg)
{
	/* 추가적으로 초기화가 필요하다면 수행해준다. */
//	BACKGROUND_DESC*		pDesc = static_cast<BACKGROUND_DESC*>(pArg);

	UIOBJECT_DESC			Desc{};
	Desc.fSpeedPerSec = 5.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	UIOBJECT_DESC* pDesc = (UIOBJECT_DESC*)pArg;
	m_eLevel = pDesc->eLevel;
	Desc.eLevel = m_eLevel;
	Desc.fDepth = pDesc->fDepth;
	Desc.fSizeX = pDesc->fSizeX; // g_iWinSizeX
	Desc.fSizeY = pDesc->fSizeY;
	Desc.fX = pDesc->fX ;
	Desc.fY = pDesc->fY ;
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	return S_OK;
}

void CBackGround::Priority_Update(_float fTimeDelta)
{
	_uint iData = 10;
}

void CBackGround::Update(_float fTimeDelta)
{
	_uint iData = 10;
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
        return;
}

void CBackGround::Late_Update(_float fTimeDelta)
{

}

HRESULT CBackGround::Render()
{


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL; 

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;



	return S_OK;
}

HRESULT CBackGround::Add_Components()
{
	/* 멤버변수로 직접 참조를 하게되면 */
	/* 1. 내가 내 컴포넌트를 이용하고자할 때 굳이 검색이 필요없이 특정 멤버변수로 바로 기능을 이용하면 된다. */
	/* 2. 다른 객체가 내 컴포넌트를 검색하고자 할때 스위치케이스가 겁나 늘어나는 상황. */

	/* For.Com_Texture */
	switch (m_eLevel)
	{
	case Client::LEVEL_LOADING:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
			TEXT("Com_Texture_Loading"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;

		break;
	case Client::LEVEL_LOGO:	
		if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Menu_Back"),
		TEXT("Com_Texture_Menu"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;

		break;
	case Client::LEVEL_GAMEPLAY:
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Logo"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	case Client::LEVEL_IMGUI:
		if (FAILED(__super::Add_Component(LEVEL_IMGUI, TEXT("Prototype_Component_Texture_Save"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	case Client::LEVEL_YARD:
		if (FAILED(__super::Add_Component(LEVEL_YARD, TEXT("Prototype_Component_Texture_Logo"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	default:
		break;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CBackGround::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	
	return S_OK;
}

CBackGround * CBackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	
	CBackGround*		pInstance = new CBackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBackGround::Clone(void * pArg)
{
	CBackGround*		pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom_Menu);
	Safe_Release(m_pTextureCom_Loading);
	
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}