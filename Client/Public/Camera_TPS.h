#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)
class Camera_TPS final : public CCamera
{
public:
	typedef struct : public CCamera::CAMERA_DESC
	{
		LEVELID eLevel{};
		_float	fMouseSensor{};

	}CAMERA_FREE_DESC;

private:
	Camera_TPS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	Camera_TPS(const Camera_TPS& Prototype);
	virtual ~Camera_TPS() = default;

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

	void	Set_Position(_vector vPos);
	void Set_ViewMatrix(_matrix	ViewMatrix) { m_ViewMatrix = ViewMatrix; }
	void Set_CamDir(_vector vDir) { m_vecDir = vDir; }
private:
	_float					m_fMouseSensor = { 0.f };
	_bool					m_bMouseLock = true;
	_matrix					m_ViewMatrix{};
	_vector					m_vecDir{};
	LEVELID					m_eLevelID = LEVEL_END;
private:



public:
	static Camera_TPS* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END