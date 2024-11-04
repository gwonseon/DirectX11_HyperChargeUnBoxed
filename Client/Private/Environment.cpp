#include "stdafx.h"
#include "..\Public\Environment.h"

#include "GameInstance.h"
#include "VIBuffer_Terrain.h"

CEnvironment::CEnvironment(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEnvironment::CEnvironment(const CEnvironment& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CEnvironment::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEnvironment::Initialize(void* pArg)
{
	ENVIRONMENT_DESC* pDesc = static_cast<ENVIRONMENT_DESC*>(pArg);
	ENVIRONMENT_DESC		Desc{};
	Desc.eID = pDesc->eID;
	Desc.fPosition = pDesc->fPosition;
	Desc.fScale = pDesc->fScale;
	m_fScale = pDesc->fScale;
	XMFLOAT4 Position = { pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.0f };
	_fvector vPosition = XMLoadFloat4(&Position);
	m_iModelIndex = pDesc->iModelComponentIndex;
	m_eLevel = pDesc->eID;
	Desc.eID = m_eLevel;
	Desc.fRotationPerSec = 2.f;
	
	
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom->Set_Scaling(pDesc->fScale.x, pDesc->fScale.y, pDesc->fScale.z);


#ifdef _DEBUG
	if(m_eLevel == LEVEL_IMGUI)
	{
		// 콜리전박스 초기값 세팅
		CCollisionBox::COLLISIONBOX_DESC CollisionDesc{};
		CollisionDesc.iImGuiMode = pDesc->iImGuiMode;
		CollisionDesc.eLevel = LEVEL_IMGUI;
		m_pCollisionBox = m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_IMGUI, TEXT("Layer_Collision"), TEXT("Prototype_GameObject_Collision_Box"), &CollisionDesc);
		m_fCollisionBoxScale = m_fScale;
		m_vecCollisionBoxPos = vPosition;
	}
#endif
	return S_OK;
}

void CEnvironment::Priority_Update(_float fTimeDelta)
{
	Picking();


}

void CEnvironment::Update(_float fTimeDelta)
{
	if (m_bDead)
	{
		#ifdef _DEBUG

		if (m_eLevel == LEVEL_IMGUI)
			static_cast<CCollisionBox*>(m_pCollisionBox)->Set_Dead();
		#endif

		return;
	}
}

void CEnvironment::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CEnvironment::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
			return E_FAIL;
		if (m_iModelIndex == 84 + ENVIRONMENT_EA ||
			m_iModelIndex == 91 + ENVIRONMENT_EA ||
			m_iModelIndex == 116 + ENVIRONMENT_EA||
			m_iModelIndex == 142 + ENVIRONMENT_EA||
			m_iModelIndex == 143 + ENVIRONMENT_EA||
			m_iModelIndex == 152 + ENVIRONMENT_EA)
		{
			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;
		}

		m_pModelCom->Render(i);
	}


	return S_OK;
}

void CEnvironment::Picking()
{
#ifdef _DEBUG
	if (m_eLevel == LEVEL_IMGUI)
	{
		static_cast<CCollisionBox*>(m_pCollisionBox)->Set_Position(m_vecCollisionBoxPos); // 박스 중심 위치
		static_cast<CCollisionBox*>(m_pCollisionBox)->Set_PickingCheck(m_bChecking);	// 박스 선택 됐는지 체크
		static_cast<CCollisionBox*>(m_pCollisionBox)->Set_Scale(m_fCollisionBoxScale);	// 박스 사이즈 
		static_cast<CCollisionBox*>(m_pCollisionBox)->Set_ImGuiMode(m_iCurrentImGuiMode);
	}
#endif
}

HRESULT CEnvironment::Add_Components()
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	
	const _wstring Model_Component = TEXT("Prototype_Component_Model_Environment");
	const _wstring Model_Component_Result = Model_Component + to_wstring(m_iModelIndex);
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnvironment::Bind_ShaderResources()
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

CEnvironment* CEnvironment::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnvironment* pInstance = new CEnvironment(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnvironment");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEnvironment::Clone(void* pArg)
{
	CEnvironment* pInstance = new CEnvironment(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
 		MSG_BOX("Failed to Created : CEnvironment");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnvironment::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);


}
