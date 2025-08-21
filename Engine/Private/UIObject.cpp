#include "..\Public\UIObject.h"

CUIObject::CUIObject(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
	: CGameObject{pDevice,pContext}
{}

CUIObject::CUIObject(const CUIObject& Prototype)
	: CGameObject{Prototype}
{}

HRESULT CUIObject::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CUIObject::Initialize(void* pArg)
{
	if(nullptr != pArg)
	{
		UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);

		m_fX = pDesc->fX;
		m_fY = pDesc->fY;
		m_fSizeX = pDesc->fSizeX;
		m_fSizeY = pDesc->fSizeY;
		m_fDepth = pDesc->fDepth;
		m_iCount = pDesc->m_iCount;
	}

	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	_uint   iNumViewports = {1};
	D3D11_VIEWPORT ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports,&ViewportDesc);

	m_pTransformCom->Set_Scaling(m_fSizeX,m_fSizeY,1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,XMVectorSet(m_fX - ViewportDesc.Width * 0.5f,-m_fY + ViewportDesc.Height * 0.5f,m_fDepth,1.f));

	XMStoreFloat4x4(&m_ViewMatrix,XMMatrixIdentity());

	/* 뷰스페이스 상의 화면에 보여줄 영역(뷰볼륨)을 설정한다. */
	XMStoreFloat4x4(&m_ProjMatrix,XMMatrixOrthographicLH(ViewportDesc.Width,ViewportDesc.Height,0.f,1.f));

	return S_OK;

}

void CUIObject::Priority_Update(_float fTimeDelta)
{}

void CUIObject::Update(_float fTimeDelta)
{}

void CUIObject::Late_Update(_float fTimeDelta)
{}

HRESULT CUIObject::Render()
{
	return S_OK;
}



void CUIObject::Free()
{
	__super::Free();
}