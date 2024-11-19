#include "stdafx.h"
#include "..\Public\CollisionBox.h"

#include "GameInstance.h"
#include "Environment.h"

CCollisionBox::CCollisionBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}
CCollisionBox::CCollisionBox(const CCollisionBox& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CCollisionBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollisionBox::Initialize(void* pArg)
{
	COLLISIONBOX_DESC* pDesc = static_cast<COLLISIONBOX_DESC*>(pArg);
	m_iImGuiMode = pDesc->iImGuiMode;
	m_eLevel = pDesc->eLevel;
	
	m_iCellType = 0;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (m_eLevel == LEVEL_NAVIGATION)
	{
		m_fScale = { 0.3f, 0.3f, 0.3f };
		m_iPointNumber = pDesc->iPoint_Number; // ¹è¿­ ÀÎµ¦½º
		m_iCellType = pDesc->iCellType;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f));
		m_pTransformCom->Set_Scaling(0.3f, 0.3f, 0.3f);
		m_vecPosition = { pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f };
		XMStoreFloat3(&m_fPickingPos, m_vecPosition);
		m_iIndexNumber = pDesc->iIndexNumber; // ÀüÃ¼ ÀÎµ¦½º

		if (m_iPointNumber == 0)
		{
			m_fClolor = { 255.f,0.f,0.f };
		}
		else if (m_iPointNumber == 1)
		{
			m_fClolor = { 0.f,255.f,0.f };
		}
		else if (m_iPointNumber == 2)
		{
			m_fClolor = { 0.f,0.f,255.f };
		}
	

	}
	if (m_eLevel == LEVEL_MONSTERSPAWN)
	{
		m_fScale = { 0.3f, 0.3f, 0.3f };
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f));
		m_pTransformCom->Set_Scaling(0.3f, 0.3f, 0.3f);
		m_vecPosition = { pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f };
		XMStoreFloat3(&m_fPickingPos, m_vecPosition);
		m_iCellIdx = pDesc->iCell_Idx;
	}
	return S_OK;
}

void CCollisionBox::Priority_Update(_float fTimeDelta)
{
}

void CCollisionBox::Update(_float fTimeDelta)
{
	if (m_bDead)
		return;
	if(m_eLevel == LEVEL_IMGUI)
	{
		if (m_iImGuiMode == m_iCurrentImGuiMode)
		{
			if (m_bChecking == false)
			{
				m_fClolor = { 0.f,0.f,0.f };
			}
			else
			{
				m_fClolor = { 255.f,0.f,0.f };
			}

			// YÁÂÇ¥ ¿Ã·ÁÁÖ±â
			m_vecPosition = XMVectorSetByIndex(m_vecPosition, 1.f, 3);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);
			m_pTransformCom->Set_Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
		}
	}
	if (m_eLevel == LEVEL_NAVIGATION)
	{
		if (m_bChecking == true)
		{
			if (m_fScale.y > 0.7f)
				m_bBouncing = true;
			if (m_fScale.y < 0.3f)
				m_bBouncing = false;
			if (m_bBouncing == false)
				m_fScale.y += fTimeDelta;
			else
				m_fScale.y -= fTimeDelta;
		}
		else
		{
			m_fScale.y = 0.3f;
		}
		m_pTransformCom->Set_Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);
	}
	if (m_eLevel == LEVEL_MONSTERSPAWN)
	{
		if (m_bChecking == true)
		{
			m_fClolor = { 255.f,0.f,0.f };
			if (m_fScale.y > 0.7f)
				m_bBouncing = true;
			if (m_fScale.y < 0.3f)
				m_bBouncing = false;
			if (m_bBouncing == false)
				m_fScale.y += fTimeDelta;
			else
				m_fScale.y -= fTimeDelta;
		}
		else
		{
			m_fClolor = { 0.f,0.f,0.f };
			m_fScale.y = 0.3f;
		}
		m_pTransformCom->Set_Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);
	}
}

void CCollisionBox::Late_Update(_float fTimeDelta)
{

	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CCollisionBox::Render()
{
	if(m_eLevel == LEVEL_IMGUI)
	{
		if (m_iImGuiMode == m_iCurrentImGuiMode)
		{
			if (FAILED(Bind_ShaderResources()))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Bind_Buffers()))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}
	if(m_eLevel == LEVEL_NAVIGATION || m_eLevel == LEVEL_MONSTERSPAWN )
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Bind_Buffers()))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CCollisionBox::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxBoxColor"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Box"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	return S_OK;
}

HRESULT CCollisionBox::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_fClolor, sizeof(_float3))))
	//	return E_FAIL;

	return S_OK;

}

CCollisionBox* CCollisionBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCollisionBox* pInstance = new CCollisionBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCollisionBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCollisionBox::Clone(void* pArg)
{
	CCollisionBox* pInstance = new CCollisionBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCollisionBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollisionBox::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	

}