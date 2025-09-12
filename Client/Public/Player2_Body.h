#pragma once
#include "Client_Defines.h"
#include "Player2_Parts.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
END

BEGIN(Client)

class CPlayer2_Body final: public CPlayer2_Parts
{
public:
	struct PLAYER2_BODY_DESC :public CPlayer2_Parts::PLAYER2_PARTS_DESC
	{
		_bool* m_bAttackState = {nullptr};
		const _uint* pParentState_Upper = {nullptr};
		const _uint* pParentState_Lower = {nullptr};

	};

	enum PLAYER_ANIM {
		PLAYER_ANIM_DualDagger_Attack_1 = 8,
		PLAYER_ANIM_FiringAnimation8_Base = 10,
		PLAYER_ANIM_Idle_Katana = 15,
		PLAYER_ANIM_Idle_Unarmed = 21,
		PLAYER_ANIM_Jump_End = 23,
		PLAYER_ANIM_Jump_Loop = 24,
		PLAYER_ANIM_Jump_Start = 25,
		PLAYER_ANIM_NinjaSweepAttack = 28,
		PLAYER_ANIM_NinjaSwiftAttack = 30,
		PLAYER_ANIM_Run_N_Katana = 34,
		PLAYER_ANIM_Run_N_Rifle = 35,
		PLAYER_ANIM_Run_NE_Katana = 38,
		PLAYER_ANIM_Run_NE_Rifle = 39,
		PLAYER_ANIM_Run_NE = 40,
		PLAYER_ANIM_Run_NW_Katana = 43,
		PLAYER_ANIM_Run_NW_Rifle = 44,
		PLAYER_ANIM_Run_NW = 45,
		PLAYER_ANIM_Run_N = 46,
		PLAYER_ANIM_Walk_E_Katana = 55,
		PLAYER_ANIM_Walk_E_Rifle = 56,
		PLAYER_ANIM_Walk_E = 58,
		PLAYER_ANIM_Walk_N_Katana = 60,
		PLAYER_ANIM_Walk_N_Rifle = 61,
		PLAYER_ANIM_Walk_N_Shotgun = 62,
		PLAYER_ANIM_Walk_NE_Katana = 64,
		PLAYER_ANIM_Walk_NE_Rifle = 65,
		PLAYER_ANIM_Walk_NE = 67,
		PLAYER_ANIM_Walk_NW_Katana = 69,
		PLAYER_ANIM_Walk_NW_Rifle = 70,
		PLAYER_ANIM_Walk_NW = 72,
		PLAYER_ANIM_Walk_N = 73,
		PLAYER_ANIM_Walk_S_Katana = 75,
		PLAYER_ANIM_Walk_S_Rifle = 76,
		PLAYER_ANIM_Walk_SE_Katana = 79,
		PLAYER_ANIM_Walk_SE_Rifle = 80,
		PLAYER_ANIM_Walk_SE = 82,
		PLAYER_ANIM_Walk_SW_Katana = 84,
		PLAYER_ANIM_Walk_SW_Rifle = 85,
		PLAYER_ANIM_Walk_SW = 87,
		PLAYER_ANIM_Walk_S =88,
		PLAYER_ANIM_Walk_W_Katana = 90,
		PLAYER_ANIM_Walk_W_Rifle = 91,
		PLAYER_ANIM_Walk_W = 92,
		PLAYER_ANIM_WeaponReload = 94,
		PLAYER_ANIM_END
	};
	enum UPPERBODY_STATE
	{
		UPPER_STATE_IDLE		= 0x00000001,
		UPPER_STATE_ATTACK1		= 0x00000002,
		UPPER_STATE_ATTACK2		= 0x00000004,
		UPPER_STATE_HOLD_ITEM	= 0x00000008,
		UPPER_STATE_THROW_ITEM	= 0x00000010,
		UPPER_STATE_FIRE		= 0x00000020,
		UPPER_STATE_CLOSE_ATTACK= 0x00000040,
		UPPER_STATE_END
	};

private:
	CPlayer2_Body(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CPlayer2_Body(const CPlayer2_Body& Prototype);
	virtual ~CPlayer2_Body() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	//virtual HRESULT Render_Shadow() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	
	const _float4x4* Get_SocketMatrix(const _char* pBoneName);



private:
	CShader* m_pShaderCom = {nullptr};
	CModel* m_pModelCom = {nullptr};
	CCollider* m_pColliderCom = {nullptr};


private:
	_uint	m_iShaderPassNum = 0;
	LEVELID m_eLevelID{};



public:
	static CPlayer2_Body* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END