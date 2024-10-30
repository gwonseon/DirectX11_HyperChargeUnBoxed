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
	Desc.fSpeedPerSec = 25.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.f);
	Desc.fPosition = _float3(386.295f, 1.f, 450.425f);
	m_fMouseSensor = 0.1f;
	PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);
	m_vecCameraAt = pDesc->vCameraAt;
	m_vecCameraPos = pDesc->vCameraPos;
	m_iRound = pDesc->iRound;
 
	/* 추가적으로 초기화가 필요하다면 수행해준다. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;
	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(386.295f, 1.f, 450.425f, 1.f));
//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(Desc.fPosition.x, Desc.fPosition.y, Desc.fPosition.z, 1.f));
	m_pTransformCom->Set_Scaling(2.f, 2.f, 2.f);
	m_iWeaponState = WEAPON_RIFLE;
	m_pWaepon = static_cast<CWeapon*>(m_PartObjects[TPS_PART_WEAPON]);
	m_pKatana = static_cast<CWeapon_Katana*>(m_PartObjects[TPS_PART_KATANA]);
	m_pHead = static_cast<CHead_Player*>(m_PartObjects[TPS_PART_HEAD]);

	CPivot* m_pTPSPivot = static_cast<CPivot*>(m_PartObjects[TPS_PART_PIVOT]);
	CFPS_Pivot* m_pFPSPivot = static_cast<CFPS_Pivot*>(m_PartObjects[FPS_PART_PIVOT]);

	m_vecTPS_CamPos = m_pTPSPivot->Get_TPS_CameraPos();
	m_vecFPS_CamPos = m_pFPSPivot->Get_FPS_CameraPos();
	m_vecWeaponPos = m_pWaepon->Get_WeaponPos();
	m_vecWeaponDir = m_pWaepon->Get_WeaponDir();


	m_fHp = 100.f;
	m_fEnergy = 100.f;
	m_fAttack = 10.f;
	m_iCoin = 0;
	m_bDontDestroy = true;
	m_bKnockdown = false;

	m_fRun_FourDirection = 1.5f;
	m_fRun_EightDirection = m_fRun_FourDirection * 0.5f;

	m_bBuildMode = true; 
	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	if (m_bBuildMode == true && m_bBuild_Able == true)
	{
		if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_E))
		{
			m_bBuild_Gauging = true;
		}
		else
			m_bBuild_Gauging = false;
	}
	else
	{
		m_bBuild_Gauging = false;
	}
	if (m_pGameInstance->Get_DIKeyState_Down(DIK_C))
	{
		if (m_iViewState == PLAYER_TPS_VIEW)
			m_iViewState = PLAYER_FPS_VIEW;
		else
			m_iViewState = PLAYER_TPS_VIEW;
	}
	if (m_pGameInstance->Get_DIKeyState_Down(DIK_F) && *m_bRoundStart == false)
	{
		if (m_bBuildMode == false)
			m_bBuildMode = true;
		else
			m_bBuildMode = false;
	}
	// 회전
	_long   MouseMove = { 0 };
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
	}

	m_vecPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 pos{};
	XMStoreFloat3(&pos, m_vecPos);
#pragma region 지우ㅡㅓ
	if(m_pGameInstance->Get_DIKeyState_Down(DIK_P))
	{
		cout << "Cell : " << m_pNavigationCom->Get_CurrentCell_Index() << endl;
		cout << pos.x << "     " << pos.y << "     " << pos.z << endl;
	}
	if (m_pGameInstance->Get_DIKeyState_Down(DIK_0))
	{
		m_iWeaponState++;
		if (m_iWeaponState > 7)
			m_iWeaponState = 0;
	}
#pragma endregion 지우ㅡㅓ	
	if(m_iViewState == PLAYER_FPS_VIEW)
	{
		
		// FPS
		m_pHead->Set_PlayerViewState(false); 
		m_pBody->Set_PlayerViewState(false);
		m_pWaepon->Set_TPSState(false);
	}
	else if(m_iViewState == PLAYER_TPS_VIEW)
	{
		 // TPS
		m_pHead->Set_PlayerViewState(true);
		m_pBody->Set_PlayerViewState(true);
		m_pWaepon->Set_TPSState(true);
	}

	if (m_bReloading == true) // 장전이 참일 때 
	{
		m_fReload_Charging += fTimeDelta; // 장전 시간
	}
	if (m_fReload_Charging >= 1.f)  // 장전 시간이 다 끝났을 때
	{
		m_fReload_Charging = 0.f;
		m_bReloading = false;			// 장전 false
		m_pWaepon->Set_BulletIn(true);	// 총한테 장전되었다고 알려주기
	}
	m_pWaepon->Set_CameraPos(m_vecCameraPos);			// 카메라 At 보내주기
	m_pWaepon->Set_CameraAt(m_vecCameraAt);			// 카메라 At 보내주기
	m_pBody->Set_WeaponState(m_iWeaponState);		// 몸에게 무기 상태 보내주기   TPS	
	m_pWaepon->Set_WeaponState(m_iWeaponState);		// 무기에게 무기 상태 보내주기

	if (m_iWeaponState == WEAPON_KATANA)			// 칼에게 무기 상태 보내주기	
	{
		m_iViewState = PLAYER_TPS_VIEW;				// 카타나는 무조건 3인칭 
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

	// 공격 당했을 때 무적상태 1초간 
	if (m_bCanAttacked == false)
		m_fInvincibleTime += fTimeDelta;
	if (m_fInvincibleTime >= 1.f)
	{
		m_bCanAttacked = true;
		m_fInvincibleTime = 0.f;
	}

}

void CPlayer::Late_Update(_float fTimeDelta)
{

	__super::Late_Update(fTimeDelta);

	if(m_bDead == false)
	{
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
			return;
	}

}
HRESULT CPlayer::Render()
{
#ifdef _DEBUG
	
	m_pNavigationCom->Render();
#endif
	return S_OK;
}
HRESULT CPlayer::Add_Components()
{
	// For.Com_Navigation
	CNavigation::NAVIGATION_DESC		Desc{};

	Desc.iCurrentCellIndex = 7;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
		return E_FAIL;

	

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
	BodyDesc.m_bAttackState = &m_bAttackState;
	if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_Player"), TPS_PART_BODY, &BodyDesc)))
		return E_FAIL;

	m_pBody = static_cast<CBody_Player*>(m_PartObjects[TPS_PART_BODY]);

	/* For.Body */
	CHead_Player::HEADPLAYER_DESC HeadDesc{};
	HeadDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	HeadDesc.fSpeedPerSec = 0.f;
	HeadDesc.fRotationPerSec = 0.f;
	HeadDesc.pParentState = &m_iState_Upper;
	HeadDesc.pSocketMatrix = static_cast<CBody_Player*>(m_PartObjects[TPS_PART_BODY])->Get_SocketMatrix("chest_SKEL");
	HeadDesc.m_iViewState = &m_iViewState;
	HeadDesc.m_iWeaponState = &m_iWeaponState;
		/*head_SKEL*/
	if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Head_Player"), TPS_PART_HEAD, &HeadDesc)))
		return E_FAIL;
	
	CWeapon::WEAPON_DESC	WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	WeaponDesc.fSpeedPerSec = 0.f;
	WeaponDesc.fRotationPerSec = XMConvertToRadians(40.0f);
	WeaponDesc.pParentState = &m_iState_Upper;
	WeaponDesc.pSocketMatrix = static_cast<CBody_Player*>(m_PartObjects[TPS_PART_BODY])->Get_SocketMatrix("hand_R_SKEL");
	WeaponDesc.m_iViewState = &m_iViewState;
	WeaponDesc.vCameraAt = m_vecCameraAt;
	WeaponDesc.vCameraPos = m_vecCameraPos;
	WeaponDesc.bShotStart =  Get_ShotStart();
	WeaponDesc.bReload = &m_bReloading;
	WeaponDesc.fReloadingTime = &m_fReload_Charging;
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
	FPSPivotDesc.pSocketMatrix = static_cast<CBody_Player*>(m_PartObjects[TPS_PART_BODY])->Get_SocketMatrix("Camera");
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
			m_fReload_Charging = 0.f;
			m_bReloading = true;
			if (m_iViewState == PLAYER_FPS_VIEW)
			{
				// 아무것도 하지마 ( 1인칭 모션이 없음 ) 
			}
			else
			{
				// 장전 동작해라  
				if (!(m_iState_Upper & RELOADING))
				{
					if (m_iState_Upper & STATE_IDLE)
						m_iState_Upper ^= STATE_IDLE;
					m_iState_Upper |= RELOADING;
				}
			}
		}
		else
		{
			m_iState_Upper |= STATE_IDLE;
		}
	}
	if (m_pGameInstance->Get_DIMouseState_Pressing(DIM_LB) && m_bReloading == false)
	{
		if (m_iWeaponState == WEAPON_KATANA)
			m_bAttackState = true;
		if (!(m_iState_Upper & FIRE))
		{
			if (m_iState_Upper & STATE_IDLE)
				m_iState_Upper ^= STATE_IDLE;
			m_iState_Upper |= FIRE;
		}
	}
	else if (m_pGameInstance->Get_DIMouseState_Pressing(DIM_LB) && m_bReloading == true && m_iViewState == PLAYER_FPS_VIEW)
	{
		m_iState_Upper = STATE_IDLE;
	}
	if (m_pGameInstance->Get_DIMouseState_Up(DIM_LB)) 
	{
		if (m_iWeaponState != WEAPON_KATANA)
		{
			m_iState_Upper = STATE_IDLE;
		}
	}
	if (m_pGameInstance->Get_DIMouseState_Pressing(DIM_RB))
	{
		m_bAttackState = true;
		if (!(m_iState_Upper & FIRE_RB))
		{
			if (m_iState_Upper & STATE_IDLE)
				m_iState_Upper ^= STATE_IDLE;
			m_iState_Upper |= FIRE_RB;
		}
	}


	if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_V) && m_iViewState == PLAYER_TPS_VIEW)
	{
		if (!(m_iState_Upper & MELEE))
		{
			if (m_iState_Upper & STATE_IDLE)
				m_iState_Upper ^= STATE_IDLE;
			m_iState_Upper |= MELEE;
		}
	}



	if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_S))
	{
		// 뒤 왼쪽으로 걷기 
		if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_A))
		{
			
			if (iJumpState == LANDING_STATE)
			{
				m_pTransformCom->Go_Left_Nav(fTimeDelta , m_pNavigationCom);
				m_pTransformCom->Go_Backward_Nav(fTimeDelta , m_pNavigationCom);
				if (!(m_iState_Lower & WALKSTATE_SOUTHWEST))
				{
					if (m_iState_Lower & STATE_IDLE)
						m_iState_Lower ^= STATE_IDLE;
					m_iState_Lower |= WALKSTATE_SOUTHWEST;
				}
			}

		}
		// 뒤 오른쪽 으로 걷기
		else if (m_pGameInstance->Get_DIKeyState_Pressing(DIK_D))
		{
			m_pTransformCom->Go_Right_Nav(fTimeDelta , m_pNavigationCom);
			m_pTransformCom->Go_Backward_Nav(fTimeDelta , m_pNavigationCom);
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
		// 그냥 뒤로 걷기
		else
		{
			m_pTransformCom->Go_Backward_Nav(fTimeDelta, m_pNavigationCom);
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
				m_pTransformCom->Go_Left_Nav(fTimeDelta * m_fRun_EightDirection, m_pNavigationCom);
				m_pTransformCom->Go_Straight_Nav(fTimeDelta * m_fRun_EightDirection, m_pNavigationCom);
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
				m_pTransformCom->Go_Right_Nav(fTimeDelta * m_fRun_EightDirection, m_pNavigationCom);
			//	m_pTransformCom->Go_Straight(fTimeDelta, 1.5f);
				m_pTransformCom->Go_Straight_Nav(fTimeDelta * m_fRun_EightDirection, m_pNavigationCom);
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
			//	m_pTransformCom->Go_Straight(fTimeDelta, 2.f);
				m_pTransformCom->Go_Straight_Nav(fTimeDelta * m_fRun_FourDirection, m_pNavigationCom);
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
			m_pTransformCom->Go_Left_Nav(fTimeDelta, m_pNavigationCom);
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
			m_pTransformCom->Go_Right_Nav(fTimeDelta, m_pNavigationCom);
			if (iJumpState == LANDING_STATE)
			{
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
				m_pTransformCom->Go_Straight_Nav(fTimeDelta, m_pNavigationCom);
				m_pTransformCom->Go_Left_Nav(fTimeDelta * m_fRun_EightDirection, m_pNavigationCom);
				if (iJumpState == LANDING_STATE)
				{
				//	m_pTransformCom->Go_Straight(fTimeDelta * 0.7f);
					
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
				m_pTransformCom->Go_Straight_Nav(fTimeDelta, m_pNavigationCom);
				m_pTransformCom->Go_Right_Nav(fTimeDelta * m_fRun_EightDirection, m_pNavigationCom);
				if (iJumpState == LANDING_STATE)
				{
			//		m_pTransformCom->Go_Straight(fTimeDelta * 0.7f);
					
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
		//		m_pTransformCom->Go_Straight(fTimeDelta);
				m_pTransformCom->Go_Straight_Nav(fTimeDelta, m_pNavigationCom);
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
				m_pTransformCom->Go_Left_Nav(fTimeDelta, m_pNavigationCom);
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
				m_pTransformCom->Go_Right_Nav(fTimeDelta, m_pNavigationCom);
				if (iJumpState == LANDING_STATE)
				{
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

	
		if ((m_pGameInstance->Get_DIKeyState_Down(DIK_SPACE)) && iJumpState == LANDING_STATE && m_iJumpCount == 0)  // 점프 시작
		{
			iJumpState = JUMPING_START_STATE;

			m_bJumpStart = false;
			m_fPower = 0.f;
			m_iJumpCount = 1;
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
				m_fPower = 18.f;
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
			m_iJumpCount = 0;

		}
		if (iJumpState >= JUMPING_LOOP_STATE)
		{
			if(m_pGameInstance->Get_DIKeyState_Down(DIK_SPACE) && m_iJumpCount == 1)
			{
				m_fPower = 18.f;
				m_iJumpCount = 2;
			}
			m_pTransformCom->Jump(fTimeDelta, m_fHeight, m_fPower, iJumpState, m_iJumpCount);
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

	Safe_Release(m_pNavigationCom);
}
