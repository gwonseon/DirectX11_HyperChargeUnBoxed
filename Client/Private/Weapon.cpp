#include "stdafx.h"
#include "..\Public\Weapon.h"

#include "GameInstance.h"
#include "Player.h"
#include <Bullet.h>

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWeapon::CWeapon(const CWeapon& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	/* 패킷, 파일입ㅇ출력을 통한 초기화. */

	return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);
	m_bShotStart = pDesc->bShotStart;
	m_pParentState = pDesc->pParentState;
	m_pSocketMatrix = pDesc->pSocketMatrix;
	m_vecCameraAt = pDesc->vCameraAt;
	m_vecCameraPos = pDesc->vCameraPos;
	m_eLevelID = pDesc->m_eLevelID;
	m_bReloading = pDesc->bReload;
	m_pReloading_Time = pDesc->fReloadingTime;
	m_iViewState = pDesc->m_iViewState;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	
	Position = {-0.86f, 0.12f, -0.39f };
	Scale = { 2.3f };
	Rotation = { 8.87969f, -115.2f, 6.4f };

	m_pTransformCom->Set_Scaling(Scale, Scale, Scale);
	m_pTransformCom->Rotation(XMConvertToRadians(Rotation.x), XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(Position.x, Position.y, Position.z, 1.f));
	
	// 총알
	m_bBulletIn = false; 
	m_iFull_Bullet = m_iRifle_Bullet; 
	m_iCurrent_Bullet = m_iFull_Bullet;
	
	return S_OK;
}

void CWeapon::Priority_Update(_float fTimeDelta)
{
	m_iFull_Bullet = m_iRifle_Bullet;
	if (m_bBulletIn == true) // 장전 완료
	{
		m_iCurrent_Bullet = m_iFull_Bullet; // 나중에 총에 따라 나누기
		m_bBulletIn = false;
	}

	if (m_iCurrent_Bullet <= 0 && *m_bReloading == false)
	{
		*m_bReloading = true;
		*m_pReloading_Time = 0.f;
	}


	if (*m_pParentState & CPlayer::MELEE || *m_pParentState & CPlayer::RELOADING)
	{
		m_pTransformCom->Rotation(XMConvertToRadians(8.87969f), XMConvertToRadians(-115.2f), XMConvertToRadians(6.4f));
	}
	else
	{
	if (m_bTPSState == false)
	{
		_long MouseMoveY = { 0 };
		if (MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			if (m_fAngle_Y <= 80.f && m_fAngle_Y >= -80.f)
				m_fAngle_Y += fTimeDelta * MouseMoveY * 4.f;
			if (m_fAngle_Y > 60.f)
				m_fAngle_Y = 60.f;
			if (m_fAngle_Y < -60.f)
				m_fAngle_Y = -60.f;
		}
		m_pTransformCom->Rotation(XMConvertToRadians(m_fAngle_Y), XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	//	m_pTransformCom->Turn(false, false, true, fTimeDelta * MouseMoveY * 0.1f);
	}
	else
	{
		_long MouseMoveY = { 0 };
		if (MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			if (m_fAngle_Y <= 35.f && m_fAngle_Y >= -50.f)
				m_fAngle_Y += fTimeDelta * MouseMoveY * 3.f;
			if (m_fAngle_Y > 35.f)
				m_fAngle_Y = 35.f;
			if (m_fAngle_Y < -50.f)
				m_fAngle_Y = -50.f;
		}	
		m_pTransformCom->Rotation(XMConvertToRadians(m_fAngle_Y), XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));

	//	m_pTransformCom->Turn(false, false, true, fTimeDelta * MouseMoveY * 0.1f);
	}
	}


}

void CWeapon::Update(_float fTimeDelta)
{
	if (*m_iViewState == PLAYER_FPS_VIEW)
	{
		m_iShaderPassNum = 1;
	}
	else
		m_iShaderPassNum = 0;


	if (m_iWeaponState == BATTERY)
		Scale = 2.3f;
	else
		Scale = 2.3f;

	m_pTransformCom->Set_Scaling(Scale, Scale, Scale);

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
	m_vecWeaponPos = XMVectorSet(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43, 1.f);
	m_vecWeaponDir = XMVectorSet(m_WorldMatrix._31, m_WorldMatrix._32, m_WorldMatrix._33, 0.f);

}

void CWeapon::Late_Update(_float fTimeDelta)
{




	// 총알 발사
	if (*m_bShotStart == true && *m_bReloading == false)
	{
		--m_iCurrent_Bullet;
		CBullet::BULLET_DESC Desc{};
		Desc.m_vecWeaponPos = m_vecWeaponPos + (m_vecWeaponDir * 0.3f); // 총구쪽에서 생성
		Desc.m_vecWeaponDir = (*m_vecCameraAt - Desc.m_vecWeaponPos);	// At까지의 방향, 총구 방향과 다름 주의
		Desc.m_vecCameraPos = *m_vecCameraPos;
		
		Desc.m_vecCameraAt = *m_vecCameraAt;							// 총알 궤적을 위함
		Desc.m_vecWeaponRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		Desc.eID = m_eLevelID;
		static_cast<CBullet*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevelID, TEXT("Bullet_Layer"), TEXT("Prototype_GameObject_Bullet"), &Desc));
	}

	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_LAST, this)))
		return;
}

