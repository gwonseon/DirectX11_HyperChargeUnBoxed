#include "stdafx.h"
#include "..\Public\Battery.h"

#include "GameInstance.h"


CBattery::CBattery(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CBattery::CBattery(const CBattery& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CBattery::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBattery::Initialize(void* pArg)
{
	BATTERY_DESC* pDesc = static_cast<BATTERY_DESC*>(pArg);
	m_eLevel = pDesc->eID;
	m_pPlayer = pDesc->pPlayer;
	m_pGauge = pDesc->pGauge;
	m_pEnergy_Machine = pDesc->pEnergy_Machine;
	m_pBrain = pDesc->pBrain;
	m_pInGameUI = pDesc->pInGameUI;
	m_pInGameUI_Gauge = pDesc->pInGameUI_Gauge;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f));
	m_pTransformCom->Set_Scaling(pDesc->fScale.x, pDesc->fScale.y, pDesc->fScale.z);
	m_bVisible = m_pPlayer->Get_Visible_Battery();
	
	m_fEnergy = 100.f;
	return S_OK;
}

void CBattery::Priority_Update(_float fTimeDelta)
{
	// 배터리가 머신 안에 있을 때 위치 값 지정해주기
	if (m_pEnergy_Machine->Get_Battery_Is_In() == true)
	{
		m_pInGameUI_Gauge->Set_BatteryGauge(m_fEnergy);
		m_pInGameUI->Set_BatteryGauge(m_fEnergy);
		if (m_fEnergy > 0.f)
		{
			m_fCharging_Delay += fTimeDelta;
			// 2초에 1씩 회복 시킴
			if (m_pBrain->Get_Energy() < 100.f && m_fCharging_Delay >= 2.f)
			{
				m_fCharging_Delay = 0.f;
				m_fEnergy -= 1.f;
				Set_Energy(m_fEnergy);
				m_pBrain->Set_GetEnergy(1.f);
			}
		}
		_vector vMachinePos = m_pEnergy_Machine->Get_EnergyMachinePos();
		vMachinePos = XMVectorSetY(vMachinePos, 4.f);
		vMachinePos = XMVectorSetZ(vMachinePos, XMVectorGetZ(vMachinePos) + 1.f);
		vMachinePos = XMVectorSetX(vMachinePos, XMVectorGetX(vMachinePos) - 0.5f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMachinePos);
		m_pTransformCom->Rotation(0.f, 0.f, 0.f);
		*m_bVisible = true;
	}
	else
	{
		m_pInGameUI_Gauge->Set_BatteryGauge(0.f);
		m_pInGameUI->Set_BatteryGauge(0.f);
		if (m_vecPos != nullptr)
		{
			// 위치 변경, ( 플레이어가 내려놓을 경우 플레이어 위치 받아와서 내려놓음)
			XMStoreFloat3(&fPrevPos, m_vecPrevPos);
			XMStoreFloat3(&fPos, *m_vecPos);
			if (fPrevPos.x != fPos.x && fPrevPos.z != fPos.z)
			{
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPos.x, fPos.y + 4.f, fPos.z, 1.f));
			}
			m_vecPrevPos = *m_vecPos;
		}
		// 배터리 내려 놓았을 때 바닥에 떨어지도록
		if (m_bFirst_PickUp == false && (fPos.x != 0.f && fPos.z != 0.f))
		{
			m_pTransformCom->Rotation(0.f, 0.f, XMConvertToRadians(90.f));
			_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (XMVectorGetY(vCurrentPos) > 0.5f)
			{
				vCurrentPos = XMVectorSetY(vCurrentPos, XMVectorGetY(vCurrentPos) - (fTimeDelta * 10.f));
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPos);
			}
			else
			{
				vCurrentPos = XMVectorSetY(vCurrentPos, 0.5f);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPos);
			}
		}

		if (m_pTransformCom->Cal_Distance_vec(m_pPlayer->Get_Position(), m_pTransformCom->Get_State(CTransform::STATE_POSITION)) <= 80.f) // 플레이어와의 거리 계산
		{
			// 근처에 있을 때 상호작용이 되어야 하는데
			if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_E))
			{
				if (m_pGauge->Get_ItemInteraction_End() == false)
				{
					// Circle Gauge한테 충전하라고 보내주기
					m_pGauge->Set_Item_Interaction(true);
				}
			}
			else
			{
				// Circle Gauge 값 초기화
				m_pGauge->Set_Item_Interaction(false);

			}
			// 아이템 상호작용이 끝났을 때 안보이게 만들기, 나중에 위치 업데이트되면 그자리로 보낸 후 업데이트 할 것
			if (m_pGauge->Get_ItemInteraction_End() == true)
			{
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
				*m_bVisible = false;
				m_bFirst_PickUp = false;
				m_pGauge->Set_Item_Interaction(false);
				m_pGauge->Set_Item_InteractionEnd(false); // 상호작용 끝났는지 알려주는 값 초기화 해주기
				// 플레이어한테 배터리 들라고 알려주기
				m_pPlayer->PickUp_Battery(CPlayer::BATTERY); //111번 
				m_vecPos = m_pPlayer->Get_BatteryPos();
			}
		}
	}
}

void CBattery::Update(_float fTimeDelta)
{
}

void CBattery::Late_Update(_float fTimeDelta)
{
	if(*m_bVisible == true)
	{
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
			return;
	}
}

HRESULT CBattery::Render()
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


	return S_OK;
}

HRESULT CBattery::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxItem"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	const _wstring Model_Component = TEXT("Prototype_Component_Model_Weapon");
	const _wstring Model_Component_Result = Model_Component + to_wstring(11);
	/* For.Com_Model */
	;
	if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBattery::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fFar = m_pGameInstance->Get_CameraFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
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
		return E_FAIL;*/

	return S_OK;


}

CBattery* CBattery::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBattery* pInstance = new CBattery(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBattery");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBattery::Clone(void* pArg)
{
	CBattery* pInstance = new CBattery(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBattery");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattery::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
