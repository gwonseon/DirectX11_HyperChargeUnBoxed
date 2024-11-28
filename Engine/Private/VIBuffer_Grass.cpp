#include "..\Public\VIBuffer_Grass.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"

CVIBuffer_Grass::CVIBuffer_Grass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Instancing{ pDevice, pContext }
{
}

CVIBuffer_Grass::CVIBuffer_Grass(const CVIBuffer_Grass& Prototype)
    : CVIBuffer_Instancing{ Prototype }
{
    for (_int i = 0; i < aiTextureType_UNKNOWN; i++)
    {
        m_Materials[i] = Prototype.m_Materials[i];

        for (auto& iter : m_Materials[i])
            Safe_AddRef(iter);
   }
}

HRESULT CVIBuffer_Grass::Initialize_Prototype(const wstring pDataFilePath, _fmatrix PreTransformMatrix, _uint iIndex, vector<_float3> fPos, const CVIBuffer_Instancing::INSTANCING_DESC* pDesc)
{
    m_isLoop = pDesc->isLoop;
    m_iNumInstance = pDesc->iNumInstance;
    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

    HANDLE hFileRead = CreateFile(pDataFilePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFileRead)
    {
        MessageBox(NULL, L" ModelData Exporter Failed", L"Error", MB_OK);
        return E_FAIL;
    }


    ReadFile(hFileRead, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        _uint iLen{};
        ReadFile(hFileRead, &iLen, sizeof(_uint), &dwByte, nullptr);
        char* buffer = new char[iLen + 1]; // +1 for null terminator
        ReadFile(hFileRead, buffer, iLen * sizeof(_char), &dwByte, nullptr);
        buffer[iLen] = '\0';
        // 메모리 해제
        delete[] buffer;
        ReadFile(hFileRead, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
        ReadFile(hFileRead, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);

        m_iIndexStride = sizeof(_uint);
        ReadFile(hFileRead, &m_iFaceNum, sizeof(_uint), &dwByte, nullptr);

        m_iNumIndices = m_iFaceNum * 3;
        m_iNumVertexBuffers = 2;
        m_eIndexFormat = DXGI_FORMAT_R32_UINT;
        m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
#pragma region VERTEX_BUFFER
        m_iVertexStride = sizeof(VTXMESH);
        ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
        /* 할당하고자하는 메모리공간의 크기(Byte)*/
        m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
        /* 버퍼의 속성 (정적, 동적) */
        m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
        m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        m_BufferDesc.CPUAccessFlags = 0;
        m_BufferDesc.MiscFlags = 0;
        m_BufferDesc.StructureByteStride = m_iVertexStride;

        ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
        VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
        _uint iVertice = 0;
        ReadFile(hFileRead, &iVertice, sizeof(_uint), &dwByte, nullptr);
        _float3			fVerticesPos{}, fVerticesNor{}, fVerticesTangent{};
        _float2			fVerticesTex{};

        for (_uint j = 0; j < iVertice; j++)
        {
            ReadFile(hFileRead, &fVerticesPos, sizeof(_float3), &dwByte, nullptr);
            ReadFile(hFileRead, &fVerticesNor, sizeof(_float3), &dwByte, nullptr);
            ReadFile(hFileRead, &fVerticesTex, sizeof(_float2), &dwByte, nullptr);
            ReadFile(hFileRead, &fVerticesTangent, sizeof(_float3), &dwByte, nullptr);

            pVertices[j].vPosition = fVerticesPos;
            pVertices[j].vNormal = fVerticesNor;
            pVertices[j].vTexcoord = fVerticesTex;
            pVertices[j].vTangent = fVerticesTangent;
        }

        m_InitialDesc.pSysMem = pVertices;

        if (FAILED(__super::Create_Buffer(&m_pVB)))
            return E_FAIL;
        Safe_Delete_Array(pVertices);
#pragma endregion
#pragma region INDEX_BUFFER
        ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
        m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices * m_iNumInstance;
        m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
        m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        m_BufferDesc.CPUAccessFlags = 0;
        m_BufferDesc.MiscFlags = 0;
        m_BufferDesc.StructureByteStride = 0;

        ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
        _uint* pIndices = new _uint[m_iNumIndices * m_iNumInstance];
        _uint* pStore = new _uint[m_iNumIndices];
        m_iNumIndexPerInstance = m_iNumIndices;
        _uint		iNumIndices = { 0 };
        _uint iFaceSize = 0;
        _uint iIndiciesNum = 0;
        ReadFile(hFileRead, &iFaceSize, sizeof(_uint), &dwByte, nullptr);
        for (_uint j = 0; j < iFaceSize; j++)
        {
            ReadFile(hFileRead, &iIndiciesNum, sizeof(_uint), &dwByte, nullptr);
            pStore[iNumIndices++] = iIndiciesNum;
        }
        iNumIndices = 0;
        for (size_t j = 0; j < m_iNumInstance; j++)
        {
            memcpy(&pIndices[iNumIndices], &pStore[0], sizeof(_uint) * m_iNumIndices);
            iNumIndices += m_iNumIndices;
        }
        
        m_InitialDesc.pSysMem = pIndices;
        if (FAILED(__super::Create_Buffer(&m_pIB)))
            return E_FAIL;
        Safe_Delete_Array(pIndices);
        Safe_Delete_Array(pStore);
#pragma endregion

#pragma region INSTANCE_BUFFER
        ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);

        m_iInstanceVertexStride = sizeof(VTXMATRIX);

        m_InstanceBufferDesc.ByteWidth = m_iInstanceVertexStride * m_iNumInstance;

        m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        m_InstanceBufferDesc.MiscFlags = 0;
        m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;

        ZeroMemory(&m_InstanceInitialDesc, sizeof m_InstanceInitialDesc);
        m_pInstanceVertices = new VTXMATRIX[m_iNumInstance];

        for (size_t j = 0; j < m_iNumInstance; j++)
        {

            _float		fScale = m_pGameInstance->Compute_Random(pDesc->vSize.x, pDesc->vSize.y);
           

            m_pInstanceVertices[j].vRight = _float4(fScale, 0.f, 0.f, 0.f);
            m_pInstanceVertices[j].vUp = _float4(0.f, 6.f, 0.f, 0.f);
            m_pInstanceVertices[j].vLook = _float4(0.f, 0.f, fScale, 0.f);

            m_pInstanceVertices[j].vTranslation = _float4(fPos.back().x, fPos.back().y, fPos.back().z, 1.f);
            if(fPos.size() > 0)
            {
                fPos.erase(fPos.end() - 1);
            }
        }
        
        m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;

#pragma endregion
        
    

        ReadFile(hFileRead, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);

        for (size_t q = 0; q < m_iNumMaterials; q++)
        {
            for (size_t i = 0; i < aiTextureType_UNKNOWN; i++)
            {
                _uint		iNumTexture = 0;
                ReadFile(hFileRead, &iNumTexture, sizeof(_uint), &dwByte, nullptr);
                for (_uint j = 0; j < iNumTexture; j++)
                {
                    ID3D11ShaderResourceView* pSRV = { nullptr };
                    _char			szFullPath[MAX_PATH] = {};
                    _char			szExt[MAX_PATH] = {};
                    _uint			iExtLen{}, iFullPathLen{};

                    ReadFile(hFileRead, &iExtLen, sizeof(_uint), &dwByte, nullptr);
                    for (_uint k = 0; k < iExtLen; k++)
                    {
                        ReadFile(hFileRead, &szExt[k], sizeof(_char), &dwByte, nullptr);
                    }
                    ReadFile(hFileRead, &iFullPathLen, sizeof(_uint), &dwByte, nullptr);
                    for (_uint k = 0; k < iFullPathLen; k++)
                    {
                        ReadFile(hFileRead, &szFullPath[k], sizeof(_char), &dwByte, nullptr);
                    }


                    _tchar		szPerfectPath[MAX_PATH] = {};
                    MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szPerfectPath, MAX_PATH);
                    //cout << szFullPath << endl;

                    if (false == strcmp(szExt, ".dds"))
                    {
                        if (FAILED(CreateDDSTextureFromFile(m_pDevice, szPerfectPath, nullptr, &pSRV)))
                            return E_FAIL;
                    }
                    else
                    {
                        if (FAILED(CreateWICTextureFromFile(m_pDevice, szPerfectPath, nullptr, &pSRV)))
                            return E_FAIL;
                    }
                    m_Materials[i].push_back(pSRV);
                }
            }
        }
    }
    CloseHandle(hFileRead);
    return S_OK;
}

HRESULT CVIBuffer_Grass::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CVIBuffer_Grass::Bind_ShaderResource(CShader* pShader, _uint iMeshIndex, aiTextureType eMaterialType, _uint iIndex, const _char* pConstantName)
{
    if (iIndex >= m_Materials[eMaterialType].size())
        return E_FAIL;

    return pShader->Bind_SRV(pConstantName, m_Materials[eMaterialType][iIndex]);
}

CVIBuffer_Grass* CVIBuffer_Grass::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring pDataFilePath, _fmatrix PreTransformMatrix, _uint iIndex, vector<_float3> fPos, const CVIBuffer_Instancing::INSTANCING_DESC* pDesc)
{
    CVIBuffer_Grass* pInstance = new CVIBuffer_Grass(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pDataFilePath,  PreTransformMatrix,  iIndex, fPos, pDesc)))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Grass");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Grass::Clone(void* pArg)
{
    CVIBuffer_Grass* pInstance = new CVIBuffer_Grass(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Grass");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Grass::Free()
{
    __super::Free();

    for (auto& Materials : m_Materials)
    {
        for (auto& iter : Materials)
        {
            Safe_Release(iter);
        }
    }
}
