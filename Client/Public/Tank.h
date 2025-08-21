#pragma once

#include "Client_Defines.h"
#include "Monster.h"
#include "Player_Build.h"
#include "Camera_Free.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CTank final: public CMonster
{
public:
	typedef struct: CMonster::MONSTER_DESC
	{
		CCamera_Free* pCamera = {nullptr};
		CPlayer_Build* m_pBuild = {nullptr};
	}TANK_DESC;

	enum TANK_ANIM
	{
		MONSTER_Tank_Drive,
		MONSTER_Tank_ForwardStart,
		MONSTER_Tank_RecoilFireLeft,
		MONSTER_Tank_RecoilFireRight,
		MONSTER_Tank_RecoilForwardFire,
		MONSTER_Tank_RecoilRearFire,
		MONSTER_Tank_Staged,
	};

private:
	CTank(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CTank(const CTank& Prototype);
	virtual ~CTank() = default;

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
	virtual HRESULT Render_Shadow() override;


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CCollider* m_pColliderCom = {nullptr};
	CShader* m_pShaderCom = {nullptr};
	CModel* m_pModelCom = {nullptr};
	CPlayer_Build* m_pBuild = {nullptr};
	CNavigation* m_pNavigationCom = nullptr;
	CCamera_Free* m_pCamera = {nullptr};

private:



	_bool		m_bAnimState{};
	_bool		m_bFirstShot = false; // 첫 발은 애니메이션으로 안돼서 따로 쏴줌
	_bool		m_bShotOnce = false;	// 애니메이션 끝났을 때의 조건문이 두 번 돌아서 한 번만 쏘게 만들어줌

private: // For Thread 
	future<vector<_float3>> m_vecFindingPath;
	bool m_bRequest_Path = false;

public:
	static CTank* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END