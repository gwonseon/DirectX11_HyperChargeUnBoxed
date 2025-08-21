#include "stdafx.h"
#include "..\Public\Collector.h"

#include "GameInstance.h"


CCollector::CCollector(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
	: CGameObject{pDevice,pContext}
{}

CCollector::CCollector(const CCollector& Prototype)
	: CGameObject{Prototype}
	,m_vecItemPos{Prototype.m_vecItemPos}
{}

HRESULT CCollector::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollector::Initialize(void* pArg)
{
	COLLECTOR_DESC* pDesc = static_cast<COLLECTOR_DESC*>(pArg);
	m_pPlayer = pDesc->pPlayer;
	m_fScale = pDesc->fScale;
	m_iModelIndex = pDesc->iModelIndex;
	m_eLevel = pDesc->eID;

	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if(FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaling(m_fScale.x,m_fScale.y,m_fScale.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,XMVectorSet(pDesc->fPosition.x,0.2f,pDesc->fPosition.z,1.f));

	// 중점위치 변경 ( 회전 위치를 바꿔줌)
	_float4x4 matSecondPreTransform{};
	XMStoreFloat4x4(&matSecondPreTransform,XMMatrixTranslation(0.0f,0.f,0.f));
	m_pModelCom->Set_SecondPreTransform(matSecondPreTransform);
	m_fCharging_Time = 0.f;


	return S_OK;
}

void CCollector::Priority_Update(_float fTimeDelta)
{
	_vector Axis = {0.f,1.f,0.f};
	m_pTransformCom->Turn(Axis,fTimeDelta * 0.3f);

}

void CCollector::Update(_float fTimeDelta)
{
	if(m_bDead)
		return;
	if(m_bKnockdown == true)
	{
		if(m_bOnce == false)
		{
			m_pPlayer->Set_Energy_Plus(20.f);
			m_pGameInstance->StopSound(ITEM);
			m_pGameInstance->PlaySoundW(L"FE_FX_Pickup_Coin.wav",Engine::CHANNELID::ITEM,m_fSound * 0.4f);
			m_bOnce = true;
		}
		m_fDeadTime += fTimeDelta;
		if(m_fDeadTime >= 1.f)
		{
			m_pPlayer->Set_PickUp_CollectItem();
			m_bDead = true;
		}
	}
	_vector vecPlayerPos = m_pPlayer->Get_Position();
	m_vecItemPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	// 플레이어와 아이템 거리가 가까워졌을 때
	if(m_pTransformCom->Cal_Distance_vec(vecPlayerPos,m_vecItemPos) <= 50.f)
	{
		if(m_fSizeUp <= 2.f)
			m_fSizeUp += fTimeDelta * 10.f;
		// 사이즈 커지기
		m_pTransformCom->Set_Scaling(m_fScale.x + m_fSizeUp,m_fScale.y + m_fSizeUp,m_fScale.z + m_fSizeUp);

		if(m_pGameInstance->Get_DIKeyState_Pressing(DIK_E))
		{
			m_bInteraction = true;
			m_fCharging_Time += fTimeDelta;
		} else
		{
			m_bInteraction = false;
			m_fCharging_Time = 0.f;
		}

	} else
	{
		m_bInteraction = false;
		if(m_fSizeUp >= 0.f)
			m_fSizeUp -= fTimeDelta * 10.f;
		m_pTransformCom->Set_Scaling(m_fScale.x + m_fSizeUp,m_fScale.y + m_fSizeUp,m_fScale.z + m_fSizeUp);
		m_fCharging_Time = 0.f;
	}

	// 차징 끝났을 때 아이템 얻기
	if(m_fCharging_Time >= 1.f)
	{
		m_fCharging_Time = 0.f;
		// UI 띄우기 ( 남은 아이템 개수 )
		m_bInteraction = false;
		m_bKnockdown = true;
	}


}

void CCollector::Late_Update(_float fTimeDelta)
{
	if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONLIGHT,this)))
		return;
}

HRESULT CCollector::Render()
{
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for(size_t i = 0; i < iNumMeshes; i++)
	{
		if(FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom,i,aiTextureType_DIFFUSE,0,"g_DiffuseTexture")))
			return E_FAIL;

		if(m_bKnockdown == false)
		{
			if(FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;
		} else
		{
			if(FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;
		}

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CCollector::Add_Components()
{    /* For.Com_Texture */
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_Dissolved"),
		TEXT("Com_Texture"),reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Shader_VtxItem"),
		TEXT("Com_Shader"),reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	const _wstring Model_Component = TEXT("Prototype_Component_Model_Environment");
	const _wstring Model_Component_Result = Model_Component + to_wstring(m_iModelIndex + ENVIRONMENT_EA);
	/* For.Com_Model */
	if(FAILED(__super::Add_Component(m_eLevel,Model_Component_Result,
		TEXT("Com_Model"),reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCollector::Bind_ShaderResources()
{
	if(m_bKnockdown == true)
	{
		if(FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom,"g_MaskTexture",static_cast<_uint>(1))))
			return E_FAIL;
		if(FAILED(m_pShaderCom->Bind_RawValue("g_fDissolve_Value",&m_fDeadTime,sizeof(float))))
			return E_FAIL;
	}
	if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom,"g_WorldMatrix")))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_SecondMatrix",m_pModelCom->Get_SecondPreTransform())))
		return E_FAIL;
	auto SecondPreTrans = *m_pModelCom->Get_SecondPreTransform();
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	_float fFar = m_pGameInstance->Get_CameraFar();
	if(FAILED(m_pShaderCom->Bind_RawValue("g_fFar",&fFar,sizeof(float))))
		return E_FAIL;



	return S_OK;
}

CCollector* CCollector::Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
{
	CCollector* pInstance = new CCollector(pDevice,pContext);
	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCollector");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCollector::Clone(void* pArg)
{
	CCollector* pInstance = new CCollector(*this);
	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCollector");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollector::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}