HRESULT CWeapon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	Weapon_Exchange();
	

	return S_OK;
}


HRESULT CWeapon::Add_Components()
{
	/* 멤버변수로 직접 참조를 하게되면 */
	/* 1. 내가 내 컴포넌트를 이용하고자할 때 굳이 검색이 필요없이 특정 멤버변수로 바로 기능을 이용하면 된다. */
	/* 2. 다른 객체가 내 컴포넌트를 검색하고자 할때 스위치케이스가 겁나 늘어나는 상황. */

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	const _wstring Weapon_Component = TEXT("Prototype_Component_Model_Weapon");
	const _wstring WeaponComponentTag = TEXT("Com_Model");
	/* For.Com_Model */
	for(int i = 0 ; i < WEAPON_EA; i++)
	{
		const _wstring WeaponNumber = Weapon_Component + to_wstring(i);
		const _wstring WeaponComponentTag_Result = WeaponComponentTag + to_wstring(i);

		if (FAILED(__super::Add_Component(m_eLevelID, WeaponNumber,
			WeaponComponentTag_Result, reinterpret_cast<CComponent**>(&m_pModelCom[i]))))
			return E_FAIL;
	}

	return S_OK;
}
HRESULT CWeapon::Bind_ShaderResources()
{
	/*if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;*/

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}
CWeapon* CWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon* pInstance = new CWeapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CWeapon::Clone(void* pArg)
{
	CWeapon* pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CWeapon::Free()
{
	__super::Free();
	for(int i = 0 ; i < WEAPON_EA; i++)
		Safe_Release(m_pModelCom[i]);
	Safe_Release(m_pShaderCom);
}
HRESULT CWeapon::Weapon_Exchange()
{
	_uint		iNumMeshes{};
	switch (m_iWeaponState)
	{
	case Client::CWeapon::WEAPON_UNARMED:
		break;
	case Client::CWeapon::WEAPON_RIFLE:
		iNumMeshes = m_pModelCom[m_iWeaponState]->Get_NumMeshes();
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom[m_iWeaponState]->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Begin(m_iShaderPassNum)))
				return E_FAIL;

			m_pModelCom[0]->Render(i);
			m_pModelCom[1]->Render(i);
			m_pModelCom[WEAPONPARTS_RIFLE]->Render(i);
		}

		break;
	case Client::CWeapon::WEAPON_SHOTGUN:
		iNumMeshes = m_pModelCom[WEAPONPARTS_SHOTGUN]->Get_NumMeshes();
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom[WEAPONPARTS_SHOTGUN]->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Begin(m_iShaderPassNum)))
				return E_FAIL;

			m_pModelCom[0]->Render(i);
			m_pModelCom[1]->Render(i);
			m_pModelCom[WEAPONPARTS_SHOTGUN]->Render(i);
		}
		break;
	case Client::CWeapon::WEAPON_PULSECANNON:
		iNumMeshes = m_pModelCom[WEAPONPARTS_PULSE]->Get_NumMeshes();
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom[WEAPONPARTS_PULSE]->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Begin(m_iShaderPassNum)))
				return E_FAIL;

			m_pModelCom[0]->Render(i);
			m_pModelCom[1]->Render(i);
			m_pModelCom[WEAPONPARTS_PULSE]->Render(i);
		}
		break;
	case Client::CWeapon::WEAPON_TELEPORT:
		iNumMeshes = m_pModelCom[WEAPONPARTS_TELEPORT]->Get_NumMeshes();
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom[WEAPONPARTS_TELEPORT]->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Begin(m_iShaderPassNum)))
				return E_FAIL;

			m_pModelCom[0]->Render(i);
			m_pModelCom[1]->Render(i);
			m_pModelCom[WEAPONPARTS_TELEPORT]->Render(i);
		}
		break;
	case Client::CWeapon::WEAPON_LOCKETLAUNCHER:
		iNumMeshes = m_pModelCom[WEAPONPARTS_LOCKETLAUNCHER]->Get_NumMeshes();
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom[WEAPONPARTS_LOCKETLAUNCHER]->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Begin(m_iShaderPassNum)))
				return E_FAIL;

			m_pModelCom[0]->Render(i);
			m_pModelCom[1]->Render(i);
			m_pModelCom[WEAPONPARTS_LOCKETLAUNCHER]->Render(i);
		}
		break;
	case Client::CWeapon::WEAPON_RIFLE_SECOND:
		break;
	case Client::CWeapon::WEAPON_KATANA:
		break;
	case Client::CWeapon::BATTERY:
		iNumMeshes = m_pModelCom[BATTERY]->Get_NumMeshes();
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom[BATTERY]->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Begin(m_iShaderPassNum)))
				return E_FAIL;
			m_pModelCom[BATTERY]->Render(i);
		}
		break;
	default:
		break;
	}

	return S_OK;
}