#include "..\Public\Picking.h"

#include "GameInstance.h"

CPicking::CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{

	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

}

HRESULT CPicking::Initialize(HWND hWnd, _uint iViewportWidth, _uint iViewportHeight)
{
	m_hWnd = hWnd;
	m_iViewportWidth = iViewportWidth;
	m_iViewportHeight = iViewportHeight;

	D3D11_TEXTURE2D_DESC			TextureDesc{};

	TextureDesc.Width = iViewportWidth;
	TextureDesc.Height = iViewportHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_STAGING;
	TextureDesc.BindFlags = 0;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	return S_OK;
}

_bool CPicking::isPicked(_float3* pOut)
{
	POINT			ptMouse;
	GetCursorPos(&ptMouse);

	/* 뷰포트 상의 마우스 위치를 구했다. */
	ScreenToClient(m_hWnd, &ptMouse);

	_uint			iIndex = ptMouse.y * m_iViewportWidth + ptMouse.x;

	m_pGameInstance->Copy_RT_Resource(TEXT("Target_Depth"), m_pTexture2D);

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);

	_float4* pPixel = static_cast<_float4*>(SubResource.pData) + iIndex;
	if (pPixel == nullptr)
		return false;
	_float3			vWorldPos = {};

	/* 투영공간상의 위치를 구한다. */
	vWorldPos.x = ptMouse.x / (m_iViewportWidth * 0.5f) - 1.f;
	vWorldPos.y = ptMouse.y / (m_iViewportHeight * -0.5f) + 1.f;
	vWorldPos.z = pPixel->x;

	/* 뷰공간상의 위치를 구한다. */
	_vector			vPosition = XMVector3TransformCoord(XMLoadFloat3(&vWorldPos), m_pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ));

	/* 월드공간상의 위치를 구한다. */
	vPosition = XMVector3TransformCoord(vPosition, m_pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW));

	m_pContext->Unmap(m_pTexture2D, 0);

	XMStoreFloat3(pOut, vPosition);

	return _bool(pPixel->w);
}


_bool CPicking::isComputeHeight(_fvector vTargetPos, _float3* pOut)
{
	/* 받아온 객체의 월드위치를 직교투영한 상태대로 투영공간상의 위치로 변환하고. */
	/* 그 투영공간상의 좌표를 텍스쳐 상의 좌표로 변환하낟. */

	_float4x4			ViewMatrix, ProjMatrix;

	// XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(64.5f, 20.f, 64.5f, 1.f), XMVectorSet(64.5f, 0.f, 64.5f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	_matrix			matView = XMMatrixIdentity();
	matView.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	matView.r[1] = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	matView.r[2] = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	matView.r[3] = XMVectorSet(XMVectorGetX(vTargetPos), 20.f, XMVectorGetZ(vTargetPos), 1.f);

	XMStoreFloat4x4(&ViewMatrix, XMMatrixInverse(nullptr, matView));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixOrthographicLH(200.f, 200.f, 0.f, 30.f));


	_vector		vProjPos = XMVector3TransformCoord(vTargetPos, XMLoadFloat4x4(&ViewMatrix));
	vProjPos = XMVector3TransformCoord(vProjPos, XMLoadFloat4x4(&ProjMatrix));

	_float2		vTexcoord;
	vTexcoord.x = XMVectorGetX(vProjPos) * m_iViewportWidth * 0.5f + m_iViewportWidth * 0.5f;
	vTexcoord.y = XMVectorGetY(vProjPos) * m_iViewportHeight * -0.5f + m_iViewportHeight * 0.5f;

	_uint			iIndex = (_uint)vTexcoord.y * m_iViewportWidth + (_uint)vTexcoord.x;

	m_pGameInstance->Copy_RT_Resource(TEXT("Target_Height"), m_pTexture2D);

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);

	_float4* pPixel = static_cast<_float4*>(SubResource.pData) + iIndex;

	m_pContext->Unmap(m_pTexture2D, 0);
	if (pPixel == nullptr)
		return false;
	XMStoreFloat3(pOut, XMVectorSetY(vTargetPos, pPixel->x));

	return _bool(pPixel->w);
}

CPicking* CPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iViewportWidth, _uint iViewportHeight)
{
	CPicking* pInstance = new CPicking(pDevice, pContext);

	if (FAILED(pInstance->Initialize(hWnd, iViewportWidth, iViewportHeight)))
	{
		MSG_BOX("Failed to Created : CPicking");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPicking::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pTexture2D);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
