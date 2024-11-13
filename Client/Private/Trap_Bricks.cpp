#include "stdafx.h"
#include "..\Public\Trap_Bricks.h"

#include "GameInstance.h"
#include "Broken_Bricks.h"
CTrap_Bricks::CTrap_Bricks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Build{ pDevice, pContext }
{
}

CTrap_Bricks::CTrap_Bricks(const CTrap_Bricks& Prototype)
	: CPlayer_Build{ Prototype }
{
}

HRESULT CTrap_Bricks::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CTrap_Bricks::Initialize(void* pArg)
{
	TRAP_BRICKS_DESC* pDesc = static_cast<TRAP_BRICKS_DESC*>(pArg);
	m_pPlayer = pDesc->pPlayer;
	m_iModel_Idx = pDesc->iModel_Idx;
	m_eLevel = pDesc->eID;
	m_bBuild = pDesc->m_bBuild;
	m_bBuild_PreView = pDesc->m_bBuild_PreView;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	
	if (FAILED(Add_Components()))
		return E_FAIL;


	m_fEnergy = 0.f;
	m_bDontDestroy = true;
	m_bDraw = false;
	m_bOnce = false;
	if (m_iModel_Idx == 1) // 레고 트랩
	{
		m_fHp = 100.f;
		m_iCoin = 100;
	}
	if (m_iModel_Idx == 9) // 탱크 트랩
	{
		m_fHp = 70.f;
		m_iCoin = 70;
	}
	
	return S_OK;
}

void CTrap_Bricks::Priority_Update(_float fTimeDelta)
{
	
	// 파괴되었을 때
	if (m_bKnockdown == true)
	{
		m_bAffected = false;
		*m_bBuild = false;
		*m_bBuild_PreView = false;

		if (m_bOnce == false && m_iModel_Idx == 1)
		{
			_float3 fPos{};
			XMStoreFloat3(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CBroken_Bricks::PLAYER_BUILD_DESC pDesc{};
			pDesc.fPosition = fPos;
			pDesc.fSpeedPerSec = 3.f;
			pDesc.eID = m_eLevel;
			pDesc.fScale = { 4.f,4.f,4.f };
			m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel, TEXT("Layer_Broken"), TEXT("Prototype_GameObject_broken"), &pDesc);

			m_bOnce = true;
		}

	
	}
}

void CTrap_Bricks::Update(_float fTimeDelta)
{
	if (m_bDead)
		return;
	// 건설되었을 때
	if(*m_bBuild == true)
	{
		m_bAffected = true;
		m_bCanAttacked = true;
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	}
	else
	{
		m_bAffected = false;
		m_bCanAttacked = false;
		// 건설 안되었을 때
		// 살 수 있을 때와 없을 때 구분
		if (m_pPlayer->Get_Coin() >= m_iCoin)
		{
			m_bCanBuy = true;
		}
		else
		{
			m_bCanBuy = false;
		}
	}

}

void CTrap_Bricks::Late_Update(_float fTimeDelta)
{
	if (*m_bBuild_PreView == true || *m_bBuild == true)
	{
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
			return;
	}
}

HRESULT CTrap_Bricks::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
			return E_FAIL;
		if (*m_bBuild == false)
		{
			if(m_bCanBuy == true)
			{
				if (FAILED(m_pShaderCom->Begin(0)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderCom->Begin(2)))
					return E_FAIL;
			}
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;
		}

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
		m_pColliderCom->Render();
#endif
	return S_OK;
}

HRESULT CTrap_Bricks::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTrap"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	const _wstring Model_Component = TEXT("Prototype_Component_Model_Trap");
	const _wstring Model_Component_Result = Model_Component + to_wstring(m_iModel_Idx);
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_AABB::BOUND_AABB_DESC		AABBDesc{};
	if(m_iModel_Idx == 1)
		AABBDesc.vExtents = _float3(1.2f, 0.3f, 1.2f);
	if (m_iModel_Idx == 9)
		AABBDesc.vExtents = _float3(0.6f, 0.6f, 0.6f);

	AABBDesc.vCenter = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrap_Bricks::Bind_ShaderResources()
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

CTrap_Bricks* CTrap_Bricks::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrap_Bricks* pInstance = new CTrap_Bricks(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrap_Bricks");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTrap_Bricks::Clone(void* pArg)
{
	CTrap_Bricks* pInstance = new CTrap_Bricks(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CTrap_Bricks");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrap_Bricks::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
