#include "stdafx.h"
#include "..\Public\Explosion.h"

#include "GameInstance.h"
CExplosion::CExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext } 
{
}

CExplosion::CExplosion(const CExplosion& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CExplosion::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CExplosion::Initialize(void* pArg)
{
    EXPLOSION_DESC* pDesc = static_cast<EXPLOSION_DESC*>(pArg);
    m_eLevel = pDesc->eID;
    m_iModelIndex = pDesc->iModelIndex;
    m_eType = pDesc->eType;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->fPosition.x, pDesc->fPosition.y, pDesc->fPosition.z, 1.f));
    m_pTransformCom->Set_Scaling(pDesc->fScale.x, pDesc->fScale.y, pDesc->fScale.z);
    m_bAttackState = true;
    switch (m_eType)
    {
    case Client::CExplosion::EXPLOSION_TRUCK:
        m_fAttack = 100.f;
        break;
    case Client::CExplosion::EXPLOSION_TANK:
        m_fAttack = 20.f;
        break;
    case Client::CExplosion::EXPLOSION_END:
        break;
    default:
        break;
    }
   

    return S_OK;
}

void CExplosion::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
 
}

void CExplosion::Update(_float fTimeDelta)
{


    __super::Update(fTimeDelta);
}

void CExplosion::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
}

HRESULT CExplosion::Render()
{
   /* if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }*/

#ifdef _DEBUG
    if (m_bDead == false)
        m_pColliderCom->Render();
#endif
    return S_OK;
}

HRESULT CExplosion::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    /* For.Com_Collider_Sphere*/
    CBounding_Sphere::BOUND_SPHERE_DESC			SphereDesc{};
    SphereDesc.fRadius = 5.f;
    SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);
    if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CExplosion::Bind_ShaderResources()
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

CExplosion* CExplosion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CExplosion* pInstance = new CExplosion(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CExplosion");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CExplosion::Clone(void* pArg)
{
    CExplosion* pInstance = new CExplosion(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CExplosion");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CExplosion::Free()
{
    __super::Free();
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
