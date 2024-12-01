#include "stdafx.h"
#include "..\Public\Monster_Bullet.h"

#include "GameInstance.h"
#include <Explosion.h>
#include <Effect_Explosion_Tank.h>
CMonster_Bullet::CMonster_Bullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMonster_Bullet::CMonster_Bullet(const CMonster_Bullet& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CMonster_Bullet::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster_Bullet::Initialize(void* pArg)
{
    MONSTER_BULLET_DESC* pDesc = static_cast<MONSTER_BULLET_DESC*>(pArg);
    m_eLevel = pDesc->eID;
    m_iModelNumber = pDesc->m_iModelNumber;
    m_eType = pDesc->eType;
    m_vecDir = pDesc->vDir;
    m_vecTargetPos = pDesc->vTargetPos;
    m_pBuild = pDesc->m_pBuild;
    m_pPlayer = pDesc->pPlayer;
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Add_Components()))
        return E_FAIL;
    _vector vPos{};
    switch (m_eType)
    {
    case Client::CMonster_Bullet::TANK_BULLET:
    {
        m_pTransformCom->Set_Scaling(5.f, 5.f, 5.f);
        m_pTransformCom->LookAt(m_vecTargetPos);
        vPos = XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f);
        vPos = vPos + XMVector3Normalize(m_vecTargetPos - vPos) * 6.8f;
        vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 4.f);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
        m_pCamera = pDesc->pCamera;
        m_fAttack = 10.f;
        break;
    }
    case Client::CMonster_Bullet::HELICOPTER_BULLET:
        m_pTransformCom->Set_Scaling(0.05f, 0.05f, 0.05f);
        m_pTransformCom->LookAt(m_vecTargetPos);
        vPos = XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
        m_fAttack = 3.f; 
        break;
    case Client::CMonster_Bullet::RIFLEMAN_BULLET:
        m_pTransformCom->Set_Scaling(0.05f, 0.05f, 0.05f);
        vPos = XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y + 1.f, pDesc->fPosition.z, 1.f);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
        m_fAttack = 3.f;
        break;

        

    case Client::CMonster_Bullet::MONSTERBULLET_END:
        break;
    default:
        break;
    }
    m_fHp = 10.f;

    m_bCanAttacked = true;
    m_bIsBullet = true;
   
    m_pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(m_eLevel, TEXT("Layer_PlayerBuild"), TEXT("Com_Collider_AABB")));
    m_pPlayerCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(m_eLevel, TEXT("Layer_Player"), TEXT("Com_Collider_AABB")));
    return S_OK;
}

void CMonster_Bullet::Priority_Update(_float fTimeDelta)
{
    _vector vPos{};
    switch (m_eType)
    {
    case Client::CMonster_Bullet::TANK_BULLET:
    {
        m_pTransformCom->LookAt(m_vecTargetPos); // 목표를 항상 바라보고 있게
        m_pTransformCom->Go_Straight(fTimeDelta * 20.f); // 날아간당
        break;
    }
    case Client::CMonster_Bullet::HELICOPTER_BULLET:
        m_pTransformCom->LookAt(m_vecTargetPos); // 목표를 항상 바라보고 있게
        m_pTransformCom->Go_Straight(fTimeDelta * 60.f); // 날아간당
        break;
    case Client::CMonster_Bullet::RIFLEMAN_BULLET:
        vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        vPos = vPos + m_vecDir * fTimeDelta * 50.f;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos); // 날아간당
        break;

    default:
        break;
    }
}
void CMonster_Bullet::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

}

