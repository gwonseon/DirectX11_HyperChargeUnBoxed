
#include "stdafx.h"
#include "..\Public\Pivot.h"

#include "GameInstance.h"
#include "Player.h"
CPivot::CPivot(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
	: CPartObject{pDevice,pContext}
{}

CPivot::CPivot(const CPivot& Prototype)
	: CPartObject{Prototype}
{}

HRESULT CPivot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPivot::Initialize(void* pArg)
{
	PIVOT_DESC* pDesc = static_cast<PIVOT_DESC*>(pArg);

	m_pParentState = pDesc->pParentState;
	m_pSocketMatrix = pDesc->pSocketMatrix;

	/* 추가적으로 초기화가 필요하다면 수행해준다. */
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if(FAILED(Add_Components()))
		return E_FAIL;

	// Position = { -0.329998,-0.729998,1.74475 };
	Position = {-0.439998f,-0.109998f,1.84475};
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,XMVectorSet(Position.x,Position.y,Position.z,1.f));


	return S_OK;

}

void CPivot::Priority_Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for(size_t i = 0; i < 3; i++)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_WorldMatrix,m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
	m_vecTPS_CamPos = XMVectorSet(m_WorldMatrix._41,m_WorldMatrix._42,m_WorldMatrix._43,1.f);
}

void CPivot::Update(_float fTimeDelta)
{

	/*if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_1))
	{
		Position.x += 0.01f;
	}
	if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_2))
	{
		Position.y += 0.01f;
	}
	if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_3))
	{
		Position.z += 0.01f;
	}
	if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_4))
	{
		Position.x -= 0.01f;
	}
	if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_5))
	{
		Position.y -= 0.01f;
	}
	if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_6))
	{
		Position.z -= 0.01f;
	}
	if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_J))
	{
		Rotation.x += 0.01f;
	}
	if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_K))
	{
		Rotation.y += 0.01f;
	}
	if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_L))
	{
		Rotation.y += 0.01f;
	}
	if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_U))
	{
		Rotation.x -= 0.01f;
	}
	if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_I))
	{
		Rotation.y -= 0.01f;
	}
	if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_O))
	{
		Rotation.z -= 0.01f;
	}
	if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_P))
	{
		cout << Position.x << "    " << Position.y << "    " << Position.z << endl;
		cout << Rotation.x << "    " << Rotation.y << "    " << Rotation.z << endl;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(Position.x, Position.y, Position.z, 1.f));*/


}

void CPivot::Late_Update(_float fTimeDelta)
{

	if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND,this)))
		return;
}

HRESULT CPivot::Render()
{
	/*if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}*/

	return S_OK;
}

HRESULT CPivot::Add_Components()
{
	/* For.Com_Shader */
	if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"),reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	///* For.Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon1"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

	return S_OK;

}

HRESULT CPivot::Bind_ShaderResources()
{
	if(FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix",&m_WorldMatrix)))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition",m_pGameInstance->Get_CamPosition(),sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if(nullptr == pLightDesc)
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir",&pLightDesc->vDirection,sizeof(_float4))))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse",&pLightDesc->vDiffuse,sizeof(_float4))))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient",&pLightDesc->vAmbient,sizeof(_float4))))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular",&pLightDesc->vSpecular,sizeof(_float4))))
		return E_FAIL;

	return S_OK;

}

CPivot* CPivot::Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
{
	CPivot* pInstance = new CPivot(pDevice,pContext);

	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPivot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPivot::Clone(void* pArg)
{
	CPivot* pInstance = new CPivot(*this);

	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPivot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPivot::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}