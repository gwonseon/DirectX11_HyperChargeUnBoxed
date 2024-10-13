#include "stdafx.h"
#include "..\Public\Body_Player.h"

#include "GameInstance.h"
#include "Player.h"

CBody_Player::CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Player::CBody_Player(const CBody_Player& Prototype)
	: CPartObject{ Prototype }
{
}

const _float4x4* CBody_Player::Get_SocketMatrix(const _char* pBoneName)
{
	return m_pModelCom->Get_BoneMatrix(pBoneName);
}

HRESULT CBody_Player::Initialize_Prototype()
{
	/* 패킷, 파일입ㅇ출력을 통한 초기화. */

	return S_OK;
}

HRESULT CBody_Player::Initialize(void* pArg)
{
	BODY_PLAYER_DESC* pDesc = static_cast<BODY_PLAYER_DESC*>(pArg);

	m_pParentState_Upper = pDesc->pParentState_Upper;
	m_pParentState_Lower = pDesc->pParentState_Lower;
	/* 추가적으로 초기화가 필요하다면 수행해준다. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	m_iViewState = pDesc->m_iViewState;
	m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Idle_Unarmed, true);
	m_pModelCom->Set_Animation_UpperBody(PLAYER_ANIM_Idle_Shotgun, true);

	return S_OK;
}

void CBody_Player::Priority_Update(_float fTimeDelta)
{
	_uint iData = 10;
}

void CBody_Player::Update(_float fTimeDelta)
{

		// 회전
		_long   MouseMove = { 0 };
		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			m_fArmAngle += (MouseMove * -0.01f);
			if (m_fArmAngle > 25.f)
				m_fArmAngle = 25.f;
			if (m_fArmAngle < -15.f)
				m_fArmAngle = -15.f;

			m_bUpperAnimState = m_pModelCom->Play_Animation_UpperBody(fTimeDelta, m_fArmAngle);
			m_bAnimState = m_pModelCom->Play_Animation_LowerBody(fTimeDelta);

		}
		else
		{
			m_bUpperAnimState = m_pModelCom->Play_Animation_UpperBody(fTimeDelta, m_fArmAngle);
			m_bAnimState = m_pModelCom->Play_Animation_LowerBody(fTimeDelta);

		}		

	
		UpperBody_Anim(fTimeDelta);
		LowerBody_Anim(fTimeDelta);
		

}

void CBody_Player::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pParentMatrix) * m_pTransformCom->Get_WorldMatrix());
	m_vecPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CBody_Player::Render()
{
	//if(m_bTPSState == true)
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
	}

	return S_OK;
}


void CBody_Player::UpperBody_Anim(_float fTimeDelta)
{
	if (*m_pParentState_Upper & CPlayer::FIRE)
	{
		m_pModelCom->Set_Animation_UpperBody(PLAYER_ANIM_FiringAnimation8_Base, true);
	}

	if (*m_pParentState_Upper & CPlayer::RELOADING)
	{
		 m_pModelCom->Set_Animation_UpperBody(PLAYER_ANIM_Pistol_Reload);
	}


	if (*m_pParentState_Upper & CPlayer::STATE_IDLE)
	{
		if(m_iWeaponState == WEAPON_KATANA)
			m_pModelCom->Set_Animation_UpperBody(PLAYER_ANIM_Idle_Katana, true);
		else
			m_pModelCom->Set_Animation_UpperBody(PLAYER_ANIM_Idle_Shotgun, true);

	}
}

void CBody_Player::LowerBody_Anim(_float fTimeDelta)
{
	if (m_iJumpState == 0)
	{
		if (*m_pParentState_Lower & CPlayer::WALKSTATE_NORTH)
		{
			switch (m_iWeaponState)
			{
			case Client::CBody_Player::WEAPON_UNARMED:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_SHOTGUN:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_PULSECANNON:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_TELEPORT:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE_SECOND:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_KATANA:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N_Katana, true);
				break;
			default:
				break;
			}

		}

		if (*m_pParentState_Lower & CPlayer::WALKSTATE_WEST)
		{
			switch (m_iWeaponState)
			{
			case Client::CBody_Player::WEAPON_UNARMED:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_W, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_W_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_SHOTGUN:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_W_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_PULSECANNON:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_W_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_TELEPORT:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_W_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_W_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE_SECOND:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_W_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_KATANA:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_W_Katana, true);
				break;
			default:
				break;
			}

		}

		if (*m_pParentState_Lower & CPlayer::WALKSTATE_SOUTH)
		{
			switch (m_iWeaponState)
			{
			case Client::CBody_Player::WEAPON_UNARMED:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_S, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_S_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_SHOTGUN:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_S_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_PULSECANNON:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_S_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_TELEPORT:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_S_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_S_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE_SECOND:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_S_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_KATANA:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_S_Katana, true);
				break;
			default:
				break;
			}

		}

		if (*m_pParentState_Lower & CPlayer::WALKSTATE_EAST)
		{
			switch (m_iWeaponState)
			{
			case Client::CBody_Player::WEAPON_UNARMED:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_E, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_E_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_SHOTGUN:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_E_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_PULSECANNON:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_E_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_TELEPORT:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_E_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_E_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE_SECOND:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_E_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_KATANA:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_E_Katana, true);
				break;
			default:
				break;
			}

		}

		if (*m_pParentState_Lower & CPlayer::WALKSTATE_NORTHWEST)
		{
			switch (m_iWeaponState)
			{
			case Client::CBody_Player::WEAPON_UNARMED:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NW, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NW_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_SHOTGUN:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NW_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_PULSECANNON:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NW_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_TELEPORT:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NW_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NW_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE_SECOND:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NW_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_KATANA:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NW_Rifle, true);
				break;
			default:
				break;
			}

		}

		if (*m_pParentState_Lower & CPlayer::WALKSTATE_NORTHEAST)
		{
			switch (m_iWeaponState)
			{
			case Client::CBody_Player::WEAPON_UNARMED:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NE, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NE_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_SHOTGUN:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NE_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_PULSECANNON:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NE_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_TELEPORT:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NE_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NE_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE_SECOND:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NE_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_KATANA:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NE_Rifle, true);
				break;
			default:
				break;
			}

		}

		if (*m_pParentState_Lower & CPlayer::WALKSTATE_SOUTHWEST)
		{
			switch (m_iWeaponState)
			{
			case Client::CBody_Player::WEAPON_UNARMED:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_SW, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_SW_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_SHOTGUN:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_SW_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_PULSECANNON:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_SW_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_TELEPORT:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_SW_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_SW_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE_SECOND:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_SW_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_KATANA:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_SW_Rifle, true);
				break;
			default:
				break;
			}

		}

		if (*m_pParentState_Lower & CPlayer::WALKSTATE_SOUTHEAST)
		{
			switch (m_iWeaponState)
			{
			case Client::CBody_Player::WEAPON_UNARMED:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_SE, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_SE_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_SHOTGUN:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_SE_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_PULSECANNON:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_SE_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_TELEPORT:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_SE_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_SE_Shotgun, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE_SECOND:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_SE_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_KATANA:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_SE_Rifle, true);
				break;
			default:
				break;
			}
		}
		if (*m_pParentState_Lower & CPlayer::STATE_IDLE)
		{
			switch (m_iWeaponState)
			{
			case Client::CBody_Player::WEAPON_UNARMED:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Idle_Unarmed, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Idle_Unarmed, true);
				break;
			case Client::CBody_Player::WEAPON_SHOTGUN:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Idle_Unarmed, true);
				break;
			case Client::CBody_Player::WEAPON_PULSECANNON:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Idle_Unarmed, true);
				break;
			case Client::CBody_Player::WEAPON_TELEPORT:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Idle_Unarmed, true);
				break;
			case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Idle_Unarmed, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE_SECOND:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Idle_Unarmed, true);
				break;
			case Client::CBody_Player::WEAPON_KATANA:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Idle_Unarmed, true);
				break;
			default:
				break;
			}

		}

		if (*m_pParentState_Lower & CPlayer::RUNSTATE_NORTH)
		{
			switch (m_iWeaponState)
			{
			case Client::CBody_Player::WEAPON_UNARMED:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_N, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_N_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_SHOTGUN:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_N_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_PULSECANNON:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_N_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_TELEPORT:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_N_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_N_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE_SECOND:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_N_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_KATANA:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_N_Katana, true);
				break;
			default:
				break;
			}

		}


		if (*m_pParentState_Lower & CPlayer::RUNSTATE_NORTHWEST)
		{
			switch (m_iWeaponState)
			{
			case Client::CBody_Player::WEAPON_UNARMED:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_NW, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_NW_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_SHOTGUN:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_NW_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_PULSECANNON:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_NW_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_TELEPORT:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_NW_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_NW_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE_SECOND:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_NW_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_KATANA:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_NW_Katana, true);
				break;
			default:
				break;
			}

		}

		if (*m_pParentState_Lower & CPlayer::RUNSTATE_NORTHEAST)
		{
			switch (m_iWeaponState)
			{
			case Client::CBody_Player::WEAPON_UNARMED:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_NE, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_NE_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_SHOTGUN:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_NE_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_PULSECANNON:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_NE_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_TELEPORT:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_NE_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_NE_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE_SECOND:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_NE_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_KATANA:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Run_NE_Katana, true);
				break;
			default:
				break;
			}

		}


	}




	//점프
	if (*m_pParentState_Lower & CPlayer::JUMP_START && m_iJumpState == 0)
	{
		m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Jump_Start, false);
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

	if (*m_pParentState_Lower & CPlayer::JUMP_LOOP)
		m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Jump_Loop, true);

	if (*m_pParentState_Lower & CPlayer::JUMP_END)
		m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Jump_End, false);


}

HRESULT CBody_Player::Add_Components()
{
	/* 멤버변수로 직접 참조를 하게되면 */
	/* 1. 내가 내 컴포넌트를 이용하고자할 때 굳이 검색이 필요없이 특정 멤버변수로 바로 기능을 이용하면 된다. */
	/* 2. 다른 객체가 내 컴포넌트를 검색하고자 할때 스위치케이스가 겁나 늘어나는 상황. */

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Anim7"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Player::Bind_ShaderResources()
{
	/*if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;*/

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

CBody_Player* CBody_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Player* pInstance = new CBody_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Player::Clone(void* pArg)
{
	CBody_Player* pInstance = new CBody_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBody_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Player::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
