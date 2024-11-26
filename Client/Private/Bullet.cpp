#include "stdafx.h"
#include "..\Public\Bullet.h"

#include "GameInstance.h"
#include <Explosion.h>
CBullet::CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CBullet::CBullet(const CBullet& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CBullet::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
    BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);
    m_eLevel = pDesc->eID;
    m_vecWeaponPos = pDesc->m_vecWeaponPos;
    m_vecWeaponDir = pDesc->m_vecWeaponDir;
    m_vecCameraAt = pDesc->m_vecCameraAt;
    m_vecCameraPos = pDesc->m_vecCameraPos;
    m_eType = pDesc->eType;
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;
    switch (m_eType)
    {
    case Client::CBullet::BULLET_RIFLE:
        iRand = 2;
        m_fSize = 0.1f;
        m_pTransformCom->Set_Scaling(m_fSize, m_fSize, m_fSize);
        break;
    case Client::CBullet::BULLET_LOCKET:
        iRand = 3; 
        m_pTransformCom->Set_Scaling(0.3f, 0.3f, 0.3f);
        break;
    case Client::CBullet::BULLET_END:
        break;
    default:
        break;
    }
    m_bCanAttacked = true;
    m_bIsBullet = true;
    m_fBullet_Move = 0.f;
    m_bChange_Root = false; // 타겟 위치 도달 후 총알 궤적 변경
    return S_OK;
}

void CBullet::Priority_Update(_float fTimeDelta)
{
   
}

void CBullet::Update(_float fTimeDelta)
{
    if (m_eType == BULLET_RIFLE)
    {
        m_pTransformCom->Set_Scaling(m_fSize, m_fSize, m_fSize);
        if (m_fSpeed >= 150.5f)
            m_fSize += fTimeDelta ;
        
    }
}

void CBullet::Late_Update(_float fTimeDelta)
{
    m_fSpeed = 150.f;
    m_fBullet_Move += (fTimeDelta * m_fSpeed); // 총알 날아가기
    if (m_bChange_Root == false)
    {
        // At 위치와 총알의 위치가 비슷해 졌을 때 날아가는 방향 변경
        vTargetPos = m_vecWeaponPos + XMVector3Normalize(m_vecWeaponDir) * m_fBullet_Move;
        distance = XMVectorGetX(XMVector3Length(vTargetPos - m_vecCameraAt));
        if (distance <= 1.5f)
        {
            m_fBullet_Move = 0.f;
            m_bChange_Root = true;
        }
    }
    else
    {
        m_bDead = true;
        // At위치 도달 전까지는 그냥 날아가라
        vTargetPos = vTargetPos + XMVector3Normalize(m_vecCameraAt - m_vecCameraPos) * m_fBullet_Move;
    }
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTargetPos);

    if (m_eType == BULLET_LOCKET && m_bDead == true)
    {
        CExplosion::EXPLOSION_DESC pExplosion{};
        pExplosion.eID = m_eLevel;
        pExplosion.eType = CExplosion::EXPLOSION_LOCKET;
        pExplosion.fPosition = _float3{ XMVectorGetX(m_vecPosition), XMVectorGetY(m_vecPosition) ,XMVectorGetZ(m_vecPosition) };
        pExplosion.fScale = _float3{ 8.f, 8.f, 8.f };
        m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel, TEXT("Layer_Explosion_Player"), TEXT("Prototype_GameObject_Explosion"), &pExplosion);

    }
    if(m_eType == BULLET_RIFLE)
    {
       
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONLIGHT, this)))
            return;
    }
    if (m_eType == BULLET_LOCKET)
    {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
            return;
    }

    m_fSpeed += fTimeDelta;
}

HRESULT CBullet::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
            return E_FAIL;

        if (m_eType == BULLET_RIFLE)
        {
            if (FAILED(m_pShaderCom->Begin(8)))
                return E_FAIL;
        }
        else if (m_eType == BULLET_LOCKET)
        {
            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;
        }

        m_pModelCom->Render(i);
    }
    return S_OK;
}

HRESULT CBullet::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    
    const _wstring Model_Component = TEXT("Prototype_Component_Model_Bullet");
    const _wstring Model_Component_Result = Model_Component + to_wstring(iRand);
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBullet::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _float fFar = m_pGameInstance->Get_CameraFar();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;


    return S_OK;
}

CBullet* CBullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBullet* pInstance = new CBullet(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBullet");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBullet::Clone(void* pArg)
{
    CBullet* pInstance = new CBullet(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBullet");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBullet::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
