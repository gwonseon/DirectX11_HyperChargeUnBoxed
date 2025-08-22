#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)



class CCamera_Free final: public CCamera
{
public:
	typedef struct: public CCamera::CAMERA_DESC
	{
		_vector* m_vecTPS_CamPos{};
		_vector* m_vecFPS_CamPos{};
		_vector* m_vecWeaponPos{};
		_vector* m_vecWeaponDir{};

		_bool* bShotStart{}; // 총 쏘는 시작 타이밍
		_uint* iViewState{};
		const _float4x4* matPlayerWorld = {nullptr};
	}CAMERA_FREE_DESC;
private:
	CCamera_Free(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& Prototype);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void InGame_Camera(_float fTimeDelta);
public:
	void	Set_PlayerPos(_vector vPos) {
		m_vecPos = vPos;
	}
	void	Set_Rotation(_float fX,_float fY,_float fZ) {
		m_pTransformCom->Rotation(fX,fY,fZ);
	}



	_vector Get_Dir() {
		return m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	}
	_vector* Get_Camera_At() {
		return &m_vecAt;
	}
	_vector* Get_Camera_Pos() {
		return &m_vecPos;
	}


private:
	_vector					m_vecDir{};
	_vector					m_vecStore_Dir{};

	_vector					vecEye{};
	_uint*					m_iViewState{};
	_vector					m_vecAt{};
	_vector					m_vecPos{};
private:
	const _float4x4* m_matPlayerWorld = {nullptr};

private:
	_float					m_fRotationPerSec{};
	_float					m_fAngle_Y{};
	_float					m_fMotion_Delay{};

private:
	_vector* m_vecTPSPos = {nullptr};
	_vector* m_vecFPSPos = {nullptr};
	_vector* m_vecWeaponPos = {nullptr};
	_vector* m_vecWeaponDir = {nullptr};

	_bool* m_pShotStart = {nullptr};

	XMMATRIX RotationMatrix{};
	XMMATRIX matWorld{};

	_float m_fStore_RandomValue{};
public:
	static CCamera_Free* Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};

END