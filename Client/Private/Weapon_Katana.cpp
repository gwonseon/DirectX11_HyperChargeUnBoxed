#include "stdafx.h"
#include "..\Public\Weapon_Katana.h"

#include "GameInstance.h"
#include "Player.h"

CWeapon_Katana::CWeapon_Katana(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWeapon_Katana::CWeapon_Katana(const CWeapon_Katana& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CWeapon_Katana::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Katana::Initialize(void* pArg)
{
	KATANA_DESC* pDesc = static_cast<KATANA_DESC*>(pArg);

	m_pParentState = pDesc->pParentState;
	m_pSocketMatrix = pDesc->pSocketMatrix;
	m_eLevelID = pDesc->m_eLevelID;
	/* 추가적으로 초기화가 필요하다면 수행해준다. */

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_iViewState = pDesc->m_iViewState;

	Position = { -0.0599999f, 0.639999f, 0.0599998f };
	Scale = { 3.33 };
	Rotation = { 101.3,57.5,-68 };
	m_pTransformCom->Set_Scaling(Scale, Scale, 2.f);
	m_pTransformCom->Rotation(XMConvertToRadians(Rotation.x), XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(Position.x, Position.y, Position.z, 1.f));

	// 이펙트 생성
	for(int i = 0; i < 6; i++)
	{
		CKatana_Effect::EFFECT_KATANA_DESC pEffect{};
		pEffect.eLevel = m_eLevelID;
		pEffect.iEffectNumber = i;
		pEffect.bKatanaState = &m_bKatanaState;
		pEffect.pParentState = m_pParentState;
		m_pKatana_Effect[i] = static_cast<CKatana_Effect*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevelID, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Katana_Effect"), &pEffect));
		
	}
	

	return S_OK;
}

void CWeapon_Katana::Priority_Update(_float fTimeDelta)
{
	if(m_bKatanaState == true)
	{


	}
}

void CWeapon_Katana::Update(_float fTimeDelta)
{
	if (m_bKatanaState == true)
	{
		// 이전 월드 매트릭스 저장(3개 전까지 저장)
	/*m_PrevWorldMatrix[m_iStorePrevTiming] = m_WorldMatrix;*/
		m_pKatana_Effect[m_iStorePrevTiming]->Set_WorldMatrix(m_WorldMatrix);
		m_pKatana_Effect[m_iStorePrevTiming]->Set_BlendValue(0.8f);
		m_iStorePrevTiming++;
		if (m_iStorePrevTiming >= 6)
			m_iStorePrevTiming = 0;

		_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

		for (size_t i = 0; i < 3; i++)
			SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	
	
		m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));
	}
}



void CWeapon_Katana::Late_Update(_float fTimeDelta)
{
	if(m_bKatanaState == true)
	{
		
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
			return;
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_SHADOW, this)))
			return;
	}
}

HRESULT CWeapon_Katana::Render()
{
	if (m_bKatanaState == true)
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
#ifdef _DEBUG
		if (m_bDead)
			return S_OK;
		m_pColliderCom->Render();
#endif

	}
	return S_OK;
}

HRESULT CWeapon_Katana::Render_Shadow()
{
	_float4x4			ViewMatrix, ProjMatrix;

	_float fFar = m_pGameInstance->Get_CameraFar();
	_float4 fPlayerPos = m_pGameInstance->Get_PlayerPos();
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(fPlayerPos.x - 3.f, fPlayerPos.y + 10.f, fPlayerPos.z - 3.f, 1.f), XMVectorSet(fPlayerPos.x, fPlayerPos.y, fPlayerPos.z, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.f), (_float)1280.f / 720.f, 0.1f, fFar));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

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

HRESULT CWeapon_Katana::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(m_eLevelID ,TEXT("Prototype_Component_Model_Weapon8"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider_Sphere */
	CBounding_Sphere::BOUND_SPHERE_DESC			SphereDesc{};

	SphereDesc.fRadius = 0.7f;
	SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(m_eLevelID, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CWeapon_Katana::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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

CWeapon_Katana* CWeapon_Katana::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Katana* pInstance = new CWeapon_Katana(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Katana");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon_Katana::Clone(void* pArg)
{
	CWeapon_Katana* pInstance = new CWeapon_Katana(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CWeapon_Katana");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Katana::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
