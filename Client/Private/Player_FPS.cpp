#include "stdafx.h"
#include "..\Public\Player_FPS.h"

#include "GameInstance.h"
#include "Player.h"
CPlayer_FPS::CPlayer_FPS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CPlayer_FPS::CPlayer_FPS(const CPlayer_FPS& Prototype)
	: CPartObject{ Prototype }
{
}

const _float4x4* CPlayer_FPS::Get_SocketMatrix(const _char* pBoneName)
{
	return m_pModelCom->Get_BoneMatrix(pBoneName);
}

HRESULT CPlayer_FPS::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_FPS::Initialize(void* pArg)
{
	FPS_PLAYER_DESC* pDesc = static_cast<FPS_PLAYER_DESC*>(pArg);

	m_pParentState = pDesc->pParentState;

	/* 추가적으로 초기화가 필요하다면 수행해준다. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	return S_OK;
}

void CPlayer_FPS::Priority_Update(_float fTimeDelta)
{
}

void CPlayer_FPS::Update(_float fTimeDelta)
{
	if (m_bFPSState == true)
	{
		m_bAnimState = m_pModelCom->Play_Animation(fTimeDelta, m_bAnimInit);

		if (m_iJumpState == 0)
		{
			if (*m_pParentState & CPlayer::WALKSTATE_NORTH || *m_pParentState & CPlayer::WALKSTATE_WEST || *m_pParentState & CPlayer::WALKSTATE_SOUTH
				|| *m_pParentState & CPlayer::WALKSTATE_EAST || *m_pParentState & CPlayer::WALKSTATE_NORTHWEST || *m_pParentState & CPlayer::WALKSTATE_NORTHEAST
				|| *m_pParentState & CPlayer::WALKSTATE_SOUTHWEST || *m_pParentState & CPlayer::WALKSTATE_SOUTHEAST)
			{
				switch (m_iWeaponState)
				{
				case Client::CBody_Player::WEAPON_UNARMED:
					m_pModelCom->Set_Animation(FPS_FP_EvilWalking1, true);
					break;
				case Client::CBody_Player::WEAPON_RIFLE:
					m_pModelCom->Set_Animation(FPS_Rifle_Walk, true);
					break;
				case Client::CBody_Player::WEAPON_SHOTGUN:
					m_pModelCom->Set_Animation(FPS_Rifle_Walk, true);
					break;
				case Client::CBody_Player::WEAPON_PULSECANNON:
					m_pModelCom->Set_Animation(FPS_Smoker_Draw, true);
					break;
				case Client::CBody_Player::WEAPON_TELEPORT:
					m_pModelCom->Set_Animation(FPS_Rifle_Walk, true);
					break;
				case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
					m_pModelCom->Set_Animation(FPS_Rifle_Walk, true);
					break;
				case Client::CBody_Player::WEAPON_RIFLE_SECOND:
					m_pModelCom->Set_Animation(FPS_Rifle_Walk, true);
					break;
				case Client::CBody_Player::WEAPON_KATANA:
					m_pModelCom->Set_Animation(FPS_Katana_Walk, true);
					break;
				default:
					break;
				}

			}

			if (*m_pParentState & CPlayer::STATE_IDLE)
			{
				switch (m_iWeaponState)
				{
				case Client::CBody_Player::WEAPON_UNARMED:
					m_pModelCom->Set_Animation(FPS_Unarmed_Idle, true);
					break;
				case Client::CBody_Player::WEAPON_RIFLE:
					m_pModelCom->Set_Animation(FPS_Rifle_Idle, true);
					break;
				case Client::CBody_Player::WEAPON_SHOTGUN:
					m_pModelCom->Set_Animation(FPS_Rifle_Idle, true);
					break;
				case Client::CBody_Player::WEAPON_PULSECANNON:
					m_pModelCom->Set_Animation(FPS_Rifle_Idle, true);
					break;
				case Client::CBody_Player::WEAPON_TELEPORT:
					m_pModelCom->Set_Animation(FPS_Rifle_Idle, true);
					break;
				case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
					m_pModelCom->Set_Animation(FPS_Rifle_Idle, true);
					break;
				case Client::CBody_Player::WEAPON_RIFLE_SECOND:
					m_pModelCom->Set_Animation(FPS_Rifle_Idle, true);
					break;
				case Client::CBody_Player::WEAPON_KATANA:
					m_pModelCom->Set_Animation(FPS_Katana_Idle, true);
					break;
				default:
					break;
				}

			}

			if (*m_pParentState & CPlayer::RUNSTATE_NORTH || *m_pParentState & CPlayer::RUNSTATE_NORTHWEST || *m_pParentState & CPlayer::RUNSTATE_NORTHEAST)
			{
				switch (m_iWeaponState)
				{
				case Client::CBody_Player::WEAPON_UNARMED:
					m_pModelCom->Set_Animation(FPS_FP_EvilSprint1, true);
					break;
				case Client::CBody_Player::WEAPON_RIFLE:
					m_pModelCom->Set_Animation(FPS_Rifle_Sprint, true);
					break;
				case Client::CBody_Player::WEAPON_SHOTGUN:
					m_pModelCom->Set_Animation(FPS_Rifle_Sprint, true);
					break;
				case Client::CBody_Player::WEAPON_PULSECANNON:
					m_pModelCom->Set_Animation(FPS_Rifle_Sprint, true);
					break;
				case Client::CBody_Player::WEAPON_TELEPORT:
					m_pModelCom->Set_Animation(FPS_Rifle_Sprint, true);
					break;
				case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
					m_pModelCom->Set_Animation(FPS_Rifle_Sprint, true);
					break;
				case Client::CBody_Player::WEAPON_RIFLE_SECOND:
					m_pModelCom->Set_Animation(FPS_Rifle_Sprint, true);
					break;
				case Client::CBody_Player::WEAPON_KATANA:
					m_pModelCom->Set_Animation(FPS_Katana_Sprint, true);
					break;
				default:
					break;
				}

			}

			if (*m_pParentState & CPlayer::RELOADING)
			{
				m_pModelCom->Set_Animation(FPS_Rifle_Reload, true);
			}

			//점프
			if (*m_pParentState & CPlayer::JUMP_START && m_iJumpState == 0)
			{
				m_pModelCom->Set_Animation(FPS_Rifle_Idle, false);
				m_iJumpState = 1;
				m_bAnimInit = false;
			}
			if (m_bAnimState == true && m_iJumpState == 3 && m_fHeight <= 0.f)
			{
				m_bAnimInit = false;
				m_iJumpState = 0;

			}
			if (m_bAnimState == true && m_iJumpState == 1 && m_fHeight <= 0.f)
			{
				m_bAnimInit = false;
				m_iJumpState = 2;

			}
			else if (m_bAnimState == false && m_iJumpState == 2 && m_fHeight <= 2.8f && m_fPower <= 0)
			{
				m_bAnimInit = false;
				m_iJumpState = 3;
			}

			if (*m_pParentState & CPlayer::JUMP_LOOP)
				m_pModelCom->Set_Animation(FPS_Rifle_Idle, true);

			if (*m_pParentState & CPlayer::JUMP_END)
				m_pModelCom->Set_Animation(FPS_Rifle_Idle, false);

		}





	}
}
void CPlayer_FPS::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pParentMatrix) * m_pTransformCom->Get_WorldMatrix());
	m_vecPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CPlayer_FPS::Render()
{
	/*if(m_bFPSState == f)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;

			m_pModelCom->Render(i);
		}
	}*/

	return S_OK;
}

HRESULT CPlayer_FPS::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Anim6"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_FPS::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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

CPlayer_FPS* CPlayer_FPS::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_FPS* pInstance = new CPlayer_FPS(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_FPS");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CPlayer_FPS::Clone(void* pArg)
{
	CPlayer_FPS* pInstance = new CPlayer_FPS(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer_FPS");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_FPS::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
