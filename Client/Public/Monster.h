#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Player.h"
BEGIN(Client)

class CMonster : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		CPlayer* pPlayer = { nullptr };
		CLayer* pTrapLayer = { nullptr };

		const _float4x4* matPlayerWorld = { nullptr };
		const _float4x4* matBrainCoreWorld = { nullptr };
		_vector* vecTargetPos = {nullptr};

		LEVELID		eID = {};
		_uint		iBraincore_CellNumber{};
		_int		iModelComponentIndex{};
		_uint		iCell_Idx{};
	}MONSTER_DESC;

	
	enum EVILDAMAGE_ANIM
	{

	};
	enum BLIMP_ANIM
	{
		BlimpDeflate_Anim
	};
	enum MEATBAG_ANIM
	{
		Meatbag_FallingHigh,
		Meatbag_Falling,
		Meatbag_HeadSpin,
		Meatbag_Idle01,
		Meatbag_Idle02,
		Meatbag_Kick,
		Meatbag_Pose,
		Meatbag_PunchL,
		Meatbag_PunchR,
		Meatbag_Run,
		Meatbag_StumbleBack_L_newRoot,
		Meatbag_StumbleBack_R_newRoot,
		Meatbag_StumbleBackSpin_L,
		Meatbag_StumbleBackSpin_R,
		Meatbag_WalkEnd,
		Meatbag_WalkStart,
		Meatbag_Walk,
		MeatbagPilot,
		Meatgbag_Landing,
	};

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() ;
	virtual HRESULT Initialize(void* pArg) ;
	virtual void Priority_Update(_float fTimeDelta) ;
	virtual void Update(_float fTimeDelta) ;
	virtual void Late_Update(_float fTimeDelta) ;
	virtual HRESULT Render();

	void Set_TargetPos(_vector* pPos) { m_vecTargetPos = pPos; }

protected:
	LEVELID	m_eLevel = {};
	_int	m_iModelIndex = {};
	_uint	m_iBraincore_CellNumber = {};
	const _float4x4* m_matPlayerWorld = { nullptr };
	const _float4x4* m_matBrainCoreWorld = { nullptr };

	_float		m_fTime_For_Target{};  // 트랩 찾는 경로 탐색 지연 시간
	_float		m_fCurrentTime = 0.f;  
	_float		m_fDamaged_DelayTime = 1.f;
	// 넉백용
	_bool		m_bKnockBacking = false;
	_float		m_fKnockBack_Power = 0.f;
	_float		m_fKnockBack_Height = 0.f;

	//  길 찾기
	vector<_float3> Path{};
	_uint		m_iCell_Idx{};
	_uint		m_iPrevPlayer_Cell_Idx{};


protected:

protected:
	_vector*	m_vecTargetPos;
	_vector		m_vecNewTargetPos{};
	_vector		m_vecStoreTargetPos{};

	CPlayer*	m_pPlayer		= { nullptr };
	CLayer*		m_pTrapLayer	= { nullptr };
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;



};

END