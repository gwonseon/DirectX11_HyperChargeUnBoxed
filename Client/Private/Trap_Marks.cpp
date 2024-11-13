#include "stdafx.h"
#include "..\Public\Trap_Marks.h"

#include "GameInstance.h"

CTrap_Marks::CTrap_Marks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Build{ pDevice, pContext }
{
}

CTrap_Marks::CTrap_Marks(const CTrap_Marks& Prototype)
	: CPlayer_Build{ Prototype }
{
}

HRESULT CTrap_Marks::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrap_Marks::Initialize(void* pArg)
{
	TRAP_MARKS_DESC* pDesc = static_cast<TRAP_MARKS_DESC*>(pArg);
	m_pPlayer = pDesc->pPlayer;
	m_iModel_Idx = pDesc->iModel_Idx;
	m_eLevel = pDesc->eID;
	m_eType = pDesc->eType;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;
	
	CTrap_Bricks::TRAP_BRICKS_DESC Bricks_Desc{};
	Bricks_Desc.eID = m_eLevel;
	Bricks_Desc.fScale = { 5.f,5.f,5.f };
	Bricks_Desc.pPlayer = m_pPlayer;
	Bricks_Desc.m_bBuild = &m_bBuild;
	Bricks_Desc.fPosition = pDesc->fPosition;
	Bricks_Desc.m_bBuild_PreView = &m_bBuild_PreView;

	if (m_eType == BRICKS_TRAP)
		Bricks_Desc.iModel_Idx = 1; // 레고 트랩
	if (m_eType == TANK_TRAP)
		Bricks_Desc.iModel_Idx = 9; // 탱크 트랩


	m_pBricks = static_cast<CTrap_Bricks*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel, TEXT("Layer_Trap_Shield"), TEXT("Prototype_GameObject_TrapBricks"), &Bricks_Desc));
	m_bAffected = false;
	m_bDraw = true;
	return S_OK;
}

void CTrap_Marks::Priority_Update(_float fTimeDelta)
{
	if(m_bBuild == false)
	{
		// 위치 비교해서 트랩 설치가 가능한지 확인
		m_vecPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector m_vecPlayerPos = m_pPlayer->Get_Position();
		m_fDistance = m_pTransformCom->Cal_Distance_vec(m_vecPlayerPos, m_vecPos);
		if (m_fDistance <= 150.f && *m_pPlayer->Get_BuildMode() == true)
			m_bBuild_PreView = true;
		else
			m_bBuild_PreView = false;
	}
	else 
	{
		// 트랩 설치 확정
		m_bBuild_PreView = false;
		m_bDraw = false;
	}

}

void CTrap_Marks::Update(_float fTimeDelta)
{
}

void CTrap_Marks::Late_Update(_float fTimeDelta)
{
	if (m_bDraw == true)
	{
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
			return;
	}
}

HRESULT CTrap_Marks::Render()
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

HRESULT CTrap_Marks::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	const _wstring Model_Component = TEXT("Prototype_Component_Model_Trap");
	_wstring Model_Component_Result{};
	if(m_eType == BRICKS_TRAP)
		  Model_Component_Result = Model_Component + to_wstring(11);
	if (m_eType == TANK_TRAP)
		  Model_Component_Result = Model_Component + to_wstring(10);
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrap_Marks::Bind_ShaderResources()
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

CTrap_Marks* CTrap_Marks::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrap_Marks* pInstance = new CTrap_Marks(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrap_Marks");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTrap_Marks::Clone(void* pArg)
{
	CTrap_Marks* pInstance = new CTrap_Marks(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CTrap_Marks");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTrap_Marks::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBricks);
}
