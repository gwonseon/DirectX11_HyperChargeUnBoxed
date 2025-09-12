#include "stdafx.h"
#include "Player2_Body.h"
#include "GameInstance.h"


CPlayer2_Body::CPlayer2_Body(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
	: CPlayer2_Parts{pDevice,pContext}
{}

CPlayer2_Body::CPlayer2_Body(const CPlayer2_Body & Prototype)
	: CPlayer2_Parts{Prototype}
{}

HRESULT CPlayer2_Body::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer2_Body::Initialize(void * pArg)
{
	PLAYER2_BODY_DESC* pDesc = static_cast<PLAYER2_BODY_DESC*>(pArg);
	m_bAttackState = pDesc->m_bAttackState;
	m_eLevelID = pDesc->eLevelID;
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if(FAILED(Add_Components()))
		return E_FAIL;
	m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Idle_Unarmed,true);
	m_pModelCom->Set_Animation_UpperBody(PLAYER_ANIM_FiringAnimation8_Base,true);

	return S_OK;
}

void CPlayer2_Body::Priority_Update(_float fTimeDelta)
{}

void CPlayer2_Body::Update(_float fTimeDelta)
{}

void CPlayer2_Body::Late_Update(_float fTimeDelta)
{}

HRESULT CPlayer2_Body::Render()
{
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for(size_t i = 0; i < iNumMeshes; i++)
	{
		if(FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom,i,aiTextureType_DIFFUSE,0,"g_DiffuseTexture")))
			return E_FAIL;

		if(FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom,i,"g_BoneMatrices")))
			return E_FAIL;

		if(FAILED(m_pShaderCom->Begin(m_iShaderPassNum)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	#ifdef _DEBUG
	m_pColliderCom->Render();
	#endif
	return S_OK;
}
HRESULT CPlayer2_Body::Add_Components()
{
	/* For.Com_Shader */
	if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"),reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	/* For.Com_Model */
	if(FAILED(__super::Add_Component(m_eLevelID,TEXT("Prototype_Component_Model_Anim7"),
		TEXT("Com_Model"),reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;
	/* For.Com_Collider_AABB */
	CBounding_AABB::BOUND_AABB_DESC		AABBDesc{};
	AABBDesc.vExtents = _float3(0.5f,1.5f,0.5f);
	AABBDesc.vCenter = _float3(0.f,AABBDesc.vExtents.y + 1.25f,0.f);
	if(FAILED(__super::Add_Component(m_eLevelID,TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"),reinterpret_cast<CComponent**>(&m_pColliderCom),&AABBDesc)))
		return E_FAIL;
	return S_OK;
}
HRESULT CPlayer2_Body::Bind_ShaderResources()
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
const _float4x4* CPlayer2_Body::Get_SocketMatrix(const _char* pBoneName)
{
	return m_pModelCom->Get_BoneMatrix(pBoneName);
}
CPlayer2_Body * CPlayer2_Body::Create(ID3D11Device * pDevice,ID3D11DeviceContext * pContext)
{
	CPlayer2_Body* pInstance = new CPlayer2_Body(pDevice,pContext);
	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer2_Body");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CGameObject * CPlayer2_Body::Clone(void * pArg)
{
	CPlayer2_Body* pInstance = new CPlayer2_Body(*this);
	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer2_Body");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CPlayer2_Body::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
