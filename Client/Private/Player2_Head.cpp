#include "stdafx.h"
#include "Player2_Head.h"
#include "GameInstance.h"

namespace {
constexpr _float3 HEADPOS = {0.9,0.f,0.f};
constexpr _float3 HEADROTATION = {65.3996f,34.5f,-45.2999f};
constexpr _float3 HEADSCALE = {3.f,3.f,3.f};
}
CPlayer2_Head::CPlayer2_Head(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
	: CPlayer2_Parts{pDevice,pContext}
{}

CPlayer2_Head::CPlayer2_Head(const CPlayer2_Head & Prototype)
	: CPlayer2_Parts{Prototype}
{}

HRESULT CPlayer2_Head::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer2_Head::Initialize(void * pArg)
{
	PLAYER2_HEAD_DESC* pDesc = static_cast<PLAYER2_HEAD_DESC*>(pArg);
	m_pSocketMatrix = pDesc->pSocketMatrix;
	m_eLevelID = pDesc->eLevelID;
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if(FAILED(Add_Components()))
		return E_FAIL;
	m_pTransformCom->Set_Scaling(HEADSCALE.x,HEADSCALE.y,HEADSCALE.z);
	m_pTransformCom->Rotation(XMConvertToRadians(HEADROTATION.x),XMConvertToRadians(HEADROTATION.y),XMConvertToRadians(HEADROTATION.z));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,XMVectorSet(HEADPOS.x,HEADPOS.y,HEADPOS.z,1.f));

	return S_OK;
}

void CPlayer2_Head::Priority_Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	for(size_t i = 0; i < 3; i++)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	XMStoreFloat4x4(&m_WorldMatrix,m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
}

void CPlayer2_Head::Update(_float fTimeDelta)
{
}

void CPlayer2_Head::Late_Update(_float fTimeDelta)
{
	if(m_bTPSState == true)
	{
		if(m_eLevelID == LEVEL_GAMEPLAY)
		{
			if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND,this)))
				return;
		}
		else
		{
			if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_LAST,this)))
				return;
		}
	}
}

HRESULT CPlayer2_Head::Render()
{
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	for(size_t i = 0; i < iNumMeshes; i++)
	{
		if(FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom,i,aiTextureType_DIFFUSE,0,"g_DiffuseTexture")))
			return E_FAIL;

		if(FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CPlayer2_Head::Add_Components()
{
	/* For.Com_Shader */
	if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"),reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if(FAILED(__super::Add_Component(m_eLevelID,TEXT("Prototype_Component_Model_Character1"),
		TEXT("Com_Model"),reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer2_Head::Bind_ShaderResources()
{
	if(FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix",&m_WorldMatrix)))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	_float fFar = m_pGameInstance->Get_CameraFar();
	if(FAILED(m_pShaderCom->Bind_RawValue("g_fFar",&fFar,sizeof(float))))
		return E_FAIL;

	return S_OK;
}

CPlayer2_Head * CPlayer2_Head::Create(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
{
	CPlayer2_Head* pInstance = new CPlayer2_Head(pDevice,pContext);
	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer2_Head");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer2_Head::Clone(void * pArg)
{
	CPlayer2_Head* pInstance = new CPlayer2_Head(*this);

	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer2_Head");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer2_Head::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
