#pragma once

#include "Client_Defines.h"
#include "Monster.h"
#include "Player_Build.h"
#include "RifleMan_State.h"
#include "Camera_Free.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CRifleMan : public CMonster
{
private:
	CRifleMan_State* m_pCurrentState;

public:
	typedef struct : CMonster::MONSTER_DESC
	{
		CCamera_Free* pCamera = { nullptr };
		CPlayer_Build* m_pBuild = { nullptr };
	}RIFLEMAN_DESC;

	enum RIFLEMAN_ANIM
	{
		 AA_ArmyMen_EndFire
		,AA_ArmyMen_LoopFir
		,AA_ArmyMen_Mov
		,AA_ArmyMen_Mov001
		,AA_ArmyMen_Move_01
		,AA_ArmyMen_Move_03
		,AA_ArmyMen_StartFi
		,AA_SpaceMen01_Fire
		,AA_SpaceMen02_Fire
	};

private:
	CRifleMan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRifleMan(const CRifleMan& Prototype);
	virtual ~CRifleMan() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();


public: // 상태패턴
	void ChangeState(CRifleMan_State* pNewState)
	{
		if (m_pCurrentState)
		{
			m_pCurrentState->Exit(this);
			delete m_pCurrentState;
		}

		m_pCurrentState = pNewState;

		if (m_pCurrentState)
		{
			m_pCurrentState->Enter(this);
		}
	}
public:
	CModel* Get_ModelCom() { return m_pModelCom; }
	_bool	Get_MoveAnimState() { return m_bMove_Anim; }

private:
	_vector vPlayerPos{};
	_float3 m_fPos{};

	_float	m_fMoveTime = 0.f;
	_float	m_fMoveSpeed = 0.f;
	_float	m_fTime_For_Target = 0.f;  // 트랩 찾는 경로 탐색 지연 시간
	_float	m_fShotTimer = 0.f;

	_float		m_iShot_Count = 0; // 3발 쏘기 위해 몇 발 쐈는지 저장
	_float		m_fShot_Time_Delay = 3.f; // 총알 쏘기용 딜레이 시간

	_bool m_bShot = false;
	_bool m_bMove_Anim = false;
	_bool	m_bFind_Path = false;

	_bool  m_bOnce = false;
	_bool  m_bDissolveStart = false;
	_float2 m_fDeadPower{};
	_float  m_fGravity = 2.3f;

	_float4 fPlayerPos{};
	_vector vUp{};
	_vector vDir{};
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	void    Dead_Motion(_float fTimeDelta);


private:
	CCollider* m_pColliderCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CCollider* m_pTargetCollider = { nullptr };
	CPlayer_Build* m_pBuild = { nullptr };
	CCamera_Free* m_pCamera = { nullptr };

public:
	static CRifleMan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END