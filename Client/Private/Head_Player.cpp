
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

	/* 추가적으로 초기화가 필요하다면 수행해준다. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	m_iViewState = pDesc->m_iViewState;

	m_pTransformCom->Set_Scaling(3.f, 3.f, 3.f);
	//m_pTransformCom->Rotation(XMConvertToRadians(90.f), XMConvertToRadians(90.f), XMConvertToRadians(270.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -0.5f, 0.5f, 1.f));

	return S_OK;
}

void CHead_Player::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CHead_Player::Update(_float fTimeDelta)
{
}

void CHead_Player::Late_Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CHead_Player::Render()
{
	/*if(m_bTPSState == true)
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

	}*/
	return S_OK;
}

HRESULT CHead_Player::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Character1"),
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
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
