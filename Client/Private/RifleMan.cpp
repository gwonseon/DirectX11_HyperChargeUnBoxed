#include "stdafx.h"
#include "..\Public\RifleMan.h"

#include "GameInstance.h"
#include "RifleMan_Defines.h"
#include <Trap_Marks.h>
#include <Monster_Bullet.h>

CRifleMan::CRifleMan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
    
{
}

CRifleMan::CRifleMan(const CRifleMan& Prototype)
    : CMonster{ Prototype }
    , m_pCurrentState(new CRifleMan_Move())
{
}

HRESULT CRifleMan::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRifleMan::Initialize(void* pArg)
{
    RIFLEMAN_DESC* pDesc = static_cast<RIFLEMAN_DESC*>(pArg);
    m_vecTargetPos = pDesc->vecTargetPos;
    m_vecStoreTargetPos = *m_vecTargetPos;
    m_pTrapLayer = pDesc->pTrapLayer;
    m_iCell_Idx = pDesc->iCell_Idx;
    m_pPlayer = pDesc->pPlayer;
    m_eLevel = pDesc->eID;
    m_matPlayerWorld = pDesc->matPlayerWorld;
    m_matBrainCoreWorld = pDesc->matBrainCoreWorld;
    m_pBuild = pDesc->m_pBuild;
    
    m_pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(m_eLevel, TEXT("Layer_PlayerBuild"), TEXT("Com_Collider_AABB")));

    pDesc->fScale = _float3(2.f, 2.f, 2.f);
    pDesc->fSpeedPerSec = 5.f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_fHp = 50.f;
    m_fEnergy = 0.f;
    m_fAttack = 0.f;

    return S_OK;
}

void CRifleMan::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

    vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    vPos = XMVectorSetY(vPos, 0.f);
    XMStoreFloat3(&m_fPos, vPos);
   
    vPlayerPos = XMVectorSet(m_matPlayerWorld->_41, m_matPlayerWorld->_42, m_matPlayerWorld->_43, 1.0f);


}