void CMonster_Bullet::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;

    if (m_fHp <= 0.f)
    {
        m_vecPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        CExplosion::EXPLOSION_DESC pExplosion{};
        pExplosion.eType = CExplosion::EXPLOSION_TANK;
        pExplosion.eID = m_eLevel;
        pExplosion.fPosition = _float3{ XMVectorGetX(m_vecPosition), XMVectorGetY(m_vecPosition) ,XMVectorGetZ(m_vecPosition) };
        pExplosion.fScale = _float3{ 3.f, 3.f, 3.f };
        m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel, TEXT("Layer_Explosion"), TEXT("Prototype_GameObject_Explosion"), &pExplosion);

        CEffect_Explosion_Tank::EFFECT_Tank_Explosion_DESC Effect{};
        Effect.eType = CEffect_Explosion_Tank::EXPLOSION_TANK;
        Effect.eLevel = m_eLevel;
        Effect.fScale = _float3{ 15.f, 15.f, 15.f };
        Effect.fPosition = _float3{ XMVectorGetX(m_vecPosition), XMVectorGetY(m_vecPosition) ,XMVectorGetZ(m_vecPosition) };
        //Effect.pCamera = m_pCamera;
        m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Tank_Explosion"), &Effect);

        m_bDead = true;
    }

    // 건물에 데미지 주기
    if (m_pTargetCollider != nullptr)
    {
        
        bCollision = m_pColliderCom->Intersect(m_pTargetCollider); // 브레인 코어와 충돌체크 
        if (bCollision == true && m_bDead == false)
        {
            if (m_eType == TANK_BULLET)
            {
                m_vecPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
                CExplosion::EXPLOSION_DESC pExplosion{};
                pExplosion.eType = CExplosion::EXPLOSION_TANK;
                pExplosion.eID = m_eLevel;
                pExplosion.fPosition = _float3{ XMVectorGetX(m_vecPosition), XMVectorGetY(m_vecPosition) ,XMVectorGetZ(m_vecPosition) };
                pExplosion.fScale = _float3{ 3.f, 3.f, 3.f };
                m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel, TEXT("Layer_Explosion"), TEXT("Prototype_GameObject_Explosion"), &pExplosion);
              
                CEffect_Explosion_Tank::EFFECT_Tank_Explosion_DESC Effect{};
                Effect.eType = CEffect_Explosion_Tank::EXPLOSION_TANK;
                Effect.eLevel = m_eLevel;
                Effect.fScale = _float3{ 15.f, 15.f, 15.f };
                Effect.fPosition = _float3{ XMVectorGetX(m_vecPosition), XMVectorGetY(m_vecPosition) ,XMVectorGetZ(m_vecPosition) };
                //Effect.pCamera = m_pCamera;
                m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Tank_Explosion"), &Effect);

                m_bDead = true;
            }
            else
            {
                m_pBuild->Set_Damaged(m_fAttack);
                m_bDead = true;
            }
        }
    }
 

    // 플레이어에게 데미지 주기
    switch (m_eType)
    {
    case Client::CMonster_Bullet::TANK_BULLET:
    {
        bCollision = m_pColliderCom->Intersect(m_pPlayerCollider); // 플레이어와 충돌체크 
        if (bCollision == true && m_bDead == false)
        {
            CLayer* pPlayerLayer = (m_pGameInstance->Find_Layer(m_eLevel, TEXT("Layer_Player")));
            CPlayer* pPlayer = static_cast<CPlayer*>(pPlayerLayer->Get_GameObject_List().front());
            pPlayer->Set_Damaged(m_fAttack);

            m_vecPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
            CExplosion::EXPLOSION_DESC pExplosion{};
            pExplosion.eID = m_eLevel;
            pExplosion.eType = CExplosion::EXPLOSION_TANK;
            pExplosion.fPosition = _float3{ XMVectorGetX(m_vecPosition), XMVectorGetY(m_vecPosition) ,XMVectorGetZ(m_vecPosition) };
            pExplosion.fScale = _float3{ 3.f, 3.f, 3.f };
            m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel, TEXT("Layer_Explosion"), TEXT("Prototype_GameObject_Explosion"), &pExplosion);
            
            CEffect_Explosion_Tank::EFFECT_Tank_Explosion_DESC Effect{};
            Effect.eLevel = m_eLevel;
            Effect.eType = CEffect_Explosion_Tank::EXPLOSION_TANK;
            Effect.fScale = _float3{ 15.f, 15.f, 15.f };
            Effect.fPosition = _float3{ XMVectorGetX(m_vecPosition), XMVectorGetY(m_vecPosition) ,XMVectorGetZ(m_vecPosition) };
            m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(m_eLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Tank_Explosion"), &Effect);
            m_bDead = true;
            break;
        }
    }
    case Client::CMonster_Bullet::HELICOPTER_BULLET:
        break;
    case Client::CMonster_Bullet::RIFLEMAN_BULLET:
        bCollision = m_pColliderCom->Intersect(m_pPlayerCollider); // 플레이어와 충돌체크 
        if (bCollision == true && m_bDead == false)
        {
            m_pPlayer->Set_Damaged(m_fAttack);
            m_bDead = true;
        }
        break;
    default:
        break;
    }

}

HRESULT CMonster_Bullet::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }


#ifdef _DEBUG
    m_pColliderCom->Render();
#endif
    return S_OK;
}

HRESULT CMonster_Bullet::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    const _wstring Model_Component = TEXT("Prototype_Component_Model_Bullet");
    const _wstring Model_Component_Result = Model_Component + to_wstring(m_iModelNumber);
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    // Sphere
    CBounding_Sphere::BOUND_SPHERE_DESC			SphereDesc{};
    SphereDesc.fRadius = 0.5f;
    SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);

    if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Bullet::Bind_ShaderResources()
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
  
    return S_OK;
}

CMonster_Bullet* CMonster_Bullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonster_Bullet* pInstance = new CMonster_Bullet(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMonster_Bullet");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster_Bullet::Clone(void* pArg)
{
    CMonster_Bullet* pInstance = new CMonster_Bullet(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CMonster_Bullet");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMonster_Bullet::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);

}
