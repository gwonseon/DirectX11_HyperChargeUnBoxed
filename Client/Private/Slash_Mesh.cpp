#include "stdafx.h"
#include "..\Public\Slash_Mesh.h"

#include "GameInstance.h"

CSlash_Mesh::CSlash_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CSlash_Mesh::CSlash_Mesh(const CSlash_Mesh& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CSlash_Mesh::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSlash_Mesh::Initialize(void* pArg)
{
    SLASH_DESC* pDesc = static_cast<SLASH_DESC*>(pArg);
    m_eLevel = pDesc->eID;
    m_vecPlayerPos = pDesc->vecPlayerPos;
   m_pSocketMatrix = pDesc->pSocketMatrix;
   m_pParentMatrix = pDesc->pParentMatrix;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;
    if (FAILED(Add_Components()))
        return E_FAIL;
    m_pTransformCom->Set_Scaling(pDesc->fScale.x, pDesc->fScale.y + 5.f, pDesc->fScale.z);
    //m_vecPos = *m_vecPlayerPos;
    //m_vecPos = XMVectorSetY(m_vecPos, XMVectorGetY(m_vecPos) + 5.f);
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPos);


    return S_OK;
}

void CSlash_Mesh::Priority_Update(_float fTimeDelta)
{
    if (m_fLifeTime >= 1.f)
    {
        m_bDead = true;
    }
    m_fLifeTime += fTimeDelta;
}

void CSlash_Mesh::Update(_float fTimeDelta)
{
   m_fUValue += fTimeDelta * 2.f;

   _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
   for (size_t i = 0; i < 3; i++)
       SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
   XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
   
}

void CSlash_Mesh::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
        return;

    //m_vecPos = *m_vecPlayerPos;
    //m_vecPos = XMVectorSetY(m_vecPos, XMVectorGetY(m_vecPos) + 5.f);
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vecPos);

}

HRESULT CSlash_Mesh::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(9)))
            return E_FAIL;

        m_pModelCom->Render(i);
    } 

    return S_OK;
}

HRESULT CSlash_Mesh::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    const _wstring Model_Component = TEXT("Prototype_Component_Model_Effect");
    const _wstring Model_Component_Result = Model_Component + to_wstring(3);
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevel, Model_Component_Result,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;


    return S_OK;
}

HRESULT CSlash_Mesh::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _float fFar = m_pGameInstance->Get_CameraFar();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fTex_Move", &m_fUValue, sizeof(float))))
        return E_FAIL;
    
    

    return S_OK;
}

CSlash_Mesh* CSlash_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSlash_Mesh* pInstance = new CSlash_Mesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSlash_Mesh");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSlash_Mesh::Clone(void* pArg)
{
    CSlash_Mesh* pInstance = new CSlash_Mesh(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CSlash_Mesh");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSlash_Mesh::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
