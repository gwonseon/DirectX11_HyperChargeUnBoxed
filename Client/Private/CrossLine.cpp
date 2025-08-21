#include "stdafx.h"
#include "..\Public\CrossLine.h"

#include "GameInstance.h"

CCrossLine::CCrossLine(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
	: CUIObject{pDevice,pContext}
{}

CCrossLine::CCrossLine(const CCrossLine& Prototype)
	: CUIObject{Prototype}
{}

HRESULT CCrossLine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCrossLine::Initialize(void* pArg)
{

	UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);
	m_eLevel = pDesc->eLevel;

	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if(FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CCrossLine::Priority_Update(_float fTimeDelta)
{}

void CCrossLine::Update(_float fTimeDelta)
{
	if((GetAsyncKeyState(VK_F7) & 0x0001))
		iChangeNum++;
}

void CCrossLine::Late_Update(_float fTimeDelta)
{
	if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI,this)))
		return;

}

HRESULT CCrossLine::Render()
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

HRESULT CCrossLine::Add_Components()
{
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_Logo2"),
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

HRESULT CCrossLine::Bind_ShaderResources()
{

	if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom,"g_WorldMatrix")))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;
	if(FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom,"g_Texture",iChangeNum)))
		return E_FAIL;
	return S_OK;
}

CCrossLine* CCrossLine::Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
{
	CCrossLine* pInstance = new CCrossLine(pDevice,pContext);

	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCrossLine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCrossLine::Clone(void* pArg)
{
	CCrossLine* pInstance = new CCrossLine(*this);

	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCrossLine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCrossLine::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}