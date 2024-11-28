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
	m_bAttackState = pDesc->m_bAttackState;
	m_eLevelID = pDesc->m_eLevelID;
	/* 추가적으로 초기화가 필요하다면 수행해준다. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_iViewState = pDesc->m_iViewState;

	m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Idle_Unarmed, true);
	m_pModelCom->Set_Animation_UpperBody(PLAYER_ANIM_FiringAnimation8_Base, true);
	m_iUpperMotion = IDLE_MOTION;


	return S_OK;
}

void CBody_Player::Priority_Update(_float fTimeDelta)
{
	//  6번 부턴 총쏘는 모션이기 때문에 사격 딜레이 넣어줌
	if (m_iUpperMotion >= 6)
	{
		m_fCurrentDelay += fTimeDelta;
	}
	UpperBody_Anim(fTimeDelta);
	LowerBody_Anim(fTimeDelta);

}

void CBody_Player::Update(_float fTimeDelta)
{
		
	// 상체
		if(m_iUpperMotion < 6)
		{
			_long   MouseMove = { 0 };
			if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
			{
				if (m_bTPSState == true)
				{
					m_bUpperAnimState = m_pModelCom->Play_Animation_UpperBody(fTimeDelta, m_fArmAngle, m_iUpperMotion, m_bTemp);
				}
				else
				{
					m_bUpperAnimState = m_pModelCom->Play_Animation_UpperBody(fTimeDelta, 0.f, m_iUpperMotion, m_bTemp);
				}
			}
			else
			{
				m_bUpperAnimState = m_pModelCom->Play_Animation_UpperBody(fTimeDelta, m_fArmAngle, m_iUpperMotion, m_bTemp);
			}
		}
		else
		{
			// 총 쏘는 것은 딜레이 시간이 끝났을 때만 쏠 수 있게 별로도 나눠줌
			// m_bShotNow 가 애니메이션의 시작과 끝을 알려주는데 총쏘는건 딱 한 번만 돌아야한다. 그래서 이걸 위한 구분이 필요함
			if (m_bShotNow == true)
			{
				m_fCurrentDelay = 0.f;
				_long   MouseMove = { 0 };
				if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
				{
					if (m_bTPSState == true)
					{
						m_bUpperAnimState = m_pModelCom->Play_Animation_UpperBody(fTimeDelta, m_fArmAngle, m_iUpperMotion, m_bShotNow);
					}
					else
					{
						m_bUpperAnimState = m_pModelCom->Play_Animation_UpperBody(fTimeDelta, 0.f, m_iUpperMotion, m_bShotNow);
					}
				}
				else
				{
					m_bUpperAnimState = m_pModelCom->Play_Animation_UpperBody(fTimeDelta, m_fArmAngle, m_iUpperMotion, m_bShotNow);
				}
			}
		}
		if (*m_bAttackState == true && m_bUpperAnimState == true)
		{
			*m_bAttackState = false;
		}

		// 하체
		if(m_bRunState == false)
			m_bAnimState = m_pModelCom->Play_Animation_LowerBody(fTimeDelta);
		else
			m_bAnimState = m_pModelCom->Play_Animation_LowerBody(fTimeDelta * 1.5f);


		
		if (*m_iViewState == PLAYER_FPS_VIEW)
		{
			m_iShaderPassNum = 1;
		}
		else
			m_iShaderPassNum = 0;
		
		XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pParentMatrix) * m_pTransformCom->Get_WorldMatrix());
		m_vecPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));
}

void CBody_Player::Late_Update(_float fTimeDelta)
{
	if(m_bDead == false)
	{
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_LAST, this)))
			return;
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_SHADOW, this)))
			return;
	}
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

			if (FAILED(m_pShaderCom->Begin(m_iShaderPassNum)))
				return E_FAIL;

			m_pModelCom->Render(i);
		}
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif
	return S_OK;
}

HRESULT CBody_Player::Render_Shadow()
{
	_float4x4			ViewMatrix, ProjMatrix;

	_float fFar = m_pGameInstance->Get_CameraFar();
	_float4 fPlayerPos = m_pGameInstance->Get_PlayerPos();
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(fPlayerPos.x - 3.f, fPlayerPos.y + 10.f, fPlayerPos.z - 3.f, 1.f), XMVectorSet(fPlayerPos.x, fPlayerPos.y, fPlayerPos.z, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.f), (_float)1280.f / 720.f, 0.1f, fFar));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(5)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}
void CBody_Player::UpperBody_Anim(_float fTimeDelta)
{
	if (*m_pParentState_Upper & CPlayer::FIRE)
	{
		if (m_iWeaponState == WEAPON_KATANA)
		{
			m_iUpperMotion = ATTACK_KATANA_MOTION;
			m_pModelCom->Set_Animation_UpperBody(PLAYER_ANIM_NinjaSweepAttack, false);
		}
		else if (m_iWeaponState == BATTERY)
		{
			m_iUpperMotion = PLAYER_ANIM_FiringAnimation8_Base; // 건전지 떨굴 때
			m_pModelCom->Set_Animation_UpperBody(PLAYER_ANIM_FiringAnimation8_Base, true);

		}
		else
		{
			switch (m_iWeaponState)
			{
			case Client::CBody_Player::WEAPON_RIFLE:
			{
				if (m_fCurrentDelay >= m_fRiflrDelay)
				{
					m_bShotNow = true;
					m_bShotStart = true;
				}
				m_iUpperMotion = RIFLE_FIRE_MOTION;
				break;
			}
			case Client::CBody_Player::WEAPON_SHOTGUN:
			{
				if (m_fCurrentDelay >= m_fShotGunDelay)
				{
					m_bShotNow = true;
					m_bShotStart = true;
				}
				m_iUpperMotion = SHOTGUN_FIRE_MOTION;
				break;
			}
			case Client::CBody_Player::WEAPON_PULSECANNON:
			{
				if (m_fCurrentDelay >= m_fPulseCannonDelay)
				{
					m_bShotNow = true;
					m_bShotStart = true;
				}
				m_iUpperMotion = PULSECANNON_FIRE_MOTION;
				break;
			}
			case Client::CBody_Player::WEAPON_TELEPORT:
			{
				if (m_fCurrentDelay >= m_fTeleportDelay)
				{
					m_bShotNow = true;
					m_bShotStart = true;
				}
				m_iUpperMotion = TELEPORTGUN_FIRE_MOTION;
				break;
			}
			case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
			{
				if (m_fCurrentDelay >= m_fLocketLauncherDelay)
				{
					m_bShotNow = true;
					m_bShotStart = true;
				}
				m_iUpperMotion = LOCKETLAUNCHER_FIRE_MOTION;
				break;
			}
			
			default:
				break;
			}
			m_pModelCom->Set_Animation_UpperBody(PLAYER_ANIM_FiringAnimation8_Base, true);
		}
	}
	if (*m_pParentState_Upper & CPlayer::FIRE_RB)
	{
		if (m_iWeaponState == WEAPON_KATANA)
		{
			m_iUpperMotion = ATTACK_KATANA_MOTION;
			m_pModelCom->Set_Animation_UpperBody(PLAYER_ANIM_NinjaSwiftAttack, false);
		}
	}
	if (*m_pParentState_Upper & CPlayer::MELEE)
	{
		m_iUpperMotion = ATTACK_MELEE_MOTION;
		m_pModelCom->Set_Animation_UpperBody(PLAYER_ANIM_DualDagger_Attack_1, false);
	}
	if (*m_pParentState_Upper & CPlayer::RELOADING)
	{
		m_iUpperMotion = RELOAD_MOTION;
		m_pModelCom->Set_Animation_UpperBody(PLAYER_ANIM_WeaponReload);
	}


	if (*m_pParentState_Upper & CPlayer::STATE_IDLE)
	{
		
		if(m_iWeaponState == WEAPON_KATANA)
		{
			m_pModelCom->Set_Animation_UpperBody(PLAYER_ANIM_Idle_Katana, true);
			m_iUpperMotion = IDLE_KATANA_MOTION;
		}
		else
		{
			m_pModelCom->Set_Animation_UpperBody(PLAYER_ANIM_FiringAnimation8_Base, true);
			m_iUpperMotion = IDLE_MOTION;
		}

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
			case Client::CBody_Player::BATTERY:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N_Rifle, true);
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
			case Client::CBody_Player::BATTERY:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_W_Rifle, true);
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
			case Client::CBody_Player::BATTERY:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_S_Rifle, true);
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
			case Client::CBody_Player::BATTERY:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_E_Rifle, true);
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
			case Client::CBody_Player::BATTERY:
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
			case Client::CBody_Player::BATTERY:
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
			case Client::CBody_Player::BATTERY:
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
			case Client::CBody_Player::BATTERY:
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
			case Client::CBody_Player::BATTERY:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Idle_Unarmed, true);
				break;
			default:
				break;
			}
		}

		m_bRunState = false;
		if (*m_pParentState_Lower & CPlayer::RUNSTATE_NORTH)
		{
			m_bRunState = true;
			switch (m_iWeaponState)
			{
			case Client::CBody_Player::WEAPON_UNARMED:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_SHOTGUN:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_PULSECANNON:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_TELEPORT:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE_SECOND:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_KATANA:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N_Katana, true);
				break;
			case Client::CBody_Player::BATTERY:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_N_Rifle, true);
				break;
			default:
				break;
			}
		}

		if (*m_pParentState_Lower & CPlayer::RUNSTATE_NORTHWEST)
		{
			m_bRunState = true;
			switch (m_iWeaponState)
			{
			case Client::CBody_Player::WEAPON_UNARMED:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NW, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NW_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_SHOTGUN:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NW_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_PULSECANNON:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NW_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_TELEPORT:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NW_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NW_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE_SECOND:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NW_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_KATANA:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NW_Katana, true);
				break;
			case Client::CBody_Player::BATTERY:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NW_Rifle, true);
				break;
			default:
				break;
			}
		}

		if (*m_pParentState_Lower & CPlayer::RUNSTATE_NORTHEAST)
		{
			m_bRunState = true;
			switch (m_iWeaponState)
			{
			case Client::CBody_Player::WEAPON_UNARMED:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NE, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NE_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_SHOTGUN:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NE_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_PULSECANNON:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NE_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_TELEPORT:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NE_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_LOCKETLAUNCHER:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NE_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_RIFLE_SECOND:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NE_Rifle, true);
				break;
			case Client::CBody_Player::WEAPON_KATANA:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NE_Katana, true);
				break;
			case Client::CBody_Player::BATTERY:
				m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Walk_NE_Rifle, true);
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
	if (m_bAnimState == false && m_iJumpState == 3 && m_fHeight <= m_fMinHeight + 4.f)
	{
		m_bAnimInit = false;
		m_iJumpState = 0;
	}
	if (m_bAnimState == true && m_iJumpState == 1 && m_fHeight <= m_fMinHeight + 2.5f)
	{
		m_bAnimInit = false;
		m_iJumpState = 2;
	}
	else if (m_bAnimState == false && m_iJumpState == 2 && m_fHeight <= m_fMinHeight + 3.f && m_fPower <= 0.5)
	{
		m_bAnimInit = false;
		m_iJumpState = 3;
	}
	
	if (*m_pParentState_Lower & CPlayer::JUMP_LOOP)
		m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Jump_Loop, true);

	if (*m_pParentState_Lower & CPlayer::JUMP_END)
		m_pModelCom->Set_Animation_LowerBody(PLAYER_ANIM_Jump_Loop, false);

}

HRESULT CBody_Player::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eLevelID, TEXT("Prototype_Component_Model_Anim7"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;
	/* For.Com_Collider_AABB */
	CBounding_AABB::BOUND_AABB_DESC		AABBDesc{};
	AABBDesc.vExtents = _float3(0.5f, 1.5f, 0.5f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y + 1.25f, 0.f);
	if (FAILED(__super::Add_Component(m_eLevelID, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
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
	_float fFar = m_pGameInstance->Get_CameraFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
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
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
