#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"
#include "GameInstance.h"
#include "Player2_Parts.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)
class CPlayer_State;
class CPlayer2 final: public CContainerObject
{
public:
	typedef struct: public CGameObject::GAMEOBJ_DESC
	{
		LEVELID m_eLevelID{};
		_vector* vCameraAt = {};
		_vector* vCameraPos = {};
		_uint* iRound = {};
		_uint iCellIdx{};
	}PLAYER_DESC;

public:
	enum PARTOBJID {
		PART_PLAYER_HEAD,PART_PLAYER_BODY,
		PART_WEAPON,PART_TPS_PIVOT,PART_FPS_PIVOT,
		PART_END
	};
	enum PLAYER_STATE {
		STATE_IDLE			= 0x00000001,
		WALKSTATE_NORTH		= 0x00000002,
		WALKSTATE_SOUTH		= 0x00000004,
		WALKSTATE_EAST		= 0x00000008,
		WALKSTATE_WEST		= 0x00000010,
		WALKSTATE_NORTHEAST = 0x00000020,
		WALKSTATE_NORTHWEST = 0x00000040,
		WALKSTATE_SOUTHEAST = 0x00000080,
		WALKSTATE_SOUTHWEST = 0x00000100,
		JUMP_START			= 0x00000200,
		JUMP_LOOP			= 0x00000400,
		JUMP_END			= 0x00000800,
		RUNSTATE_NORTH		= 0x00001000,
		RUNSTATE_NORTHWEST	= 0x00002000,
		RUNSTATE_NORTHEAST	= 0x00004000,
		RELOADING			= 0x00008000,
		FIRE				= 0x00010000,
		FIRE_RB				= 0x00020000,
		MELEE				= 0x00040000,
		PLAYER_STATE_END
	};
	enum WEAPON_STATE {
		WEAPON_UNARMED		= 0x00000001,
		WEAPON_RIFLE		= 0x00000002,
		WEAPON_KATANA		= 0x00000004,
		BATTERY				= 0x00000008,
		TRACKER				= 0x00000010,
		WEAPON_STATE_END
	};

private:
	CPlayer2(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CPlayer2(const CPlayer2& Prototype);
	virtual ~CPlayer2() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();

private:
	void Auto_Heal(_float fTimeDelta);
	void Build_Update(_float fTimeDelta);
	
	void ChangeState(unique_ptr<CPlayer_State> nextState);



private:
	CNavigation* m_pNavigationCom = nullptr;
	unique_ptr<CPlayer_State> m_pState; // 상태 제어
private:
	
	_uint* m_iRound = {nullptr};

	_vector* m_vecCameraAt{};
	_vector* m_vecCameraPos{};
	
	_bool* m_bRoundStart = {nullptr}; // 빌드 모드가 끝나고 라운드가 시작했음을 알리는  포인터

private:
	LEVELID		m_eLevelID{};

	_uint		m_iCellidx{};

	_float		m_fHpTimer{};
	
	_bool					m_bBuildMode = false;		// 빌드 모드
	_bool					m_bBuild_Gauging = false;  // E 눌러서 빌드 중임을 알려주는 변수
	_bool					m_bBuild_Able = false;


public:
	static CPlayer2* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
