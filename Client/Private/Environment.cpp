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

	return S_OK;
}

void CEnvironment::Priority_Update(_float fTimeDelta)
{
}

void CEnvironment::Update(_float fTimeDelta)
{
//	MovePos(fTimeDelta);
	if (m_bDead)
		return;

/*	Picking();	*/		    
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

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}
	return S_OK;
}

void CEnvironment::Picking()
{
	//if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		//// 피킹
		//_float3 fMousePos = m_pGameInstance->Get_MousePos_NDC(g_hWnd, g_iWinSizeX, g_iWinSizeY);
		//XMMATRIX invProj = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_PROJ);
		//XMMATRIX invView = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW);
		//XMVECTOR RayPos, RayDir;
		//m_pGameInstance->Get_MouseRayDirection(fMousePos, invProj, invView, &RayPos, &RayDir);
		//// RayDir을 정규화하고 결과를 다시 RayDir에 저장
		//RayDir = XMVector3Normalize(RayDir);
		//CVIBuffer_Terrain* pVIBuffer_Terrain = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(m_eLevel, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));
		//const _float3* VtxPos = pVIBuffer_Terrain->Get_VtxPos();  // _float3 배열의 시작 주소 반환
		//_uint VtxCountX = pVIBuffer_Terrain->Get_VtxCountX();
		//_uint VtxCountZ = pVIBuffer_Terrain->Get_VtxCountZ();
		//// Picking_Terrain 함수 호출에 정규화된 RayDir 사용
		//m_fPickingPos = m_pGameInstance->Picking_Terrain(RayPos, RayDir, VtxPos, VtxCountX, VtxCountZ);
		//XMFLOAT4 m_fNewPickingPos = { m_fPickingPos.x, m_fPickingPos.y, m_fPickingPos.z, 1.0f };
		//_fvector m_vPickingPos = XMLoadFloat4(&m_fNewPickingPos);
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPickingPos);
		//cout << "final X : " << m_fPickingPos.x << "Z : " << m_fPickingPos.z << "Y : " << m_fPickingPos.y << endl;

	}
}

HRESULT CEnvironment::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Shader_VtxMesh"),
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
