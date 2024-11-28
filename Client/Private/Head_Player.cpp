
#include "stdafx.h"
#include "..\Public\Head_Player.h"

#include "GameInstance.h"
#include "Player.h"

CHead_Player::CHead_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CHead_Player::CHead_Player(const CHead_Player& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CHead_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHead_Player::Initialize(void* pArg)
{
	HEADPLAYER_DESC* pDesc = static_cast<HEADPLAYER_DESC*>(pArg);

	m_pParentState = pDesc->pParentState;
	m_pSocketMatrix = pDesc->pSocketMatrix;
	m_iViewState = pDesc->m_iViewState;
	m_iWeaponState =  pDesc->m_iWeaponState;
	m_eLevelID = pDesc->m_eLevelID;
	/* 추가적으로 초기화가 필요하다면 수행해준다. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;
	Position = { 0.9, 0.f, 0.f };
	Rotation = { 65.3996f, 34.5f, -45.2999f };
	m_pTransformCom->Set_Scaling(3.f, 3.f, 3.f);
	m_pTransformCom->Rotation(XMConvertToRadians(Rotation.x), XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(Position.x, Position.y, Position.z, 1.f));
	return S_OK;
}

void CHead_Player::Priority_Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	for (size_t i = 0; i < 3; i++)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
}

void CHead_Player::Update(_float fTimeDelta)
{
	if (m_bTPSState == true)
	{
		_long MouseMoveY = { 0 };
		if (MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			if (m_fAngle_Y <= 80.f && m_fAngle_Y >= -80.f)
				m_fAngle_Y += fTimeDelta * MouseMoveY * 4.f;
			if (m_fAngle_Y > 60.f)
				m_fAngle_Y = 60.f;
			if (m_fAngle_Y < -60.f)
				m_fAngle_Y = -60.f;
		}
		//	m_pTransformCom->Rotation(XMConvertToRadians(m_fAngle_Y), XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
		m_pTransformCom->Turn(false, false, true, fTimeDelta * MouseMoveY * -0.1f);
	}
	else
	{
		_long MouseMoveY = { 0 };
		if (MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			if (m_fAngle_Y <= 80.f && m_fAngle_Y >= -80.f)
				m_fAngle_Y += fTimeDelta * MouseMoveY * 4.f;
			if (m_fAngle_Y > 60.f)
				m_fAngle_Y = 60.f;
			if (m_fAngle_Y < -60.f)
				m_fAngle_Y = -60.f;
		}
		m_pTransformCom->Turn(false, false, true, fTimeDelta * MouseMoveY * 0.1f);
	}
}

void CHead_Player::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_LAST, this)))
		return;
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_SHADOW, this)))
		return;
}

HRESULT CHead_Player::Render()
{
	if(m_bTPSState == true)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;

			m_pModelCom->Render(i);
		}

	}
	return S_OK;
}

HRESULT CHead_Player::Render_Shadow()
{
	_float4x4			ViewMatrix, ProjMatrix;
	_float fFar = m_pGameInstance->Get_CameraFar();
	_float4 fPlayerPos = m_pGameInstance->Get_PlayerPos();
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(400.f - 6.f, 60.f, 400.f - 6.f, 1.f), XMVectorSet(400.f, fPlayerPos.y, 400.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.f), (_float)1280.f / 720.f, 0.1f, fFar));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(7)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CHead_Player::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eLevelID , TEXT("Prototype_Component_Model_Character1"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHead_Player::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	_float fFar = m_pGameInstance->Get_CameraFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
		return E_FAIL;


	return S_OK;
}


CHead_Player* CHead_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHead_Player* pInstance = new CHead_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHead_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHead_Player::Clone(void* pArg)
{
	CHead_Player* pInstance = new CHead_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CHead_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHead_Player::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
