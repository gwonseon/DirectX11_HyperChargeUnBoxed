#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CMonster final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJ_DESC
	{
		LEVELID eID = {};
		_int	iModelComponentIndex{};
	}MONSTER_DESC;

	enum HELICOPTER_ANIM {CENTER, EAST, NORTH_EAST, NORTH_WEST, NOTRH, SOUTH, WEST, DIORAMA};
	
	enum EVILDAMAGE_ANIM
	{

	};
	enum TANK_ANIM
	{
		Stage,
		ForwardStart,
		Drive
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

private:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	/* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
	virtual HRESULT Initialize_Prototype() override;

	/* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
	/* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한ㄴ다. */
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	LEVELID	m_eLevel = {};
	_int	m_iModelIndex = {};
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;



};

END