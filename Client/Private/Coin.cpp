#include "stdafx.h"
#include "..\Public\Coin.h"

#include "GameInstance.h"

CCoin::CCoin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CCoin::CCoin(const CCoin& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CCoin::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCoin::Initialize(void* pArg)
{
    COIN_DESC* pDesc = static_cast<COIN_DESC*>(pArg);
    m_fPickingPos = pDesc->fPosition;
    m_fScale = pDesc->fScale;
    m_eLevel = pDesc->eID;
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fPickingPos.x, m_fPickingPos.y, m_fPickingPos.z, 1.f));
    m_iCoin = 10;

    return S_OK;
}

void CCoin::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
    {
        return;
    }
    m_pTransformCom->Set_Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fPickingPos.x, m_fPickingPos.y, m_fPickingPos.z, 1.f));
    
    // È¸Àü
    _vector Axis = { 0.f, 1.f, 0.f };
    m_pTransformCom->Turn(Axis, fTimeDelta * 0.3f);
    __super::Priority_Update(fTimeDelta);
}

void CCoin::Update(_float fTimeDelta)
{
    if (m_bDead)
    {
        return;
    }
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

    __super::Update(fTimeDelta);
}

void CCoin::Late_Update(_float fTimeDelta)
{
    if(m_bDead == false)
    {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
            return;
    }
 
}

HRESULT CCoin::Render()
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
    if (m_bDead == false)
        m_pColliderCom->Render();
#endif
    return S_OK;
}

HRESULT CCoin::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCoin"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    const _wstring Model_Component = TEXT("Prototype_Component_Model_Environment");
    const _wstring Model_Component_Result = Model_Component + to_wstring(206);
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider_Sphere*/
    CBounding_Sphere::BOUND_SPHERE_DESC			SphereDesc{};
    SphereDesc.fRadius = 0.3f;
    SphereDesc.vCenter = _float3(0.f, 0.f , 0.f);

    if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
        return E_FAIL;

    return S_OK;

}

HRESULT CCoin::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_SecondMatrix", m_pModelCom->Get_SecondPreTransform())))
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

CCoin* CCoin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCoin* pInstance = new CCoin(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCoin");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCoin::Clone(void* pArg)
{
    CCoin* pInstance = new CCoin(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CCoin");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CCoin::Free()
{
    __super::Free();
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
