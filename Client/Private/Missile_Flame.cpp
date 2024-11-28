#include "stdafx.h"
#include "..\Public\Missile_Flame.h"

#include "GameInstance.h"
CMissile_Flame::CMissile_Flame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMissile_Flame::CMissile_Flame(const CMissile_Flame& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CMissile_Flame::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMissile_Flame::Initialize(void* pArg)
{
	MISSILE_FLAME_DESC* pDesc = static_cast<MISSILE_FLAME_DESC*>(pArg);
	m_eLevel = pDesc->eLevel;
	m_vecPos = pDesc->vecPos;
	m_iTextureNum = pDesc->iTexNum;
	m_bDraw = pDesc->bDraw;
	m_matWorld = pDesc->matWorld;
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaling(pDesc->fScale.x + 5.f, pDesc->fScale.y + 5.f, pDesc->fScale.z + 5.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *m_vecPos);
	
	return S_OK;
}

void CMissile_Flame::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *m_vecPos);
}

void CMissile_Flame::Update(_float fTimeDelta)
{

}

void CMissile_Flame::Late_Update(_float fTimeDelta)
{

	if(*m_bDraw == true)
	{
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
			return;
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
			return;
	}
}

HRESULT CMissile_Flame::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(13)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;

}

HRESULT CMissile_Flame::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	const _wstring Model_Component = TEXT("Prototype_Component_Model_Effect");
	const _wstring Model_Component_Result = Model_Component + to_wstring(6);
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMissile_Flame::Bind_ShaderResources()
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


	return S_OK;
}

CMissile_Flame* CMissile_Flame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMissile_Flame* pInstance = new CMissile_Flame(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMissile_Flame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMissile_Flame::Clone(void* pArg)
{
	CMissile_Flame* pInstance = new CMissile_Flame(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CMissile_Flame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMissile_Flame::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
