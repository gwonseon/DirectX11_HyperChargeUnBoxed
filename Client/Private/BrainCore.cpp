#include "stdafx.h"
#include "..\Public\BrainCore.h"

#include "GameInstance.h"
#include "Layer.h"

CBrainCore::CBrainCore(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
	: CPlayer_Build{pDevice,pContext}
{}

CBrainCore::CBrainCore(const CBrainCore& Prototype)
	: CPlayer_Build{Prototype}
{}

HRESULT CBrainCore::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBrainCore::Initialize(void* pArg)
{
	BRAIN_CORE_DESC* pDesc = static_cast<BRAIN_CORE_DESC*>(pArg);
	m_iModelIndex = pDesc->iModelComponentIndex;
	m_eLevel = pDesc->eID;

	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if(FAILED(Add_Components()))
		return E_FAIL;
	m_fHp = 100.f;
	m_fEnergy = 100.f;
	m_bAffected = true;
	m_bDontDestroy = true;
	return S_OK;
}

void CBrainCore::Priority_Update(_float fTimeDelta)
{
	m_vecPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_fSound = m_pGameInstance->Sound_Cal(m_vecPos);
	if(m_fSound != 0.f)
		m_fSound *= 0.2f;
	m_pGameInstance->PlaySoundW(L"FE_Base_Braincore_Bubble_01.wav",Engine::CHANNELID::BRAINCORE,m_fSound);

}

void CBrainCore::Update(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	if(m_fEnergy <= 50.f && m_bOnce == false)
	{
		m_bOnce = true;
		m_pGameInstance->PlaySoundW(L"FE_CoreBeeingAttacked.wav",Engine::CHANNELID::SOUND_EFFECT,m_fSound);
	}

	if(m_fEnergy > 50.f)
	{
		m_bOnce = false;
	}
	if(m_fHp <= 50.f)
	{
		m_pGameInstance->StopSound(TUTORIAL);
		m_pGameInstance->PlaySoundW(L"FE_VO_Blaze_Coredefend_01.wav",Engine::CHANNELID::TUTORIAL,m_fSound);

	}


}

void CBrainCore::Late_Update(_float fTimeDelta)
{
	if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND,this)))
		return;
	if(m_eLevel == LEVEL_YARD || m_eLevel == LEVEL_GAMEPLAY)
	{

		if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_HEIGHT,this)))
			return;
	}

}

HRESULT CBrainCore::Render()
{

	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for(size_t i = 0; i < iNumMeshes; i++)
	{
		if(FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom,i,aiTextureType_DIFFUSE,0,"g_DiffuseTexture")))
			return E_FAIL;

		if(FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	#ifdef _DEBUG
	if(m_bDead)
		return S_OK;
	m_pColliderCom->Render();
	#endif
	return S_OK;
}


HRESULT CBrainCore::Render_Height()
{

	if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom,"g_WorldMatrix")))
		return E_FAIL;

	_float4x4			ViewMatrix,ProjMatrix;


	CLayer* pPlayerLayer = (m_pGameInstance->Find_Layer(m_eLevel,TEXT("Layer_Player")));
	CPlayer* pPlayer = static_cast<CPlayer*>(pPlayerLayer->Get_GameObject_List().front());
	_vector PlayerPos = pPlayer->Get_Position();
	_matrix			matView = XMMatrixIdentity();
	matView.r[0] = XMVectorSet(1.f,0.f,0.f,0.f);
	matView.r[1] = XMVectorSet(0.f,0.f,1.f,0.f);
	matView.r[2] = XMVectorSet(0.f,-1.f,0.f,0.f);
	matView.r[3] = XMVectorSet(XMVectorGetX(PlayerPos),XMVectorGetY(PlayerPos) + 6.f,XMVectorGetZ(PlayerPos),1.f);

	XMStoreFloat4x4(&ViewMatrix,XMMatrixInverse(nullptr,matView));
	XMStoreFloat4x4(&ProjMatrix,XMMatrixOrthographicLH(200.f,200.f,0.f,30.f));

	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",&ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",&ProjMatrix)))
		return E_FAIL;

	_float fFar = m_pGameInstance->Get_CameraFar();
	if(FAILED(m_pShaderCom->Bind_RawValue("g_fFar",&fFar,sizeof(float))))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for(size_t i = 0; i < iNumMeshes; i++)
	{
		if(FAILED(m_pShaderCom->Begin(6))) // ¹«Á¶°Ç ±×¸²
			return E_FAIL;
		m_pModelCom->Render(i);
	}

	return S_OK;
}


HRESULT CBrainCore::Add_Components()
{
	if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"),reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	const _wstring Model_Component = TEXT("Prototype_Component_Model_Environment");
	const _wstring Model_Component_Result = Model_Component + to_wstring(m_iModelIndex);
	/* For.Com_Model */
	if(FAILED(__super::Add_Component(m_eLevel,Model_Component_Result,
		TEXT("Com_Model"),reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_AABB::BOUND_AABB_DESC		AABBDesc{};
	AABBDesc.vExtents = _float3(0.8f,2.5f,0.8f);
	AABBDesc.vCenter = _float3(0.f,AABBDesc.vExtents.y,0.f);
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"),reinterpret_cast<CComponent**>(&m_pColliderCom),&AABBDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CBrainCore::Bind_ShaderResources()
{
	if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom,"g_WorldMatrix")))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CBrainCore* CBrainCore::Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
{
	CBrainCore* pInstance = new CBrainCore(pDevice,pContext);

	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBrainCore");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CGameObject* CBrainCore::Clone(void* pArg)
{
	CBrainCore* pInstance = new CBrainCore(*this);
	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBrainCore");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CBrainCore::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}