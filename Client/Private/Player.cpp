#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"

#include <Camera_Free.h>
#include <FPS_Pivot.h>

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CContainerObject{ Prototype }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	CContainerObject::CONTAINEROBJECT_DESC		Desc{};


	Desc.iNumPartObjects = PART_END;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.f);
	Desc.fPosition = _float3(418.755f, 0.f, 245.710f);
	m_fMouseSensor = 0.1f;
	/* 추가적으로 초기화가 필요하다면 수행해준다. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;
	

	//m_pTransformCom->Rotation(0.f, 180.f, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(418.755f, 1.5f, 245.710f, 1.f));
	m_iWeaponState = WEAPON_RIFLE;
	m_pBody = static_cast<CBody_Player*>(m_PartObjects[TPS_PART_BODY]);
	m_pWaepon = static_cast<CWeapon*>(m_PartObjects[TPS_PART_WEAPON]);
	m_pKatana = static_cast<CWeapon_Katana*>(m_PartObjects[TPS_PART_KATANA]);
	m_pFPS = static_cast<CPlayer_FPS*>(m_PartObjects[FPS_PART_BODY]);
	m_pHead = static_cast<CHead_Player*>(m_PartObjects[TPS_PART_HEAD]);


	CPivot* m_pTPSPivot = static_cast<CPivot*>(m_PartObjects[TPS_PART_PIVOT]);
	CFPS_Pivot* m_pFPSPivot = static_cast<CFPS_Pivot*>(m_PartObjects[FPS_PART_PIVOT]);

	m_vecTPS_CamPos = m_pTPSPivot->Get_TPS_CameraPos();
	m_vecFPS_CamPos = m_pFPSPivot->Get_FPS_CameraPos();

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{	
	if (m_pGameInstance->Get_DIKeyState_Down(DIK_C))
	{
		if (m_iViewState == PLAYER_TPS_VIEW)
			m_iViewState = PLAYER_FPS_VIEW;
		else
			m_iViewState = PLAYER_TPS_VIEW;
		
	}

	// 회전
	_long   MouseMove = { 0 };
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
	}

	m_vecPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (m_pGameInstance->Get_DIKeyState_Down(DIK_0))
	{

		m_iWeaponState++;
		if (m_iWeaponState > 7)
			m_iWeaponState = 0;
	}
	

	if(m_iViewState == PLAYER_FPS_VIEW)
	{
		// FPS
		m_pHead->Set_PlayerViewState(false);
		m_pFPS->Set_PlayerViewState(true);
		m_pBody->Set_PlayerViewState(false);
		const _float4x4* FPSMatrix = static_cast<CBody_Player*>(m_PartObjects[TPS_PART_BODY])->Get_SocketMatrix("hand_R_SKEL");
		m_pWaepon->Set_SocketMatrix(FPSMatrix);
	}
	else if(m_iViewState == PLAYER_TPS_VIEW)
	{
		 // TPS
		m_pHead->Set_PlayerViewState(true);
		m_pFPS->Set_PlayerViewState(false);
		m_pBody->Set_PlayerViewState(true);
		const _float4x4* TPSMatrix = static_cast<CBody_Player*>(m_PartObjects[TPS_PART_BODY])->Get_SocketMatrix("hand_R_SKEL");
		m_pWaepon->Set_SocketMatrix(TPSMatrix);


	}
	// 카메라 At 보내주기
	m_pWaepon->Set_CameraAt(m_vecCameraAt);
	// 몸에게 무기 상태 보내주기  FPS
	m_pFPS->Set_WeaponState(m_iWeaponState);
	// 몸에게 무기 상태 보내주기   TPS
	m_pBody->Set_WeaponState(m_iWeaponState);		
	// 무기에게 무기 상태 보내주기
	m_pWaepon->Set_WeaponState(m_iWeaponState);	
	// 칼에게 무기 상태 보내주기
	if (m_iWeaponState == WEAPON_KATANA)			
	{
		m_pKatana->Set_KatanaState(true);
	}
	else
		m_pKatana->Set_KatanaState(false);

	Player_Movement(fTimeDelta);					// 플레이어 동작





	__super::Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	
	__super::Late_Update(fTimeDelta);

}
HRESULT CPlayer::Render()
{

	return S_OK;
}
HRESULT CPlayer::Add_Components()
{


	return S_OK;
}
HRESULT CPlayer::Add_PartObjects()
{
	
	/* For.Body */
	CBody_Player::BODY_PLAYER_DESC BodyDesc{};
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyDesc.fSpeedPerSec = 0.f;
	BodyDesc.fRotationPerSec = 0.f;
	BodyDesc.pParentState_Upper = &m_iState_Upper;
	BodyDesc.pParentState_Lower = &m_iState_Lower;
	BodyDesc.m_iViewState = &m_iViewState;
	if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_Player"), TPS_PART_BODY, &BodyDesc)))
		return E_FAIL;

	CPlayer_FPS::FPS_PLAYER_DESC FPSDesc{};
	FPSDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	FPSDesc.fSpeedPerSec = 0.f;
	FPSDesc.fRotationPerSec = 0.f;
	FPSDesc.pParentState = &m_iState_Upper;
	FPSDesc.m_iViewState = &m_iViewState;

	if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_FPSBody_Player"), FPS_PART_BODY, &FPSDesc)))
		return E_FAIL;

	/* For.Body */
	CHead_Player::HEADPLAYER_DESC HeadDesc{};
	HeadDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	HeadDesc.fSpeedPerSec = 0.f;
	HeadDesc.fRotationPerSec = 0.f;
	HeadDesc.pParentState = &m_iState_Upper;
	HeadDesc.pSocketMatrix = static_cast<CBody_Player*>(m_PartObjects[TPS_PART_BODY])->Get_SocketMatrix("head_SKEL");
	HeadDesc.m_iViewState = &m_iViewState;

		/*head_SKEL*/
	if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Head_Player"), TPS_PART_HEAD, &HeadDesc)))
		return E_FAIL;
	
	CWeapon::WEAPON_DESC	WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	WeaponDesc.fSpeedPerSec = 0.f;
	WeaponDesc.fRotationPerSec = 0.f;
	WeaponDesc.pParentState = &m_iState_Upper;
	WeaponDesc.pSocketMatrix = static_cast<CBody_Player*>(m_PartObjects[TPS_PART_BODY])->Get_SocketMatrix("hand_R_SKEL");
	WeaponDesc.m_iViewState = &m_iViewState;

	if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Weapon"), TPS_PART_WEAPON, &WeaponDesc)))
		return E_FAIL;

	CWeapon_Katana::KATANA_DESC	KatanaDesc{};
	KatanaDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	KatanaDesc.fSpeedPerSec = 0.f;
	KatanaDesc.fRotationPerSec = 0.f;
	KatanaDesc.pParentState = &m_iState_Upper;
	KatanaDesc.pSocketMatrix = static_cast<CBody_Player*>(m_PartObjects[TPS_PART_BODY])->Get_SocketMatrix("hand_R_SKEL");
	KatanaDesc.m_iViewState = &m_iViewState;

	if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Katana"), TPS_PART_KATANA, &KatanaDesc)))
		return E_FAIL;

	CPivot::PIVOT_DESC	PivotDesc{};
	PivotDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	PivotDesc.fSpeedPerSec = 0.f;
	PivotDesc.fRotationPerSec = 0.f;
	PivotDesc.pParentState = &m_iState_Upper;
	PivotDesc.pSocketMatrix = static_cast<CBody_Player*>(m_PartObjects[TPS_PART_BODY])->Get_SocketMatrix("Camera");
	PivotDesc.m_iViewState = &m_iViewState;

	if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Pivot"), TPS_PART_PIVOT, &PivotDesc)))
		return E_FAIL;

	CFPS_Pivot::FPSPIVOT_DESC	FPSPivotDesc{};
	FPSPivotDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	FPSPivotDesc.fSpeedPerSec = 0.f;
	FPSPivotDesc.fRotationPerSec = 0.f;
	FPSPivotDesc.pParentState = &m_iState_Upper;
	FPSPivotDesc.pSocketMatrix = static_cast<CBody_Player*>(m_PartObjects[TPS_PART_BODY])->Get_SocketMatrix("root");
	FPSPivotDesc.m_iViewState = &m_iViewState;

	if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_FPSPivot"), FPS_PART_PIVOT, &FPSPivotDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{

	return S_OK;
}

