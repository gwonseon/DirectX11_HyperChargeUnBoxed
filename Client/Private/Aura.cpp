#include "stdafx.h"
#include "..\Public\Aura.h"

#include "GameInstance.h"



CAura::CAura(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CAura::CAura(const CAura& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CAura::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAura::Initialize(void* pArg)
{
	AURA_DESC* pDesc = static_cast<AURA_DESC*>(pArg);
	m_eLevel = pDesc->eLevel;
	m_vecPos = pDesc->vecPos;
	m_bInteration = pDesc->bInteration;
	m_fScale = pDesc->fScale;
	m_eType = pDesc->eType;
//	if(m_eType == ITEM_AURA)
	{
		m_fScale.x += 6.f;
		m_fScale.y += 6.f;
		m_fScale.z += 6.f;
	}
	
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vecPos += XMVectorSet(0.f, 1.f, 0.f, 0.f) * 1.5f;
	m_pTransformCom->Set_Scaling(pDesc->fScale.x , pDesc->fScale.y , pDesc->fScale.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPos);


	return S_OK;
}

void CAura::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
		return;
	_vector Axis = { 0.f, 1.f, 0.f };
	m_pTransformCom->Turn(Axis, -fTimeDelta );
}

void CAura::Update(_float fTimeDelta)
{
	if (m_bDead)
		return;
	if(m_eType == ITEM_AURA)
	{
		if (*m_bInteration == true)
		{
			m_pTransformCom->Set_Scaling(m_fScale.x + 2.f, m_fScale.y + 2.f, m_fScale.z + 2.f);
		}
		else
		{
			m_pTransformCom->Set_Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
		}
	}
	m_fUVMove += fTimeDelta;
}

void CAura::Late_Update(_float fTimeDelta)
{
	if (m_bDead)
		return;

	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
		return;


}

HRESULT CAura::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(17)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CAura::Add_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_Dissolved"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	const _wstring Model_Component = TEXT("Prototype_Component_Model_Effect");
	const _wstring Model_Component_Result = Model_Component + to_wstring(9);
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CAura::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fFar = m_pGameInstance->Get_CameraFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTex_Move", &m_fUVMove, sizeof(float))))
		return E_FAIL;

	_float fDissolve = 0.4f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolve_Value", &fDissolve, sizeof(float))))
		return E_FAIL;
	

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", static_cast<_uint>(10))))
		return E_FAIL;

	
	return S_OK;
}

CAura* CAura::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAura* pInstance = new CAura(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAura");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAura::Clone(void* pArg)
{
	CAura* pInstance = new CAura(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CAura");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAura::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
