#include "stdafx.h"
#include "Camera_Tool.h"
#include "GameInstance.h"

CCamera_Tool::CCamera_Tool(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
	: CCamera{pDevice,pContext}
{}

CCamera_Tool::CCamera_Tool(const CCamera_Tool & Prototype)
	: CCamera{Prototype}
{}

HRESULT CCamera_Tool::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Tool::Initialize(void * pArg)
{
	CAMERA_TOOL_DESC* pDesc = static_cast<CAMERA_TOOL_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;
	m_eLevelID = pDesc->eLevel;
	m_bMouseLock = false;  // 마우스 멈춤
	if(FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
	m_pGameInstance->Set_CameraFar(pDesc->fFar);


	return S_OK;
}

void CCamera_Tool::Priority_Update(_float fTimeDelta)
{
	if(m_bMouseLock == false)
	{
		_long   MouseMove = {0};
		_long   MouseMoveY = {0};
		if(MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
			m_pTransformCom->Turn(XMVectorSet(0.f,1.f,0.f,0.f),fTimeDelta * MouseMove * m_fMouseSensor);
		MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y);
		if(MouseMoveY != 0) // y축 회전
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT),fTimeDelta * MouseMoveY * m_fMouseSensor);
	}
}

void CCamera_Tool::Update(_float fTimeDelta)
{
	if(GetKeyState('S') & 0x8000)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}
	if(GetKeyState('W') & 0x8000)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if(GetKeyState('A') & 0x8000)
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}
	if(GetKeyState('D') & 0x8000)
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}
	// 마우스 고정
	if(GetAsyncKeyState(VK_MBUTTON) & 0x0001)
	{
		if(m_bMouseLock == true)
			m_bMouseLock = false;
		else
			m_bMouseLock = true;

	}
	ShowCursor(TRUE);
	__super::Priority_Update(fTimeDelta);
}

void CCamera_Tool::Late_Update(_float fTimeDelta)
{}

HRESULT CCamera_Tool::Render()
{
	return S_OK;
}

CCamera_Tool * CCamera_Tool::Create(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
{
	CCamera_Tool* pInstance = new CCamera_Tool(pDevice,pContext);

	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Tool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamera_Tool::Clone(void * pArg)
{
	CCamera_Tool* pInstance = new CCamera_Tool(*this);

	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCamera_Tool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Tool::Free()
{
	__super::Free();
}
