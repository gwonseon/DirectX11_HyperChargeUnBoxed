#include "stdafx.h"
#include "..\Public\Truck_Missile.h"

#include "GameInstance.h"
#include <Effect_Explosion_Tank.h>
#include <Terrain_Crushed.h>

CTruck_Missile::CTruck_Missile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTruck_Missile::CTruck_Missile(const CTruck_Missile& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTruck_Missile::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTruck_Missile::Initialize(void* pArg)
{
	MISSILE_DESC* pDesc = static_cast<MISSILE_DESC*>(pArg);
	m_eLevel = pDesc->eID;
	m_pShooterModelIdx = pDesc->ShooterModelIdx;
	m_pPlayer = pDesc->pPlayer;
	m_pTracker = pDesc->m_pTracker;
	m_bShotStart = m_pTracker->Get_Shot();
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;
	
	m_vecPosition = StartPos = XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f);
	StartPos_Store = StartPos;
	m_pTransformCom->Set_Scaling(pDesc->fScale.x, pDesc->fScale.y, pDesc->fScale.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, StartPos);
	m_pTransformCom->Rotation(0.f,0.f, XMConvertToRadians(-90.f));
	m_fAngle = -90.f;
	fRotX = 0.f;
	m_eMissile_State = MISSILE_IDLE;


	// Tracker 위치 저장해두기, 여기서 위치 보내줄 것임
	DWORD dwByte = 0;
	_uint iLevel{};
	_float3 fPos{};
	_wstring StartingPosPath = TEXT("../Bin/Data/TrackerPos.dat");
	HANDLE hFile = CreateFile(StartingPosPath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"Load TrackerPos File Failed", L"Error", MB_OK);
		return E_FAIL;
	}
	while (ReadFile(hFile, &iLevel, sizeof(LEVELID), &dwByte, nullptr) && dwByte > 0)
	{
		ReadFile(hFile, &fPos, sizeof(_float3), &dwByte, nullptr);
		m_vecStartingPos.push_back(fPos);
	}
	CloseHandle(hFile);


	CMissile_Flame::MISSILE_FLAME_DESC pFlame{};
	pFlame.eLevel = LEVEL_YARD;
	pFlame.fScale = pDesc->fScale ;
	pFlame.vecPos = &m_vecPosition;
	pFlame.bDraw  = &m_bDraw;
	pFlame.iTexNum = 5;
//	pFlame.matWorld = m_pTransformCom->Get_WorldMatrixPtr();
	m_pMissile_Flame = static_cast<CMissile_Flame*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Missile_Flare"), &pFlame));

	return S_OK;
}

void CTruck_Missile::Priority_Update(_float fTimeDelta)
{
	m_vecPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_fSound = m_pGameInstance->Sound_Cal(m_vecPosition);

	if (m_bKnockdown == true)
	{
		if (XMVectorGetY(m_vecPosition) >= 0.05f)
		{
			m_vecPosition = XMVectorSetY(m_vecPosition, XMVectorGetY(m_vecPosition) - (fTimeDelta * 10.f));
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);

		}
		if (m_fAngle <= 0.f)
			m_fAngle += fTimeDelta * 30.f;

		if (fRotX <= 70.f)
			fRotX += fTimeDelta * 40.f;
		m_pTransformCom->Rotation(0.f, XMConvertToRadians(fRotX), XMConvertToRadians(m_fAngle));
		return;
	}
	
	

	// 미사일 떨어짐
	if (m_eMissile_State == MISSILE_SHOT_FALL)
	{
	
		// Tracker 위치 보내주기
		if(m_vecStartingPos.size() > 0)
		{
			_vector vTrackerPos = XMVectorSet(m_vecStartingPos.back().x,300.f, m_vecStartingPos.back().z, 1.f);
			m_vecStartingPos.erase(m_vecStartingPos.end() - 1);
			m_pTracker->Get_Transform()->Set_State(CTransform::STATE_POSITION, vTrackerPos);
		}
		m_pTracker->Set_Fall(true);
		// 초기화
		*m_bShotStart = false; 
		m_eMissile_State = MISSILE_IDLE;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, StartPos_Store); // 미사일 발사대로 복귀
		m_fAngle = -90.f; // 미사일 회전값 초기화
		m_pTransformCom->Rotation(0.f, 0.f, XMConvertToRadians(m_fAngle));
	}
	if (*m_bShotStart == false) 
	{
		m_eMissile_State = MISSILE_IDLE;
	}
}

