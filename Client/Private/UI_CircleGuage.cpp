
#include "stdafx.h"
#include "..\Public\UI_CircleGuage.h"

#include "GameInstance.h"

CUI_CircleGuage::CUI_CircleGuage(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
	: CUIObject{pDevice,pContext}
{}

CUI_CircleGuage::CUI_CircleGuage(const CUI_CircleGuage& Prototype)
	: CUIObject{Prototype}
{}

HRESULT CUI_CircleGuage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_CircleGuage::Initialize(void* pArg)
{
	CIRCLEGAUGE_DESC* pDesc = (CIRCLEGAUGE_DESC*)pArg;
	m_pPlayer = pDesc->pPlayer;
	m_pvecTrap_Marks = pDesc->vecMarks;
	m_pEnergy_Machine = pDesc->pEnergy_Machine;
	m_pEnergyMachine_Cap = pDesc->pEnergyMachine_Cap;
	m_eLevel = pDesc->eLevel;
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if(FAILED(Add_Components(1)))
		return E_FAIL;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,XMVectorSet(pDesc->fPosition.x,pDesc->fPosition.y,pDesc->fPosition.z,1.f));

	m_bBattery_Insert_End = false;

	m_bBuildMode = m_pPlayer->Get_BuildMode();
	return S_OK;
}

void CUI_CircleGuage::Priority_Update(_float fTimeDelta)
{
	// 빌드모드에서 건물 만들 때
	if(*m_bBuildMode == true && m_bItemCharging == false)
	{
		if(m_pPlayer->Get_Build_Gauging() == true)
			m_bBuild_Draw = true;
		else
			m_bBuild_Draw = false;

		if(m_fReal_Gauging_Time >= 1.f)
		{
			m_fReal_Gauging_Time = 0.f;
			m_bBuild_Draw = false;
			for(auto& pMark : *m_pvecTrap_Marks)
			{
				// 만들 수 있고, 살 수 있을 때
				if(pMark->Get_BuildAble() == true && pMark->Get_CanBuy() == true)
				{
					// 돈 나가는 소리
					m_pGameInstance->PlaySoundW(L"FE_BuildCredits_Recieved_Cash_02.wav",Engine::CHANNELID::PLAYER_ACT,0.8f);
					m_pGameInstance->PlaySoundW(L"FE_Buildable_Barricades_Build_Complete.wav",Engine::CHANNELID::TRAP_BUILD,0.8f);

					m_pPlayer->UseCoin(pMark->Get_Privce());
					pMark->Set_Build_Done(true);
					break;
				}
			}
		}
	}
	// 배터리 줍기
	if(*m_pPlayer->Get_WeaponState() == CPlayer::BATTERY)
	{
		if(m_pTransformCom->Cal_Distance_vec(m_pEnergy_Machine->Get_EnergyMachinePos(),m_pPlayer->Get_Position()) <= 80.f)
		{
			if(m_pGameInstance->Get_DIKeyState_Pressing(DIK_E) && m_bBattery_Insert_End == false)
			{
				m_bBattery_Insert = true;
			} else
			{
				m_bBattery_Insert = false;
				m_fReal_Gauging_Time = 0.f;
				m_fGuaging_Time = 0.f;
			}
		}
	} else
	{
		m_bBattery_Insert_End = false;
	}
	// 아이템과의 상호 작용
	if(m_bItem_Interaction == true && m_bItem_Interaction_End == false && m_bCoinItem_Interaction == false)
	{
		if(m_fReal_Gauging_Time >= 1.f)
		{
			m_bItem_Interaction = false;
			m_bItem_Interaction_End = true;
			m_bItemCharging = false;
			m_fGuaging_Time = 0.f;
			m_fReal_Gauging_Time = 0.f;
		}
	}

	//  아이템과의 상호 작용
	if(m_bInteraction == true)
	{
		if(m_fReal_Gauging_Time >= 1.f)
		{
			m_bInteraction = false;
			m_fGuaging_Time = 0.f;
			m_fReal_Gauging_Time = 0.f;
		}
	}
}

void CUI_CircleGuage::Update(_float fTimeDelta)
{
	// 게이지 차징
	if(m_bCharging == true || m_bBuild_Draw == true || m_bItemCharging == true || m_bBattery_Insert == true || m_bInteraction == true)
	{
		m_fReal_Gauging_Time += fTimeDelta;
		m_fGuaging_Time += fTimeDelta * 10;

	} else
	{
		m_fReal_Gauging_Time = 0.f;
		m_fGuaging_Time = 0.f;
	}
}

void CUI_CircleGuage::Late_Update(_float fTimeDelta)
{

	// 배터리 삽입
	if(m_bBattery_Insert == true && m_bBattery_Insert_End == false)
	{
		// 배터리 삽입 완료 되었을 때
		if(m_fReal_Gauging_Time >= 1.f)
		{
			m_bBattery_Insert_End = true;	// 삽입 완료
			m_bBattery_Insert = false;		// 삽입 로딩 완료
			m_fGuaging_Time = 0.f;			// 시간 초기화
			m_fReal_Gauging_Time = 0.f;		// 시간 초기화
			m_pEnergy_Machine->Set_BatteryInsert(true);   // 머신에 배터리 넣기
			m_pPlayer->Insert_Battery();				  // 플레이어 들고 있는 무기 정상화
			if(m_bBattery_Insert_First == false)
			{
				m_pEnergyMachine_Cap->Set_BatteryIn();
				m_bBattery_Insert_First = true;
			}
		}
	}

	// 그리기
	if(m_bCharging == true || m_bBuild_Draw == true || m_bItemCharging == true || m_bBattery_Insert == true ||  m_bInteraction == true)
	{
		if(FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI,this)))
			return;
	}
}

HRESULT CUI_CircleGuage::Render()
{
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;
	if(FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if(FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CircleGuage::Add_Components(_int iNum)
{
	if(FAILED(__super::Add_Component(m_eLevel,TEXT("Prototype_Component_Texture_CircleGuage"),
		TEXT("Com_Texture"),reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Shader_VtxCircleGuage"),
		TEXT("Com_Shader"),reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if(FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"),reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	return S_OK;

}

HRESULT CUI_CircleGuage::Bind_ShaderResources()
{
	if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom,"g_WorldMatrix")))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_RawValue("g_Time",&m_fGuaging_Time,sizeof(float))))
		return E_FAIL;
	_float2 Winsize = {g_iWinSizeX,g_iWinSizeY};
	if(FAILED(m_pShaderCom->Bind_RawValue("g_Winsize",&Winsize,sizeof(_float2))))
		return E_FAIL;
	if(FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom,"g_Texture",0)))
		return E_FAIL;

	return S_OK;
}

CUI_CircleGuage* CUI_CircleGuage::Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
{
	CUI_CircleGuage* pInstance = new CUI_CircleGuage(pDevice,pContext);

	if(FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_CircleGuage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_CircleGuage::Clone(void* pArg)
{
	CUI_CircleGuage* pInstance = new CUI_CircleGuage(*this);

	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CUI_CircleGuage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_CircleGuage::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}