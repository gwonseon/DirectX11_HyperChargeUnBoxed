
#include "stdafx.h"
#include "..\Public\Bullet.h"

#include "GameInstance.h"
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
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;
    
    // 왼쪽으로 회전시키는 코드 근데 이럼 안됨
    //_matrix matRotateY = XMMatrixRotationY(XMConvertToRadians(-30.f));
    //_vector vRotatedDir = XMVector3TransformNormal(m_vecWeaponDir, matRotateY);
    //    m_vecWeaponDir = vRotatedDir;

    m_fBullet_Move = 0.f;
    iRand = rand() % 3;
    iRand = 2;
    m_pTransformCom->Set_Scaling(0.05f, 0.05f, 0.05f);
    m_bChange_Root = false; // 타겟 위치 도달 후 총알 궤적 변경
    return S_OK;
}

void CBullet::Priority_Update(_float fTimeDelta)
{
    m_fBullet_Move += (fTimeDelta * 110.f);

    if (m_bChange_Root == false)
    {
        vTargetPos = m_vecWeaponPos + XMVector3Normalize(m_vecWeaponDir) * m_fBullet_Move;
        distance = XMVectorGetX(XMVector3Length(vTargetPos - m_vecCameraAt));

        if (distance <= 2.f)  
        {
            m_fBullet_Move = 0.f;
            m_bChange_Root = true;
        }
    }
    else
    {
        vTargetPos = vTargetPos + XMVector3Normalize(m_vecCameraAt -m_vecCameraPos) * m_fBullet_Move;
    }
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTargetPos);
}

void CBullet::Update(_float fTimeDelta)
{
}

void CBullet::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
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

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

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