void CTruck_Missile::Update(_float fTimeDelta)
{
	if (m_bKnockdown == true)
		return;

	if (*m_bShotStart == true && m_eMissile_State == MISSILE_IDLE)
	{
		m_fSpeed = 40.f;
		m_eMissile_State = MISSILE_SHOT_START;
		m_bDraw = true;
	}
	if (m_eMissile_State == MISSILE_SHOT_ACCEL)
	{
		if (XMVectorGetY(m_vecPosition) <= 0.f)
		{
			m_eMissile_State = MISSILE_SHOT_FALL;
			// 폭발 생성
			CExplosion::EXPLOSION_DESC pExplosion{};
			pExplosion.eID = m_eLevel;
			pExplosion.eType = CExplosion::EXPLOSION_TRUCK;
			pExplosion.fPosition = _float3{ XMVectorGetX(m_vecPosition), XMVectorGetY(m_vecPosition) ,XMVectorGetZ(m_vecPosition) };
			pExplosion.fScale = _float3{ 10.f, 10.f, 10.f };
			m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel, TEXT("Layer_Explosion"), TEXT("Prototype_GameObject_Explosion"), &pExplosion);
		
			CEffect_Explosion_Tank::EFFECT_Tank_Explosion_DESC Effect{};
			Effect.eLevel = m_eLevel;
			Effect.fScale = _float3{ 80.f, 80.f, 80.f };
			Effect.eType = CEffect_Explosion_Tank::EXPLOSION_MISSILE;
			Effect.fPosition = _float3{ XMVectorGetX(m_vecPosition), XMVectorGetY(m_vecPosition) + 15.f, XMVectorGetZ(m_vecPosition) };
			m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Tank_Explosion"), &Effect);

			CTerrain_Crushed::TERRAIN_CRUSHED_DESC pCrushed{};
			pCrushed.eLevel = m_eLevel;
			pCrushed.fPosition = _float3{ XMVectorGetX(m_vecPosition), XMVectorGetY(m_vecPosition) ,XMVectorGetZ(m_vecPosition) };
			pCrushed.fScale = _float3{ 20.f, 20.f, 20.f };
			pCrushed.pPlayer = m_pPlayer;
			m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Terrain_Crushed"), &pCrushed);

			m_pGameInstance->StopSound(SOUND_MISSIE_EXPLOSION);
			m_pGameInstance->PlaySoundW(L"FE_MissileTruck_Explosion_Nuke.wav", Engine::CHANNELID::SOUND_MISSIE_EXPLOSION, m_fSound + 0.1f);
			m_pGameInstance->PlaySoundW(L"FE_Explosion_Big_Close_01.wav", Engine::CHANNELID::SOUND_ALIEN_SPEAK, 0.4f);


			m_bDraw = false;
			m_bFog = true;
			m_fFogEnd = 1.01f;
		}
		_vector vUP = { 0.f, 1.f, 0.f,0.f };
		m_vecPosition += vUP * fTimeDelta * m_fPower;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);

		_vector vTrackerPos = m_pTracker->Get_Pos();	// 추적기 위치
		vTrackerPos = XMVectorSetY(vTrackerPos, 0.f); // 추적기 높이 없애기
		_vector vecNoHeight_Pos = m_vecPosition;		// 현재 위치에서 높이 없앤 값
		vecNoHeight_Pos = XMVectorSetY(vecNoHeight_Pos, 0.f); 

		_vector vDir = vTrackerPos - vecNoHeight_Pos; // 방향
		vDir = XMVector3Normalize(vDir);		
		_float fDistance = m_pTransformCom->Cal_Distance_vec_No_Height(m_vecPosition, vTrackerPos);
		if(fDistance >= 10.f)
		{
			m_vecPosition += vDir * fTimeDelta * m_fSpeed;  // 다음 위치
		}

		// 평면 이동
		if(m_bMoving == true)
		{
			if (m_fAngle >= -270.f)
				m_fAngle -= 0.8f;

			// 플레이어가 줍지 않았을 때 중간 위치
			_vector vDir_For_Mid = StartPos;
			StartPos = XMVectorSetY(StartPos, 0.f);
			vDir_For_Mid = XMVectorSetY(vDir_For_Mid, 0.f);
			vDir_For_Mid = vTrackerPos - vDir_For_Mid; // 추적기 방향
			PrevPos = m_vecPosition; // 이전 위치
			vDir_For_Mid = XMVector3Normalize(vDir_For_Mid);

			if(m_bMidArrived == false)// 플레이어가 줍지 않았을 때 중간 위치에 도달하면
			{
				m_fSpeed += 0.5f;
				// 중간 위치 계산
				_float fDistance = m_pTransformCom->Cal_Distance_vec_No_Height(StartPos, vTrackerPos);
				fDistance = sqrt(fDistance);
				MidPos = StartPos + (vDir_For_Mid) * (fDistance * 0.6f);
				if (m_pTransformCom->IsPass_TargetPosition(PrevPos, m_vecPosition, MidPos) == true) // 중간 위치 도달시
				{
					m_bMidArrived = true;
				}
			}
			else
			{
				m_fPower = 0;
				m_fPower -= 2.f;
			}
			if (XMVectorGetY(m_vecPosition) >= 1000.f)
			{
				m_fPower -= 1.f;
			}
			// 위치 도달시 움직임 평행 이동 X
			if (m_pTransformCom->IsPass_TargetPosition(PrevPos, m_vecPosition, vTrackerPos) == true) 
			{
				m_bDraw = false;
				m_fPower -= 40.f;
				m_vecPosition = PrevPos;
				m_bMoving = false;
			}
			// 대각선 거리가 30안으로 들어왔을 때 떨어져랏
			if (fDistance <= 30.f)
			{
				m_bDraw = false;
				m_fPower -= 40.f;
				m_vecPosition = PrevPos;
				m_bMoving = false;
			}
		}
		else
		{

			if (m_fAngle >= -270.f)
				m_fAngle -= 1.6f;
			m_fPower -= 2.f; 
		}
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);
		m_pTransformCom->Rotation(0.f, 0.f, XMConvertToRadians(m_fAngle));
	}
	// 발사 시작 ( 계속 하늘로 올라가기 )
	if (m_eMissile_State == MISSILE_SHOT_START)
	{
		if(m_bFire == false)
		{
			m_pGameInstance->StopSound(SOUND_MISSILE);
			m_pGameInstance->PlaySoundW(L"FE_MissileTruck_WarningVoice.wav", Engine::CHANNELID::SOUND_MISSILE, 0.4f);
			m_bFire = true;
		}
		m_pGameInstance->PlaySoundW(L"FE_MissileTruck_RocketLaunch.wav", Engine::CHANNELID::SOUND_MISSILE_FLAME, m_fSound);

		PrevPos = m_vecPosition;
		m_vecPosition = XMVectorSetY(m_vecPosition, XMVectorGetY(m_vecPosition) + fTimeDelta * m_fSpeed);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPosition);
		m_fSpeed += 3.f;
		if (XMVectorGetY(m_vecPosition) >= 500.f)
		{
			m_pGameInstance->StopSound(SOUND_MISSILE_FLAME);
			m_eMissile_State = MISSILE_SHOT_ACCEL;
			m_fPower = 140.f;
			m_fSpeed = 60.f;
			m_bMoving = true;
			m_bMidArrived = false;
		}
	}

}