void CPlayer::Player_Movement(_float fTimeDelta)
{
	_uint iJumpState = m_pBody->Get_JumpState();	// 점프 상태 가져오기
	if (iJumpState == LANDING_STATE) // 점프중엔 IDLE 상태 안되어야함
	{
		m_iState_Lower = STATE_IDLE;
		m_pTransformCom->Set_Min_Height();
	}
	if (m_pBody->Get_UpperBody_AnimState() == true)
	{
		m_iState_Upper = STATE_IDLE;
	}
	if (m_pGameInstance->Get_DIKeyState_Down(DIK_R))
	{
		if(m_iWeaponState != WEAPON_KATANA)
		{
			if (!(m_iState_Upper & RELOADING))
			{
				if (m_iState_Upper & STATE_IDLE)
					m_iState_Upper ^= STATE_IDLE;
				m_iState_Upper |= RELOADING;
			}
		}
		else
		{
			m_iState_Upper |= STATE_IDLE;
		}
	}
	if (m_pGameInstance->Get_DIMouseState_Pressing(DIM_LB))
	{
		if (!(m_iState_Upper & FIRE))
		{
			if (m_iState_Upper & STATE_IDLE)
				m_iState_Upper ^= STATE_IDLE;
			m_iState_Upper |= FIRE;
		}
	}
	if (m_pGameInstance->Get_DIMouseState_Up(DIM_LB))
	{
		m_iState_Upper = STATE_IDLE;
	}


	if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_S))
	{
		if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_A))
		{
			m_pTransformCom->Go_Left(fTimeDelta * 0.5f);
			m_pTransformCom->Go_Backward(fTimeDelta * 0.5f);
			if (iJumpState == LANDING_STATE)
			{
				if (!(m_iState_Lower & WALKSTATE_SOUTHWEST))
				{
					if (m_iState_Lower & STATE_IDLE)
						m_iState_Lower ^= STATE_IDLE;
					m_iState_Lower |= WALKSTATE_SOUTHWEST;
				}
			}
		}
		else if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_D))
		{
			m_pTransformCom->Go_Right(fTimeDelta * 0.5f);
			m_pTransformCom->Go_Backward(fTimeDelta * 0.5f);
			if (iJumpState == LANDING_STATE)
			{
				if (!(m_iState_Lower & WALKSTATE_SOUTHEAST))
				{
					if (m_iState_Lower & STATE_IDLE)
						m_iState_Lower ^= STATE_IDLE;
					m_iState_Lower |= WALKSTATE_SOUTHEAST;
				}
			}
		}
		else
		{
			m_pTransformCom->Go_Backward(fTimeDelta);
			if (iJumpState == LANDING_STATE)
			{
				if (!(m_iState_Lower & WALKSTATE_SOUTH))
				{
					if (m_iState_Lower & STATE_IDLE)
						m_iState_Lower ^= STATE_IDLE;
					m_iState_Lower |= WALKSTATE_SOUTH;
				}
			}
		}
	}


	if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_LSHIFT))
	{
		if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_W))
		{
			if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_A))
			{
				m_pTransformCom->Go_Left(fTimeDelta * 0.8f);
				m_pTransformCom->Go_Straight(fTimeDelta, 1.5f);
				if (iJumpState == LANDING_STATE)
				{
					if (!(m_iState_Lower & RUNSTATE_NORTHWEST))
					{
						if (m_iState_Lower & STATE_IDLE)
							m_iState_Lower ^= STATE_IDLE;
						m_iState_Lower |= RUNSTATE_NORTHWEST;
					}
				}
			}
			else if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_D))
			{
				m_pTransformCom->Go_Right(fTimeDelta * 0.8f);
				m_pTransformCom->Go_Straight(fTimeDelta, 1.5f);
				if (iJumpState == LANDING_STATE)
				{
					if (!(m_iState_Lower & RUNSTATE_NORTHEAST))
					{
						if (m_iState_Lower & STATE_IDLE)
							m_iState_Lower ^= STATE_IDLE;
						m_iState_Lower |= RUNSTATE_NORTHEAST;
					}
				}
			}
			else
			{
				m_pTransformCom->Go_Straight(fTimeDelta, 2.f);
				if (iJumpState == LANDING_STATE)
				{
					if (!(m_iState_Lower & RUNSTATE_NORTH))
					{
						if (m_iState_Lower & STATE_IDLE)
							m_iState_Lower ^= STATE_IDLE;
						m_iState_Lower |= RUNSTATE_NORTH;
					}
				}
			}

		}
		if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_A))
		{
			m_pTransformCom->Go_Left(fTimeDelta);
			if (iJumpState == LANDING_STATE)
			{
				if (!(m_iState_Lower & WALKSTATE_WEST))
				{
					if (m_iState_Lower & STATE_IDLE)
						m_iState_Lower ^= STATE_IDLE;
					m_iState_Lower |= WALKSTATE_WEST;
				}
			}
		}
		if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_D))
		{

			if (iJumpState == LANDING_STATE)
			{
				m_pTransformCom->Go_Right(fTimeDelta);
				if (!(m_iState_Lower & WALKSTATE_EAST))
				{
					if (m_iState_Lower & STATE_IDLE)
						m_iState_Lower ^= STATE_IDLE;
					m_iState_Lower |= WALKSTATE_EAST;
				}
			}

		}
	}
	else
	{
		if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_W))
		{
			if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_A))
			{
				if (iJumpState == LANDING_STATE)
				{
					m_pTransformCom->Go_Straight(fTimeDelta * 0.7f);
					m_pTransformCom->Go_Left(fTimeDelta * 0.7f);
					if (!(m_iState_Lower & WALKSTATE_NORTHWEST))
					{
						if (m_iState_Lower & STATE_IDLE)
							m_iState_Lower ^= STATE_IDLE;
						m_iState_Lower |= WALKSTATE_NORTHWEST;
					}
				}
			}
			else if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_D))
			{
				if (iJumpState == LANDING_STATE)
				{
					m_pTransformCom->Go_Straight(fTimeDelta * 0.7f);
					m_pTransformCom->Go_Right(fTimeDelta * 0.7f);
					if (!(m_iState_Lower & WALKSTATE_NORTHEAST))
					{
						if (m_iState_Lower & STATE_IDLE)
							m_iState_Lower ^= STATE_IDLE;
						m_iState_Lower |= WALKSTATE_NORTHEAST;
					}
				}
			}
			else
			{
				m_pTransformCom->Go_Straight(fTimeDelta);
				if (iJumpState == LANDING_STATE)
				{
					if (!(m_iState_Lower & WALKSTATE_NORTH))
					{
						if (m_iState_Lower & STATE_IDLE)
							m_iState_Lower ^= STATE_IDLE;
						m_iState_Lower |= WALKSTATE_NORTH;
					}
				}
			}
		}
		else
		{
			if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_A))
			{

				m_pTransformCom->Go_Left(fTimeDelta);
				if (iJumpState == LANDING_STATE)
				{
					if (!(m_iState_Lower & WALKSTATE_WEST))
					{
						if (m_iState_Lower & STATE_IDLE)
							m_iState_Lower ^= STATE_IDLE;
						m_iState_Lower |= WALKSTATE_WEST;
					}
				}
			}
			if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_D))
			{

				if (iJumpState == LANDING_STATE)
				{
					m_pTransformCom->Go_Right(fTimeDelta);
					if (!(m_iState_Lower & WALKSTATE_EAST))
					{
						if (m_iState_Lower & STATE_IDLE)
							m_iState_Lower ^= STATE_IDLE;
						m_iState_Lower |= WALKSTATE_EAST;
					}
				}

			}
		}


	}
	
		if ((m_pGameInstance->Get_DIKeyState_Down(DIK_SPACE)) && iJumpState == LANDING_STATE)  // 점프 시작
		{
			iJumpState = JUMPING_START_STATE;
			m_bJumpStart = false;
			m_fPower = 0.f;
			if (!(m_iState_Lower & JUMP_START))
			{
				{
					if (m_iState_Lower & STATE_IDLE)
						m_iState_Lower ^= STATE_IDLE;
					m_iState_Lower |= JUMP_START;
				}
			}
		}
		else if (iJumpState == JUMPING_LOOP_STATE)  // 점프 루프 시작
		{
			if (m_bJumpStart == false)
			{
				m_fPower = 15.f;
				m_bJumpStart = true;
			}
			if (!(m_iState_Lower & JUMP_LOOP))
			{
				if (m_iState_Lower & STATE_IDLE)
					m_iState_Lower ^= STATE_IDLE;
				m_iState_Lower |= JUMP_LOOP;
			}
		}
		else if (iJumpState == JUMPING_END_STATE)  // 점프 마무리
		{
			if (!(m_iState_Lower & JUMP_END))
			{
				if (m_iState_Lower & STATE_IDLE)
					m_iState_Lower ^= STATE_IDLE;
				m_iState_Lower |= JUMP_END;
			}
		}
		if (iJumpState >= 2)
		{
			m_pTransformCom->Jump(fTimeDelta, m_fHeight, m_fPower, iJumpState);
			m_pBody->Set_JumpState(m_fHeight, m_fPower);
		}
	

}



CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

}
