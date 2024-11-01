#pragma once

#include "Client_Defines.h"
#include "UIObject.h"
#include "Player.h"
#include "Trap_Marks.h"
#include "Energy_Machine.h"
#include "Energy_Cap.h"
BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CUI_CircleGuage final : public CUIObject
{

public:
	typedef struct : public CUIObject::UIOBJECT_DESC
	{
		_uint	iIndex{};
		CEnergy_Machine* pEnergy_Machine	= { nullptr };
		CEnergy_Cap* pEnergyMachine_Cap		= { nullptr };
		CPlayer* pPlayer					= { nullptr };
		vector<CTrap_Marks*>* vecMarks		= { nullptr };
	}CIRCLEGAUGE_DESC;
private:
	CUI_CircleGuage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CircleGuage(const CUI_CircleGuage& Prototype);
	virtual ~CUI_CircleGuage() = default;

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
	HRESULT Add_Components(_int iNum);
	HRESULT Bind_ShaderResources();


private:
	CShader*				m_pShaderCom			= { nullptr };
	CTexture*				m_pTextureCom			= { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom			= { nullptr };
	CEnergy_Machine*		m_pEnergy_Machine		= { nullptr };
	CEnergy_Cap*			m_pEnergyMachine_Cap = { nullptr };
	CPlayer*				m_pPlayer				= { nullptr };

	vector<CTrap_Marks*>*	m_pvecTrap_Marks		= { nullptr };	// 트랩 마크에 트랩 설치
	_bool*					m_bBuild_Gauging		= { nullptr };	// 빌드모드
	_bool*					m_bBuildMode			= { nullptr };  // 빌드모드

public:
	void	Set_Charging(_bool bCharging) { m_bCharging = bCharging; }
	void	Set_Item_Interaction(_bool bItemInteraction) { 
		m_bItem_Interaction = bItemInteraction;
		if(m_bItem_Interaction == true)
			m_bItemCharging = true;
		else
			m_bItemCharging = false;
	}
	void	Set_Item_InteractionEnd(_bool bEnd) { m_bItem_Interaction_End = bEnd; }

	_bool	Get_Charging() { return m_bCharging; }
	_bool   Get_ItemInteraction_End() { return m_bItem_Interaction_End; }
	
private:
	float	m_fGuaging_Time{};
	_float	m_fReal_Gauging_Time{};
	_bool	m_bCharging{}, m_bItemCharging{};
	
	_bool	m_bBuild_Draw = false;
	
	_bool   m_bItem_Interaction = false;
	_bool   m_bItem_Interaction_End = false;

	_bool	m_bBattery_Insert = false;
	_bool	m_bBattery_Insert_End = false;
	_bool	m_bBattery_Insert_First = false;
public:
	static CUI_CircleGuage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END