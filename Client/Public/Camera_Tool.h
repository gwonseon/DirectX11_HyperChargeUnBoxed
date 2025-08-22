#pragma once
#include "Client_Defines.h"
#include "Camera.h"

class CCamera_Tool final: public CCamera
{
public:
	typedef struct: public CCamera::CAMERA_DESC
	{
		LEVELID eLevel{};
		_float	fMouseSensor{};
	}CAMERA_TOOL_DESC;
	

private:
	CCamera_Tool(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CCamera_Tool(const CCamera_Tool& Prototype);
	virtual ~CCamera_Tool() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float					m_fMouseSensor = {0.f};
	_bool					m_bMouseLock = true;
	LEVELID					m_eLevelID = LEVEL_END;

public:
	static CCamera_Tool* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

