#include "stdafx.h"
#include "..\Public\Dead_Model.h"

#include "GameInstance.h"


CDead_Model::CDead_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDead_Model::CDead_Model(const CDead_Model& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CDead_Model::Initialize_Prototype()
{



	return S_OK;
}

HRESULT CDead_Model::Initialize(void* pArg)
{
	DEADMODEL_DESC* pDesc = static_cast<DEADMODEL_DESC*>(pArg);
	m_eLevel = pDesc->eLevel;
	m_vecPosition = pDesc->vecPos;
	m_eModelType = pDesc->eModelType;

	m_matWorld = pDesc->matWorld;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	_vector Look = XMLoadFloat4x4(&m_matWorld).r[2];
	_vector Up = XMLoadFloat4x4(&m_matWorld).r[1];
	_vector Right = XMLoadFloat4x4(&m_matWorld).r[0];


	m_pTransformCom->Set_State(CTransform::STATE_LOOK, Look);
	m_pTransformCom->Set_State(CTransform::STATE_UP, Up);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, Right);
	
	m_fPower = 30.f;
	return S_OK;
}

void CDead_Model::Priority_Update(_float fTimeDelta)
{
	switch (m_eModelType)
	{
	case Client::CDead_Model::DEAD_TANK_BODY:
		m_vecPosition = XMVectorSetY(m_vecPosition, 0.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);
		break;
	case Client::CDead_Model::DEAD_TANK_TURRET:
		m_fPower -= fTimeDelta * 20.f;
		m_vecPosition += XMVectorSet(0.f, 1.f, 0.f, 0.f) * fTimeDelta * m_fPower;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);

		break;
	case Client::CDead_Model::DEAD_MODEL_END:
		break;
	default:
		break;
	}


}

void CDead_Model::Update(_float fTimeDelta)
{
	if (m_eModelType == DEAD_TANK_BODY)
	{
		m_fDissolve += fTimeDelta;
	}
	if (m_fDissolve > 1.f)
		m_bDead = true;

	if (m_bDeadState == true)
		m_fDissolve += fTimeDelta;

	if (m_fLifeTime >= 2.f)
		m_bDeadState = true;

	m_fLifeTime += fTimeDelta;

}

void CDead_Model::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CDead_Model::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(14)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}


	return S_OK;
}

HRESULT CDead_Model::Add_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_Dissolved"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	const _wstring Model_Component = TEXT("Prototype_Component_Model_Effect");
	const _wstring Model_Component_Result = Model_Component + to_wstring(m_eModelType);
	/* For.Com_Model */
	;
	if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDead_Model::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", static_cast<_uint>(1))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolve_Value", &m_fDissolve, sizeof(float))))
		return E_FAIL;
	_float fFar = m_pGameInstance->Get_CameraFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
		return E_FAIL;

	return S_OK;
}
 
CDead_Model* CDead_Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDead_Model* pInstance = new CDead_Model(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDead_Model");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDead_Model::Clone(void* pArg)
{
	CDead_Model* pInstance = new CDead_Model(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CDead_Model");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDead_Model::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);

}
