#include "stdafx.h"
#include "..\Public\Missile_Truck.h"
#include "GameInstance.h"


CMissile_Truck::CMissile_Truck(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject{ pDevice, pContext }
{
}

CMissile_Truck::CMissile_Truck(const CMissile_Truck& Prototype)
    : CContainerObject{ Prototype }
{
}

HRESULT CMissile_Truck::Initialize_Prototype()
{
    
    return S_OK;
}

HRESULT CMissile_Truck::Initialize(void* pArg)
{
    CContainerObject::CONTAINEROBJECT_DESC		Desc{};
    Desc.iNumPartObjects = MISSILETRUCK_END;
    Desc.fSpeedPerSec = 25.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.f);
    
    MISSILETRUCK_DESC* pTruck = static_cast<MISSILETRUCK_DESC*>(pArg);
    m_eLevelID = pTruck->m_eLevelID;
    m_iRound = pTruck->iRound;
    m_pPlayer = pTruck->pPlayer;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;
    if (FAILED(Add_Components()))
        return E_FAIL;


    m_pTransformCom->Set_Scaling(pTruck->fScale.x, pTruck->fScale.y, pTruck->fScale.z);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pTruck->fPosition.x, pTruck->fPosition.y, pTruck->fPosition.z, 1.f));
    m_pTransformCom->Rotation(0.f, XMConvertToRadians(-90.f), 0.f);
    m_fPos = pTruck->fPosition;
    m_fScale = pTruck->fScale;

    if (FAILED(Add_PartObjects()))
        return E_FAIL;

    m_bAffected = true;
    m_bDontDestroy = true;
    m_fHp = 200;
    return S_OK;
}

void CMissile_Truck::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
    if (m_bKnockdown == true)
    {
        m_pShooter->Set_knockdown(true);
        m_pTruckBody->Set_knockdown(true);
        m_pMissile->Set_knockdown(true);
    }

    if (m_fHp <= 0.f)
    {
        m_bKnockdown = true;
    }

    // 미사일 맞았을 떄
    if (m_bKnockdown == true)
        return;

    CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_YARD, TEXT("Layer_Explosion"));
    if (pLayer == nullptr)
        return;
    _uint i = 0;
    list<class CGameObject*> pList =  pLayer->Get_GameObject_List();
    for (auto& pExplosion : pList)
    {
        if (pExplosion == nullptr)
            return;
        CCollider* pCol = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(m_eLevelID, TEXT("Layer_Explosion"), TEXT("Com_Collider_Sphere"), i));
        if (pCol != nullptr)
        {
            if (m_pColliderCom->Intersect(pCol))
            {
                m_fHp -= 100.f;
            }
            pExplosion->Set_Count();
            if (pExplosion->Get_Count() >= 4)
                pExplosion->Set_Dead();
        }
        i++;
    }
}

void CMissile_Truck::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CMissile_Truck::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
    if (m_bDead == false)
    {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
            return;
    }

}

HRESULT CMissile_Truck::Render()
{
  
#ifdef _DEBUG
    if (m_bDead)
        return S_OK;
    m_pColliderCom->Render();
#endif
    return S_OK;
}

HRESULT CMissile_Truck::Add_Components()
{
    CBounding_AABB::BOUND_AABB_DESC		AABBDesc{};
    AABBDesc.vExtents = _float3(1.f, 1.5f, 4.f);
    AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 1.f);
    if (FAILED(__super::Add_Component(m_eLevelID, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CMissile_Truck::Add_PartObjects()
{
    // 차량
    CTruckBody::TRUCKBODY_DESC pTruckBodyDesc{};
    pTruckBodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    pTruckBodyDesc.fRotationPerSec = 30.f;
    pTruckBodyDesc.m_eLevelID = m_eLevelID;
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_MissileTruck_Body"), MISSILETRUCK_BODY, &pTruckBodyDesc)))
        return E_FAIL;
    m_pTruckBody = static_cast<CTruckBody*>(m_PartObjects[MISSILETRUCK_BODY]);

    // 발사대
    CTruckShooter::TRUCKSHOOTER_DESC pShooter{};
    pShooter.fRotationPerSec = 30.f;
    pShooter.m_eLevelID = m_eLevelID;
    pShooter.fPosition = { 302.199f, 4.3f, 575.615f };
    pShooter.fScale = m_fScale;
    pShooter.iRound = m_iRound;
    m_pShooter = static_cast<CTruckShooter*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, TEXT("Layer_MissileTruckShooter"), TEXT("Prototype_GameObject_MissileTruck_Shooter"), &pShooter));
    
    // 추적기
    CTracker::TRACKER_DESC pTracker{};
    pTracker.eID = m_eLevelID;
    pTracker.fPosition = _float3(601.485f, 300.f, 326.525f); // 위치 나중에 따로 설정해주자
    pTracker.fScale = m_fScale;
    pTracker.fSpeedPerSec = 10.f;
    pTracker.iRound = m_iRound;
    pTracker.ShooterModelIdx = m_pShooter->Get_ModelIdx();
    pTracker.pPlayer = m_pPlayer;
    m_pTracker = static_cast<CTracker*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Tracker"), &pTracker));

    // 미사일
    CTruck_Missile::MISSILE_DESC pMissile{};
    pMissile.eID = m_eLevelID; 
    pMissile.fPosition = { 302.98f,15.14f, 584.404f };
    pMissile.fScale = m_fScale;
    pMissile.fSpeedPerSec = 10.f;
    pMissile.fRotationPerSec = 30.f;
    pMissile.ShooterModelIdx = m_pShooter->Get_ModelIdx();
    pMissile.m_pTracker = m_pTracker;
    m_pMissile = static_cast<CTruck_Missile*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_YARD, TEXT("Layer_Missile"), TEXT("Prototype_GameObject_Missile"), &pMissile));


    return S_OK;
}

HRESULT CMissile_Truck::Bind_ShaderResources()
{

    return S_OK;
}

CMissile_Truck* CMissile_Truck::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMissile_Truck* pInstance = new CMissile_Truck(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMissile_Truck");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMissile_Truck::Clone(void* pArg)
{
    CMissile_Truck* pInstance = new CMissile_Truck(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CMissile_Truck");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CMissile_Truck::Free()
{
    __super::Free();
    Safe_Release(m_pColliderCom);


}
