#include "stdafx.h"
#include "..\Public\Energy_Machine.h"

#include "GameInstance.h"

CEnergy_Machine::CEnergy_Machine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Build{ pDevice, pContext }
{
}

CEnergy_Machine::CEnergy_Machine(const CEnergy_Machine& Prototype)
	: CPlayer_Build{ Prototype }
{
}

HRESULT CEnergy_Machine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEnergy_Machine::Initialize(void* pArg)
{
	ENERGYMACHINE_DESC* pDesc = static_cast<ENERGYMACHINE_DESC*>(pArg);
	m_eLevel = pDesc->eID;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CEnergy_Machine::Priority_Update(_float fTimeDelta)
{
}

void CEnergy_Machine::Update(_float fTimeDelta)
{
}

void CEnergy_Machine::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CEnergy_Machine::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CEnergy_Machine::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	const _wstring Model_Component = TEXT("Prototype_Component_Model_Environment");
	const _wstring Model_Component_Result = Model_Component + to_wstring(209);
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnergy_Machine::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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

CEnergy_Machine* CEnergy_Machine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnergy_Machine* pInstance = new CEnergy_Machine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnergy_Machine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEnergy_Machine::Clone(void* pArg)
{
	CEnergy_Machine* pInstance = new CEnergy_Machine(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CEnergy_Machine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEnergy_Machine::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
