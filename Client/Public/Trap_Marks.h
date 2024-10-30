#pragma once

#include "Client_Defines.h"
#include "Player_Build.h"
#include "Trap_Bricks.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CTrap_Marks final : public CPlayer_Build
{
public:	
	enum Trap_Type { BRICKS_TRAP, TANK_TRAP, TRAP_TYPE_END };
	typedef struct : public CPlayer_Build::PLAYER_BUILD_DESC
	{
		Trap_Type eType{};
		 
	}TRAP_MARKS_DESC;
private:
	CTrap_Marks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrap_Marks(const CTrap_Marks& Prototype);
	virtual ~CTrap_Marks() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool	Get_BuildAble() { return m_bBuild_PreView; } // 트랩을 만들 수 있다
	_bool	Get_Build_Done() { return m_bBuild; }  //  트랩이 완성 되었다
	_bool	Get_CanBuy() { return m_pBricks->Get_CanBuy(); }
	_vector Get_TrapPos() { return m_vecPos; }
	_uint	Get_Privce() { return m_pBricks->Get_Coin(); }
	void	Set_ReBuild()  // 건물이 부숴졌을 때 다시 만들 수 있도록 값 초기화해줌
	{ 
		m_bDraw = true;
		m_bBuild = false;
		m_pBricks->Set_ReBuild();
	}
	_bool	Get_Bricks_KnockDown() { return m_pBricks->Get_knockdown(); } // 블럭이 넉다운 되었는지 
	void	Set_Build_Done(_bool bDone) { m_bBuild = bDone; } // 트랩 완성 되었는가를 받아옴
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CPlayer* m_pPlayer = { nullptr };
	CTrap_Bricks* m_pBricks = { nullptr };

private:
	_uint	m_iModel_Idx{}; 
	LEVELID m_eLevel{};
	float	m_fDistance = 0.f;
	_vector m_vecPos{};

	Trap_Type	m_eType{};		// 트랩 종류
	_bool	m_bBuild = false;	// 트랩 건설 여부
	_bool	m_bBuild_PreView = false; // 트랩 건설 가능 여부
public:
	static CTrap_Marks* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END