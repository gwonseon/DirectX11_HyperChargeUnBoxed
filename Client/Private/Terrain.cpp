#include "stdafx.h"
#include "..\Public\Terrain.h"

#include "GameInstance.h"
#include "PipeLine.h"
#include "Level_ImGui.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTerrain::CTerrain(const CTerrain& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	/* 패킷, 파일입ㅇ출력을 통한 초기화. */

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	TERRAIN_DESC* pDesc = static_cast<TERRAIN_DESC*>(pArg);
	m_eLevel = pDesc->eID;
	m_pPlayer = pDesc->pPlayer;
	if(m_eLevel == LEVEL_IMGUI || m_eLevel == LEVEL_NAVIGATION || m_eLevel == LEVEL_MONSTERSPAWN)
	{
		m_eTargetID = pDesc->eTargetID;
	}
	/* 추가적으로 초기화가 필요하다면 수행해준다. */
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	
	if (FAILED(Add_Components()))
		return E_FAIL;


#ifdef _DEBUG
	if (m_eLevel == LEVEL_MONSTERSPAWN)
	{
		CCollisionBox::COLLISIONBOX_DESC CollisionDesc{};
		for (auto pCell : m_pNavigationCom->Get_Cells())
		{
			CollisionDesc.iCell_Idx = pCell->Get_CellIndex();
			CollisionDesc.eLevel = LEVEL_MONSTERSPAWN;
			CollisionDesc.fPosition = pCell->Get_Cell_CenterPos();
			m_vecCollisionBox.push_back(static_cast<CCollisionBox*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_MONSTERSPAWN, TEXT("Layer_Collision"), TEXT("Prototype_GameObject_Collision_Box"), &CollisionDesc)));
		}
	}
#endif


	return S_OK;
}

void CTerrain::Priority_Update(_float fTimeDelta)
{
	m_pNavigationCom->Update(m_pTransformCom->Get_WorldMatrixPtr());
}

void CTerrain::Update(_float fTimeDelta)
{
	//if (GetAsyncKeyState(VK_F9) & 0x01)
	//{
	//	// 맵 그리드로 바꾸기
	//	m_pVIBufferCom->Chang_Topology();
	//}

	m_pVIBufferCom->Update(fTimeDelta);
}

void CTerrain::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
		return;
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_HEIGHT, this)))
		return;

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pNavigationCom);
#endif
}

HRESULT CTerrain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Render_Height()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4			ViewMatrix, ProjMatrix;

	_vector PlayerPos = m_pPlayer->Get_Position();

	_matrix			matView = XMMatrixIdentity();
	matView.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	matView.r[1] = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	matView.r[2] = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	matView.r[3] = XMVectorSet(XMVectorGetX(PlayerPos), 20.f, XMVectorGetZ(PlayerPos), 1.f);

	XMStoreFloat4x4(&ViewMatrix, XMMatrixInverse(nullptr, matView));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixOrthographicLH(200.f, 200.f, 0.f, 30.f));


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

 
HRESULT CTerrain::Add_Components()
{
	/* 멤버변수로 직접 참조를 하게되면 */
	/* 1. 내가 내 컴포넌트를 이용하고자할 때 굳이 검색이 필요없이 특정 멤버변수로 바로 기능을 이용하면 된다. */
	/* 2. 다른 객체가 내 컴포넌트를 검색하고자 할때 스위치케이스가 겁나 늘어나는 상황. */

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	switch (m_eLevel)
	{
	case Client::LEVEL_GAMEPLAY:
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
			return E_FAIL;
		break;
	}
	case Client::LEVEL_YARD:
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Yard"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
			return E_FAIL;
		break;
	}
	case Client::LEVEL_IMGUI:
	{
		switch (m_eTargetID)
		{

		case Client::LEVEL_GAMEPLAY:
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
				TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
				return E_FAIL;
			break;
		}
		case Client::LEVEL_YARD:
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Yard"),
				TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
				return E_FAIL;
			break;
		}

		default:
			break;
		}
	
		break;
	}
	case Client::LEVEL_NAVIGATION:
	{
		switch (m_eTargetID)
		{

		case Client::LEVEL_GAMEPLAY:
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
				TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
				return E_FAIL;
			break;
		}
		case Client::LEVEL_YARD:
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Yard"),
				TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
				return E_FAIL;
			break;
		}

		default:
			break;
		}
		break;
	}
	case Client::LEVEL_MONSTERSPAWN:
	{
		switch (m_eTargetID)
		{
		case Client::LEVEL_GAMEPLAY:
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
				TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
				return E_FAIL;
			break;
		}
		case Client::LEVEL_YARD:
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Yard"),
				TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
				return E_FAIL;
			break;
		}
		default:
			break;
		}
		break;
	}
	default:
		break;
	}




	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 1)))
		return E_FAIL;


	_float fFar = m_pGameInstance->Get_CameraFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
		return E_FAIL;
	
	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pShaderCom);
}