void CTruck_Missile::Late_Update(_float fTimeDelta)
{
	// 안개 조절 
	m_pGameInstance->Set_Fog(m_bFog, m_fFogEnd);
	if(m_bFog== true)
	{
		m_fFogEnd += fTimeDelta * 0.5f;
		if (m_fFogEnd >= 10.f)
			m_bFog = false;
	}
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_HEIGHT, this)))
		return;
	if (m_bKnockdown == true)
	{
		m_bDraw = false; 
		if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
			return;
	}
	else
	{
		if (*m_pShooterModelIdx == 1)
		{
			if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
				return;
		}
	}
}

HRESULT CTruck_Missile::Render()
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

HRESULT CTruck_Missile::Render_Height()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4			ViewMatrix, ProjMatrix;
	CLayer* pPlayerLayer = (m_pGameInstance->Find_Layer(LEVEL_YARD, TEXT("Layer_Player")));
	CPlayer* pPlayer = static_cast<CPlayer*>(pPlayerLayer->Get_GameObject_List().front());
	_vector PlayerPos = pPlayer->Get_Position();
	_matrix			matView = XMMatrixIdentity();
	matView.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	matView.r[1] = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	matView.r[2] = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	matView.r[3] = XMVectorSet(XMVectorGetX(PlayerPos), XMVectorGetY(PlayerPos) + 6.f, XMVectorGetZ(PlayerPos), 1.f);

	XMStoreFloat4x4(&ViewMatrix, XMMatrixInverse(nullptr, matView));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixOrthographicLH(200.f, 200.f, 0.f, 30.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	_float fFar = m_pGameInstance->Get_CameraFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(1))) // 무조건 그림
			return E_FAIL;
		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CTruck_Missile::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxItem"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	const _wstring Model_Component = TEXT("Prototype_Component_Model_Bullet");
	const _wstring Model_Component_Result = Model_Component + to_wstring(4);
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTruck_Missile::Bind_ShaderResources()
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
	return S_OK;
}

CTruck_Missile* CTruck_Missile::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTruck_Missile* pInstance = new CTruck_Missile(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTruck_Missile");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTruck_Missile::Clone(void* pArg)
{
	CTruck_Missile* pInstance = new CTruck_Missile(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CTruck_Missile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTruck_Missile::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
