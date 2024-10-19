#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Free final : public CCamera
{
public:
	typedef struct : public CCamera::CAMERA_DESC
	{
		LEVELID eLevel{};
		_float	fMouseSensor{};

		_vector* m_vecTPS_CamPos{};
		_vector* m_vecFPS_CamPos{};
		_vector* m_vecWeaponPos{};
		_vector* m_vecWeaponDir{};

		_bool* bShotNow{}; // 총 쏘는 타이밍
		_uint* iViewState{};
		_uint* iWeaponState{};
		const _float4x4* matPlayerWorld = { nullptr };
	}CAMERA_FREE_DESC;
private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& Prototype);
	virtual ~CCamera_Free() = default;

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

public:
	void	Set_PlayerPos(_vector vPos) { m_vecPos = vPos; }
	void	Set_Rotation(_float fX, _float fY, _float fZ) {
		m_pTransformCom->Rotation(fX, fY, fZ);
	}
	void	Set_Direction(_vector vDirect) { m_vecDir = vDirect; }


	_vector Get_Dir() { return m_pTransformCom->Get_State(CTransform::STATE_LOOK); }
	_vector* Get_Camera_At() { return &vAt; }


private:
	_float					m_fMouseSensor = { 0.f };
	_bool					m_bMouseLock = true;
	_vector					m_vecDir{};
	_vector					m_vecPos{};
	LEVELID					m_eLevelID = LEVEL_END;
	_vector					vecEye{};
	_uint*					m_iViewState{};
	_vector					vAt{};
private:
	const _float4x4* m_matPlayerWorld = { nullptr };

private:
	_float					m_fRotationPerSec{};
	_float					m_fAngle_Y{};

	_vector* m_vecTPSPos = {nullptr};
	_vector* m_vecFPSPos = { nullptr };
	_vector* m_vecWeaponPos = { nullptr };
	_vector* m_vecWeaponDir = { nullptr };

	_bool* m_pShotNow = { nullptr };
	_uint* m_pWeaponState = { nullptr };


	XMMATRIX RotationMatrix{};
	XMMATRIX matWorld{};



	_bool m_bOnce = false;
	_bool m_bOnce2 = false;
	_float m_fStore_RandomValue{};
public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};

END