void CRifleMan::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    // 콜라이더 업데이트
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
    // 상태패턴 업데이트
    m_pCurrentState->Update(this, fTimeDelta);

    _float fDistance = m_pTransformCom->Cal_Distance_vec(vPlayerPos, vPos);
    // 사정거리 안에 플레이어가 없으면 
    if (fDistance > 1500.f)
    {
        if (m_bFind_Path == false)
        {
            Path = m_pTransformCom->PathFind(0.f, m_pNavigationCom, m_pNavigationCom->Get_CurrentCell_Index(), 99);
            m_bFind_Path = true;
        }
        if (m_fTime_For_Target >= 3.f) // 항상 검사하기엔 검사량이 많아서 검사 빈도수를 줄여줌
        {
            m_fTime_For_Target = 0.f;
            _int iCheck_Count = 0;
            // 트랩마다 위치 검사해서 가까이에 있으면 트랩을 향해 공격 진행
            for (auto pTrap : m_pTrapLayer->Get_GameObject_List())
            {
                if (static_cast<CTrap_Marks*>(pTrap)->Get_Build_Done() == true)
                {
                    m_vecNewTargetPos = static_cast<CTrap_Marks*>(pTrap)->Get_TrapPos();
                    // 근접 공격이기 때문에 먼거리에서 트랩을 찾을 필요는 없음
                    if (m_pTransformCom->Cal_Distance_vec(m_vecNewTargetPos, vPos) <= 2500 && static_cast<CTrap_Marks*>(pTrap)->Get_knockdown() == false)
                    {
                        // 새 타겟으로 바꿔줌
                        m_vecTargetPos = &m_vecNewTargetPos;
                        break;
                    }
                }
                ++iCheck_Count;
            }
            // 새로운 타겟이 근처에 없으면 기록해뒀던 브레인 코어 공격
            if (iCheck_Count == m_pTrapLayer->Get_GameObjectList_Size())
            {
                m_vecTargetPos = &m_vecStoreTargetPos;
            }
        }
            // 사정거리 안에 들어가면
        if (m_pTransformCom->Cal_Distance_vec(vPos, *m_vecTargetPos) <= 800.f)
        {
            _vector vecTargetPos = *m_vecTargetPos;
            vecTargetPos = XMVectorSetY(vecTargetPos, 1.f);
            m_pTransformCom->LookAt(vecTargetPos);
            // 애니메이션 변경
            m_pCurrentState->Fire(this);
            m_pModelCom->Play_Animation(fTimeDelta, true);
            // 총알 생성
            if (m_fShot_Time_Delay >= 4.f)
            {
                if (m_iShot_Count < 3)
                {
                    
                    CMonster_Bullet::MONSTER_BULLET_DESC Desc{};
                    Desc.eID = m_eLevel;
                    _float3 fBulletPos = m_fPos;
                    fBulletPos.y = 2.f;
                    Desc.fPosition = fBulletPos;
                    Desc.m_iModelNumber = 1;
                    Desc.eType = CMonster_Bullet::RIFLEMAN_BULLET;
                    Desc.vDir = XMVector3Normalize(vecTargetPos - vPos);
                    Desc.m_pBuild = m_pBuild;
                    Desc.pPlayer = m_pPlayer;
                    static_cast<CMonster_Bullet*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_GAMEPLAY, TEXT("MonsterBullet_Layer"), TEXT("Prototype_GameObject_MonsterBullet"), &Desc));
                    m_iShot_Count++;
                }
                else
                {
                    m_fShot_Time_Delay = 0.f;
                    m_iShot_Count = 0;
                }
            }
            m_fShot_Time_Delay += fTimeDelta;
           
        }
        else // 사정거리 밖일 때 움직임
        {
            // 길찾기 수행
            if (m_pTransformCom->Cal_Distance(Path.front(), m_fPos) <= 100.f)
            {
                if (Path.size() > 1)
                    Path.erase(Path.begin());
            }
            m_pTransformCom->LookAt(XMVectorSet(Path.front().x, Path.front().y, Path.front().z, 1.f));
            m_pCurrentState->Move(this);
            //  움직임 
            if (m_pModelCom->Play_Animation(fTimeDelta, false))
            {// 애니메이션 끝나면 초기화
                m_fMoveSpeed = 0.f;
                m_fMoveTime = 0.f;
            }
            else
            {
                // 움직임 계산
                m_fMoveTime += fTimeDelta;
                _float fRatio = m_fMoveTime / 2.f;
                m_fMoveSpeed = 10.f * sin(fRatio * 3.141592f);
                _vector vMovePos = vPos + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * fTimeDelta * m_fMoveSpeed);
                m_pTransformCom->Go_Straight_Nav_Type2(fTimeDelta, vMovePos, m_pNavigationCom);
            }
        }
    
    }
    else
    {
        _vector vecTarget = vPlayerPos;
        vecTarget = XMVectorSetY(vecTarget, 1.f);
        m_pTransformCom->LookAt(vecTarget);
        m_bFind_Path = false;
        // 사정거리 밖에 있으면
        if (fDistance > 700.f)
        {
            m_pCurrentState->Move(this);
            //  움직임 
            if (m_pModelCom->Play_Animation(fTimeDelta, false))
            {// 애니메이션 끝나면 초기화
                m_fMoveSpeed = 0.f;
                m_fMoveTime = 0.f;
            }
            else
            {
                // 움직임 계산
                m_fMoveTime += fTimeDelta;
                _float fRatio = m_fMoveTime / 2.f;
                m_fMoveSpeed = 10.f * sin(fRatio * 3.141592f);
                _vector vMovePos = vPos + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * fTimeDelta * m_fMoveSpeed);
                m_pTransformCom->Go_Straight_Nav_Type2(fTimeDelta, vMovePos, m_pNavigationCom);
            }
        }
        else
        {
            // 애니메이션 변경
            m_pCurrentState->Fire(this);
            m_pModelCom->Play_Animation(fTimeDelta, true);
            // 총알 생성
            if (m_fShot_Time_Delay >= 4.f)
            {
                if (m_iShot_Count < 3)
                {
                    CMonster_Bullet::MONSTER_BULLET_DESC Desc{};
                    Desc.eID = m_eLevel;
                    _float3 fBulletPos = m_fPos;
                    fBulletPos.y = 2.f;
                    Desc.fPosition = fBulletPos;
                    Desc.m_iModelNumber = 1;
                    Desc.eType = CMonster_Bullet::RIFLEMAN_BULLET;
                    Desc.vDir = XMVector3Normalize(vPlayerPos - vPos);
                    Desc.m_pBuild = m_pBuild;
                    Desc.pPlayer = m_pPlayer;
                    static_cast<CMonster_Bullet*>(m_pGameInstance->Add_GameObject_ToLayer_ReturnObject(LEVEL_GAMEPLAY, TEXT("MonsterBullet_Layer"), TEXT("Prototype_GameObject_MonsterBullet"), &Desc));
                    m_iShot_Count++;
                }
                else
                {
                    m_fShot_Time_Delay = 0.f;
                    m_iShot_Count = 0;
                }
            }
            m_fShot_Time_Delay += fTimeDelta;
        }
    }

    // 전체 진행 시간에서 현재 시간 나눠서 비율 구하기
    // 최고속도 * sin(비율 *Pi)
   

}

void CRifleMan::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CRifleMan::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
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

HRESULT CRifleMan::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    const _wstring Model_Component = TEXT("Prototype_Component_Model_Anim");
    const _wstring Model_Component_Result = Model_Component + to_wstring(ANIM_RIFLEMAN);
    if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider_OBB */
    CBounding_Sphere::BOUND_SPHERE_DESC			SphereDesc{};
    SphereDesc.fRadius = 1.5f;
    SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
        return E_FAIL;

    // For.Com_Navigation
    CNavigation::NAVIGATION_DESC		Desc{};
    Desc.iCurrentCellIndex = m_iCell_Idx;
    switch (m_eLevel)
    {

    case Client::LEVEL_GAMEPLAY:
    {
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
            TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
            return E_FAIL;
        break;
    }
    case Client::LEVEL_YARD:
    {
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Yard"),
            TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
            return E_FAIL;
        break;
    }

    default:
        break;
    }

    return S_OK;
}

HRESULT CRifleMan::Bind_ShaderResources()
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

CRifleMan* CRifleMan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRifleMan* pInstance = new CRifleMan(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CRifleMan");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CRifleMan::Clone(void* pArg)
{
    CRifleMan* pInstance = new CRifleMan(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CRifleMan");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CRifleMan::Free()
{
    __super::Free();
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pNavigationCom);

}
