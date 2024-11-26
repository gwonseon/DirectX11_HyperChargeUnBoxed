#include "stdafx.h"
#include "..\Public\Effect_Electricity.h"

#include "GameInstance.h"
#include <Player.h>

CEffect_Electricity::CEffect_Electricity(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CEffect_Electricity::CEffect_Electricity(const CEffect_Electricity& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Electricity::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect_Electricity::Initialize(void* pArg)
{
	EFFECT_ELECTRICITY_DESC* pDesc = static_cast<EFFECT_ELECTRICITY_DESC*>(pArg);
	m_eLevel = pDesc->eLevel;
	m_vecPos = pDesc->vecPos;
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fScale = pDesc->fScale;

	_vector newPos = *m_vecPos;
	newPos = XMVectorSetY(newPos, XMVectorGetY(newPos) + 6.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, newPos);

	switch (pDesc->iTexNum)
	{
	case 0:
	{

		break;
	}
	case 1:
	{
		m_fMaxFrame.x = m_fMaxFrame.y = 1.f;
		m_iTextureNum = 1;
		break;
	}
	case 2:
	{

		break;
	}
	case 3:
	{
		m_fMaxFrame.x = m_fMaxFrame.y = 0.f;
		_uint iRan = rand() % 3;
		m_iTextureNum = 3;
		break;
	}


	default:
		break;
	}
	if (m_iTextureNum == 4 || m_iTextureNum == 5)
	{
		
		m_pTransformCom->Set_Scaling(6.f, 6.f, 6.f);
	}
	else
	{
		m_pTransformCom->Set_Scaling(m_fScale.x - 2.f , m_fScale.y - 2.f, m_fScale.z - 2.f);
	}
	return S_OK;
}

void CEffect_Electricity::Priority_Update(_float fTimeDelta)
{
	_vector newPos = *m_vecPos;
	newPos = XMVectorSetY(newPos, XMVectorGetY(newPos) + 6.f);
	CLayer* pPlayerLayer = (m_pGameInstance->Find_Layer(m_eLevel, TEXT("Layer_Player")));
	CPlayer* pPlayer = static_cast<CPlayer*>(pPlayerLayer->Get_GameObject_List().front());
	_vector PlayerPos = pPlayer->Get_Position();
	_vector vDir = PlayerPos - newPos;
	vDir = XMVector3Normalize(vDir);
	newPos += vDir * (2.f + m_fMoveUV * 5.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, newPos);
}

void CEffect_Electricity::Update(_float fTimeDelta)
{
	__super::Compute_Depth();

	switch (m_iTextureNum)
	{
	case 0:
	{

		break;
	}
	case 1:
	{
		if (m_fFrame.x < m_fMaxFrame.x)
		{
			if (m_fDelay >= 0.2f)
			{
				m_fFrame.x += 1.f;
				m_fDelay = 0.f;
			}
		}
		else
		{
			m_fFrame.y += 1.f;
			m_fFrame.x = 0.f;
		}
		if (m_fFrame.y > m_fMaxFrame.y)
		{
			m_bDead = true;
		}
		m_fDelay += fTimeDelta;
		break;
	}
	case 2:
	{

		break;
	}
	case 3:
	{
		if (m_fMoveUV < 0.5f)
		{
			m_fMoveUV += fTimeDelta * 0.8f;
		}
		else
			m_bDead = true;
		break;
	}
	case 4:
	{
		if (m_fMoveUV < 0.5f)
		{
			m_fMoveUV += fTimeDelta * 0.8f;
		}
		else
			m_bDead = true;
		break;
	}
	case 5:
	{
		if (m_fMoveUV < 0.5f)
		{
			m_fMoveUV += fTimeDelta * 0.8f;
		}
		else
			m_bDead = true;
		break;
	}
	default:
		break;
	}



}

void CEffect_Electricity::Late_Update(_float fTimeDelta)
{
	CamPos = *m_pGameInstance->Get_CamPosition();
	m_pTransformCom->LookAt(XMVectorSet(CamPos.x, CamPos.y, CamPos.z, 1.f));

	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
		return;
	if(m_iTextureNum == 3 || m_iTextureNum == 4|| m_iTextureNum == 5)
	{
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
			return;
	}
}

HRESULT CEffect_Electricity::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	switch (m_iTextureNum)
	{
	case 0:
	{

		break;
	}
	case 1:
	{
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;
		break;
	}
	case 2:
	{

		break;
	}
	case 3:
	{
		if (FAILED(m_pShaderCom->Begin(4)))
			return E_FAIL;
		break;
	}
	case 4:
	{
		if (FAILED(m_pShaderCom->Begin(4)))
			return E_FAIL;
		break;
	}
	case 5:
	{
		if (FAILED(m_pShaderCom->Begin(4)))
			return E_FAIL;
		break;
	}
	default:
		break;
	}

	
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;


}

HRESULT CEffect_Electricity::Add_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_Lightning"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Electricity::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", static_cast<_uint>(m_iTextureNum))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShaderCom, "g_DepthTexture", TEXT("Target_Depth"))))
		return E_FAIL;

	_float fFar = m_pGameInstance->Get_CameraFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Index", &m_fFrame, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTex_Move", &m_fMoveUV, sizeof(float))))
		return E_FAIL;
	
	_float2		ImageEa = { m_fMaxFrame.x + 1.f,m_fMaxFrame.y + 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ImageEA", &ImageEa, sizeof(_float2))))
		return E_FAIL;


	return S_OK;
}

CEffect_Electricity* CEffect_Electricity::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Electricity* pInstance = new CEffect_Electricity(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Electricity");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Electricity::Clone(void* pArg)
{
	CEffect_Electricity* pInstance = new CEffect_Electricity(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CEffect_Electricity");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Electricity::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
