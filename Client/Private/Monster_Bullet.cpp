#include "stdafx.h"
#include "..\Public\Monster_Bullet.h"

#include "GameInstance.h"
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
        m_fAttack = 20.f;
        break;
    }
    case Client::CMonster_Bullet::HELICOPTER_BULLET:
        m_pTransformCom->Set_Scaling(0.05f, 0.05f, 0.05f);
        m_pTransformCom->LookAt(m_vecTargetPos);
        vPos = XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
        m_fAttack = 5.f; 
        break;
    case Client::CMonster_Bullet::RIFLEMAN_BULLET:
        m_pTransformCom->Set_Scaling(0.05f, 0.05f, 0.05f);
        vPos = XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
        m_fAttack = 5.f;
        break;

        

    case Client::CMonster_Bullet::MONSTERBULLET_END:
        break;
    default:
        break;
    }


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

    _bool bCollision = m_pColliderCom->Intersect(m_pTargetCollider); // 브레인 코어와 충돌체크 
    if (bCollision == true && m_bDead == false)
    {
        m_pBuild->Set_Damaged(m_fAttack);
        m_bDead = true;
    }



    switch (m_eType)
    {
    case Client::CMonster_Bullet::TANK_BULLET:
        break;
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
    

    
    
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
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
    SphereDesc.fRadius = 0.2f;
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
