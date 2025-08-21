#include "..\Public\Renderer.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BlendObject.h"

#include "VIBuffer_Rect.h"
#include "Shader.h"


_uint		g_iSizeX = 8192;
_uint		g_iSizeY = 4608;

CRenderer::CRenderer(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
	: m_pDevice{pDevice}
	,m_pContext{pContext}
	,m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
	_uint		iNumViewports = {1};

	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports,&ViewportDesc);

	/* For.Target_Diffuse */
	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"),ViewportDesc.Width,ViewportDesc.Height,DXGI_FORMAT_B8G8R8A8_UNORM,_float4(0.f,0.f,0.f,0.f))))
		return E_FAIL;
	/* For.Target_Normal */
	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"),ViewportDesc.Width,ViewportDesc.Height,DXGI_FORMAT_R16G16B16A16_UNORM,_float4(0.f,0.f,0.f,0.f))))
		return E_FAIL;
	/* For.Target_Depth */
	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"),ViewportDesc.Width,ViewportDesc.Height,
		DXGI_FORMAT_R32G32B32A32_FLOAT,_float4(1.f,1.f,1.f,1.f))))
		return E_FAIL;
	/* For.Target_PickDepth */
	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PickDepth"),ViewportDesc.Width,ViewportDesc.Height,DXGI_FORMAT_R32G32B32A32_FLOAT,_float4(0.f,0.f,0.f,0.f))))
		return E_FAIL;
	/* For.Target_LightDepth*/
	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"),g_iSizeX,g_iSizeY,DXGI_FORMAT_R32G32B32A32_FLOAT,_float4(1.f,1.f,1.f,1.f))))
		return E_FAIL;
	/* For.Target_Shade */
	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"),ViewportDesc.Width,ViewportDesc.Height,DXGI_FORMAT_R16G16B16A16_UNORM,_float4(0.f,0.f,0.f,0.f))))
		return E_FAIL;
	/* For.Target_Specular */
	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"),ViewportDesc.Width,ViewportDesc.Height,DXGI_FORMAT_R16G16B16A16_UNORM,_float4(0.f,0.f,0.f,0.f))))
		return E_FAIL;

	/* For.Target_Height */
	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Height"),ViewportDesc.Width,ViewportDesc.Height,DXGI_FORMAT_R32G32B32A32_FLOAT,_float4(0.f,0.f,0.f,0.f))))
		return E_FAIL;

	// 블룸 시즌 1
	///* For.Target_BrightExtract */
	//if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BrightExtract"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
	//	return E_FAIL;
	///* For.Target_Bloom */
	//if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
	//	return E_FAIL;
		/* For.Target_Final */
	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Final_After_Bloom"),
		ViewportDesc.Width,ViewportDesc.Height,DXGI_FORMAT_R32G32B32A32_FLOAT,
		_float4(0.f,0.f,0.f,0.f))))
		return E_FAIL;

	/* For.Target_BlurX */
	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurX"),ViewportDesc.Width,ViewportDesc.Height,DXGI_FORMAT_R32G32B32A32_FLOAT,_float4(0.f,0.f,0.f,0.f))))
		return E_FAIL;
	/* For.Target_BlurY */
	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurY"),ViewportDesc.Width,ViewportDesc.Height,DXGI_FORMAT_R32G32B32A32_FLOAT,_float4(0.f,0.f,0.f,0.f))))
		return E_FAIL;
	/* For.Target_Final */
	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Final"),ViewportDesc.Width,ViewportDesc.Height,DXGI_FORMAT_R32G32B32A32_FLOAT,_float4(0.f,0.f,0.f,0.f))))
		return E_FAIL;



	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom"),ViewportDesc.Width,ViewportDesc.Height,
		DXGI_FORMAT_R8G8B8A8_UNORM,_float4(0.f,0.f,0.f,1.f))))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_Temp"),ViewportDesc.Width,ViewportDesc.Height,
		DXGI_FORMAT_R8G8B8A8_UNORM,_float4(0.f,0.f,0.f,1.f))))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_4"),ViewportDesc.Width / 4,
		ViewportDesc.Height / 4,DXGI_FORMAT_R8G8B8A8_UNORM,
		_float4(0.f,0.f,0.f,1.f))))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_4_Temp"),ViewportDesc.Width / 4,
		ViewportDesc.Height / 4,DXGI_FORMAT_R8G8B8A8_UNORM,
		_float4(0.f,0.f,0.f,1.f))))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_8"),ViewportDesc.Width / 8,
		ViewportDesc.Height / 8,DXGI_FORMAT_R8G8B8A8_UNORM,
		_float4(0.f,0.f,0.f,1.f))))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_8_Temp"),ViewportDesc.Width / 8,
		ViewportDesc.Height / 8,DXGI_FORMAT_R8G8B8A8_UNORM,
		_float4(0.f,0.f,0.f,1.f))))
		return E_FAIL;

	//if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_44"),ViewportDesc.Width / 16,
	//	ViewportDesc.Height / 16,DXGI_FORMAT_R8G8B8A8_UNORM,
	//	_float4(0.f,0.f,0.f,1.f))))
	//	return E_FAIL;
	//if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_44_Temp"),ViewportDesc.Width / 16,
	//	ViewportDesc.Height / 16,DXGI_FORMAT_R8G8B8A8_UNORM,
	//	_float4(0.f,0.f,0.f,1.f))))
	//	return E_FAIL;

	//if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_444"),ViewportDesc.Width / 64,
	//	ViewportDesc.Height / 64,DXGI_FORMAT_R8G8B8A8_UNORM,
	//	_float4(0.f,0.f,0.f,1.f))))
	//	return E_FAIL;
	//if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_444_Temp"),ViewportDesc.Width / 64,
	//	ViewportDesc.Height / 64,DXGI_FORMAT_R8G8B8A8_UNORM,
	//	_float4(0.f,0.f,0.f,1.f))))
	//	return E_FAIL;

	// 안개
	if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Fog"),ViewportDesc.Width,
		ViewportDesc.Height,DXGI_FORMAT_R32G32B32A32_FLOAT,
		_float4(0.f,0.f,0.f,0.f))))
		return E_FAIL;


	/* For.MRT_BlurX */
	if(FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurX"),TEXT("Target_BlurX"))))
		return E_FAIL;
	/* For.MRT_BlurY */
	if(FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurY"),TEXT("Target_BlurY"))))
		return E_FAIL;
	/* For.MRT_Final */
	if(FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Final"),TEXT("Target_Final"))))
		return E_FAIL;
	///* For.MRT_Bloom */
	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom"), TEXT("Target_Bloom"))))
	//	return E_FAIL;

	/* For.MRT_Final */
	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BrightExtract"), TEXT("Target_BrightExtract"))))
	//	return E_FAIL;
	/* For.MRT_GameObjects */
	if(FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"),TEXT("Target_Diffuse"))))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"),TEXT("Target_Normal"))))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"),TEXT("Target_Depth"))))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"),TEXT("Target_PickDepth"))))
		return E_FAIL;
	/* For.MRT_LightAcc */
	if(FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"),TEXT("Target_Shade"))))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"),TEXT("Target_Specular"))))
		return E_FAIL;
	/* For.MRT_Height */
	if(FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Height"),TEXT("Target_Height"))))
		return E_FAIL;
	/* For.MRT_Shadow */
	if(FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"),TEXT("Target_LightDepth"))))
		return E_FAIL;


	/* For.MRT_Final */
	if(FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Final_After_Bloom"),TEXT("Target_Final_After_Bloom"))))
		return E_FAIL;

	// 블룸 텍스쳐들을 그려내는 타겟
	FAILED_CHECK_RETURN(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom"),TEXT("Target_Bloom")),E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_4"),TEXT("Target_Bloom_4")),E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_8"),TEXT("Target_Bloom_8")),E_FAIL);
	
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_44"),TEXT("Target_Bloom_44")),E_FAIL);
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_444"),TEXT("Target_Bloom_444")),E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_Temp"),TEXT("Target_Bloom_Temp")),E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_4_Temp"),TEXT("Target_Bloom_4_Temp")),E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_8_Temp"),TEXT("Target_Bloom_8_Temp")),E_FAIL);

	//FAILED_CHECK_RETURN(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_44_Temp"),TEXT("Target_Bloom_44_Temp")),E_FAIL);
	//FAILED_CHECK_RETURN(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_444_Temp"),TEXT("Target_Bloom_444_Temp")),E_FAIL);

	// 안개
	if(FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Fog"),TEXT("Target_Fog"))))
		return E_FAIL;


	XMStoreFloat4x4(&m_WorldMatrix,XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;

	XMStoreFloat4x4(&m_ViewMatrix,XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix,XMMatrixOrthographicLH(ViewportDesc.Width,ViewportDesc.Height,0.f,1.f));

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice,m_pContext);
	if(nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice,m_pContext,TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"),VTXPOSTEX::Elements,VTXPOSTEX::iNumElements);
	if(nullptr == m_pShader)
		return E_FAIL;


	#ifdef _DEBUG
	if(FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"),100.f,100.f,200.f,200.f)))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"),100.f,300.f,200.f,200.f)))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Depth"),100.f,500.f,200.f,200.f)))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_LightDepth"),300.f,100.f,200.f,200.f)))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Height"),300.f,300.f,200.f,200.f)))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_LightDepth"), 300.f, 500.f, 200.f, 200.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Bloom"), 300.f, 500.f, 200.f, 200.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_BrightExtract"), 500.f, 100.f, 200.f, 200.f)))
	//	return E_FAIL;
	#endif
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc,sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = g_iSizeX;
	TextureDesc.Height = g_iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;


	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if(FAILED(m_pDevice->CreateTexture2D(&TextureDesc,nullptr,&pDepthStencilTexture)))
		return E_FAIL;

	/* RenderTargetView */
	/* ShaderResourceView */
	/* DepthStencilView */

	if(FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture,nullptr,&m_LightDepthStencilView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);
	Initialize_SizeDesc();
	return S_OK;
}

HRESULT CRenderer::Add_RenderGameObject(RENDERGROUP eRenderGroup,CGameObject* pRenderGameObject)
{
	if(eRenderGroup >= RG_END ||
		nullptr == pRenderGameObject)
		return E_FAIL;
	if(pRenderGameObject->Get_Dead() == true)
		return S_OK;
	m_RenderGameObjects[eRenderGroup].push_back(pRenderGameObject);
	Safe_AddRef(pRenderGameObject);

	return S_OK;
}

HRESULT CRenderer::Add_DebugComponents(CComponent* pComponent)
{
	m_DebugComponents.push_back(pComponent);
	Safe_AddRef(pComponent);

	return S_OK;
}

HRESULT CRenderer::Draw()
{
	if(FAILED(Render_Priority()))
		return E_FAIL;
	if(FAILED(Render_Shadow()))
		return E_FAIL;

	if(FAILED(Render_Height()))
		return E_FAIL;

	if(FAILED(Render_NonBlend()))
		return E_FAIL;

	if(FAILED(Render_Lights()))
		return E_FAIL;

	// 블룸 다운 샘플링
	if(FAILED(Render_Bloom_DownSample()))
		return E_FAIL;

	if(FAILED(Render_Final()))
		return E_FAIL;


	// if (FAILED(Render_Bloom_Object()))
	// 	return E_FAIL;
	// if (FAILED(Render_BrightExtraction()))
	// 	return E_FAIL;
	// if (FAILED(Render_Bloom()))
	// 	return E_FAIL;


	if(FAILED(Render_BloomFinal()))
		return E_FAIL;

	if(FAILED(Render_Fog()))
		return E_FAIL;

	if(FAILED(Render_Blur()))
		return E_FAIL;
	if(FAILED(Render_BlurFinal()))
		return E_FAIL;
	if(FAILED(Render_NonLight()))
		return E_FAIL;
	if(FAILED(Render_Blend()))
		return E_FAIL;
	if(FAILED(Render_Last()))
		return E_FAIL;
	if(FAILED(Render_UI()))
		return E_FAIL;
	if(FAILED(Render_UI_Last()))
		return E_FAIL;

	#ifdef _DEBUG
	if(FAILED(Render_Debug()))
		return E_FAIL;
	#endif

	return S_OK;
}

void CRenderer::RenderList_Clear()
{
	for(auto& GameObjects : m_RenderGameObjects)
	{
		for(auto& pRenderGameObject : GameObjects)
			Safe_Release(pRenderGameObject);
		GameObjects.clear();
	}
}

void CRenderer::Initialize_SizeDesc()
{
	// 오리지널 사이즈
	D3D11_VIEWPORT ViewPortDesc;
	ZeroMemory(&ViewPortDesc,sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = 1280.f;
	ViewPortDesc.Height = 720.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;
	m_ViewPortDescs[SIZE_ORIGINAL] = ViewPortDesc;

	m_fdX[SIZE_ORIGINAL] = 1.f / 1280.f;
	m_fdY[SIZE_ORIGINAL] = 1.f / 720.f;

	// 4 다운 샘플링
	ViewPortDesc.Width = 1280.f / 4.f;
	ViewPortDesc.Height = 720.f / 4.f;
	m_ViewPortDescs[SIZE_DOWN_4] = ViewPortDesc;

	m_fdX[SIZE_DOWN_4] = 4.f / 1280.f;
	m_fdY[SIZE_DOWN_4] = 4.f / 720.f;

	// 8배 다운 샘플링 
	ViewPortDesc.Width = 1280.f / 8.f;
	ViewPortDesc.Height = 720.f / 8.f;
	m_ViewPortDescs[SIZE_DOWN_8] = ViewPortDesc;

	m_fdX[SIZE_DOWN_8] = 8.f / 1280.f;
	m_fdY[SIZE_DOWN_8] = 8.f / 720.f;


	//// 4x4 다운 샘플링
	//ViewPortDesc.Width = 1280.f / 16.f;
	//ViewPortDesc.Height = 720.f / 16.f;
	//m_ViewPortDescs[SIZE_DOWN_44] = ViewPortDesc;

	//m_fdX[SIZE_DOWN_44] = 16.f / 1280.f;
	//m_fdY[SIZE_DOWN_44] = 16.f / 720.f;

	//// 4x4x4 다운 샘플링
	//ViewPortDesc.Width = 1280.f / 64.f;
	//ViewPortDesc.Height = 720.f / 64.f;
	//m_ViewPortDescs[SIZE_DOWN_444] = ViewPortDesc;

	//m_fdX[SIZE_DOWN_444] = 64.f / 1280.f;
	//m_fdY[SIZE_DOWN_444] = 64.f / 720.f;

}

HRESULT CRenderer::Render_Priority()
{
	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Final"))))
		return E_FAIL;

	for(auto& pRenderGameObject : m_RenderGameObjects[RG_PRIORITY])
	{
		if(nullptr != pRenderGameObject)
			pRenderGameObject->Render();
		Safe_Release(pRenderGameObject);
	}
	m_RenderGameObjects[RG_PRIORITY].clear();
	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Final"))))
		return E_FAIL;
	return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
	m_pContext->ClearDepthStencilView(m_LightDepthStencilView,D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,1.f,0);

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc,sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)g_iSizeX;
	ViewPortDesc.Height = (_float)g_iSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;
	m_pContext->RSSetViewports(1,&ViewPortDesc);
	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"),m_LightDepthStencilView)))
		return E_FAIL;

	for(auto& pRenderGameObject : m_RenderGameObjects[RG_SHADOW])
	{
		if(nullptr != pRenderGameObject)
			pRenderGameObject->Render_Shadow();

		Safe_Release(pRenderGameObject);
	}

	m_RenderGameObjects[RG_SHADOW].clear();

	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Shadow"))))
		return E_FAIL;

	ZeroMemory(&ViewPortDesc,sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)1280.f;
	ViewPortDesc.Height = (_float)720.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1,&ViewPortDesc);

	return S_OK;
}

HRESULT CRenderer::Render_Height()
{
	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Height"))))
		return E_FAIL;

	for(auto& pRenderGameObject : m_RenderGameObjects[RG_HEIGHT])
	{
		if(nullptr != pRenderGameObject)
			pRenderGameObject->Render_Height();
		Safe_Release(pRenderGameObject);
	}
	m_RenderGameObjects[RG_HEIGHT].clear();

	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Height"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	/* Diffuse + Normal */
	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for(auto& pRenderGameObject : m_RenderGameObjects[RG_NONBLEND])
	{
		if(nullptr != pRenderGameObject)
			pRenderGameObject->Render();

		Safe_Release(pRenderGameObject);
	}

	m_RenderGameObjects[RG_NONBLEND].clear();

	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;

	return S_OK;
}



HRESULT CRenderer::Render_Lights()
{
	/* Shade */
	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return E_FAIL;

	if(FAILED(m_pShader->Bind_Matrix("g_WorldMatrix",&m_WorldMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv",m_pGameInstance->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv",m_pGameInstance->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_RawValue("g_vCamPosition",m_pGameInstance->Get_CamPosition(),sizeof(_float4))))
		return E_FAIL;
	_float fFar = m_pGameInstance->Get_CameraFar();
	if(FAILED(m_pShader->Bind_RawValue("g_fCamFar",&fFar,sizeof(_float))))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_NormalTexture",TEXT("Target_Normal"))))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_DepthTexture",TEXT("Target_Depth"))))
		return E_FAIL;
	/* 빛매니져에 접근해서 빛들을 그려라라고 하자. */
/* 빛갯수만큼 빛 매니져가 렌더를 호출한다. */
/* 빛객체 렌더안에서 사각형 버퍼를 그리자. */
	m_pGameInstance->Render_Lights(m_pShader,m_pVIBuffer);

	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_LightAcc"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Final()
{
	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Final"),nullptr,false)))
		return E_FAIL;

	if(FAILED(m_pShader->Bind_Matrix("g_WorldMatrix",&m_WorldMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;
	_float4x4			ViewMatrix,ProjMatrix;
	_float4 fPlayerPos = m_pGameInstance->Get_PlayerPos();
	_float fFar = m_pGameInstance->Get_CameraFar();
	XMStoreFloat4x4(&ViewMatrix,XMMatrixLookAtLH(XMVectorSet(fPlayerPos.x - 5.f,fPlayerPos.y + 10.f,fPlayerPos.y - 5.f,1.f),XMVectorSet(fPlayerPos.x,fPlayerPos.y,fPlayerPos.y,1.f),XMVectorSet(0.f,1.f,0.f,0.f)));
	XMStoreFloat4x4(&ProjMatrix,XMMatrixPerspectiveFovLH(XMConvertToRadians(120.f),(_float)1280.f / 720.f,0.1f,fFar));

	if(FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix",&ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix",&ProjMatrix)))
		return E_FAIL;

	if(FAILED(m_pShader->Bind_RawValue("g_fCamFar",&fFar,sizeof(_float))))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_ShadeTexture",TEXT("Target_Shade"))))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_DiffuseTexture",TEXT("Target_Diffuse"))))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_SpecularTexture",TEXT("Target_Specular"))))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_LightDepthTexture",TEXT("Target_LightDepth"))))
		return E_FAIL;

	m_pShader->Begin(3);

	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Final"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Bloom_Object()
{

	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom"))))
		return E_FAIL;

	for(auto& pRenderGameObject : m_RenderGameObjects[RG_BLOOM])
	{
		if(nullptr != pRenderGameObject)
			pRenderGameObject->Render();
		Safe_Release(pRenderGameObject);
	}
	m_RenderGameObjects[RG_BLOOM].clear();
	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom"))))
		return E_FAIL;

	return S_OK;
}
// 마지막 그린 것에서 밝기빼서 
HRESULT CRenderer::Render_BrightExtraction()
{
	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BrightExtract"),nullptr,true)))
		return E_FAIL;

	if(FAILED(m_pShader->Bind_Matrix("g_WorldMatrix",&m_WorldMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_FinalTexture",TEXT("Target_Bloom"))))
		return E_FAIL;

	m_pShader->Begin(7);

	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_BrightExtract"))))
		return E_FAIL;
	return S_OK;
}
// 블룸 주기
HRESULT CRenderer::Render_Bloom()
{
	if(FAILED(m_pShader->Bind_Matrix("g_WorldMatrix",&m_WorldMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BlurX"))))
		return E_FAIL;
	// 밝기 뺀 이미지에서
	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_FinalTexture",TEXT("Target_BrightExtract"))))
		return E_FAIL;

	m_pShader->Begin(8);

	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_BlurX"))))
		return E_FAIL;


	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BlurY"))))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_FinalTexture",TEXT("Target_BlurX"))))
		return E_FAIL;

	m_pShader->Begin(9);

	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_BlurY"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Bloom_DownSample()
{
	/* 블룸 렌더그룹의 오브젝트들 렌더링 */
	m_eNowRenderGroup = RG_BLOOM;
	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom")))) return E_FAIL;

	for(auto& pRenderGameObject : m_RenderGameObjects[RG_BLOOM])
	{
		if(nullptr != pRenderGameObject)
			pRenderGameObject->Render();

		Safe_Release(pRenderGameObject);
	}

	m_RenderGameObjects[RG_BLOOM].clear();
	
	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom")))) return E_FAIL;

	/* 블러링 */
	if(FAILED(m_pShader->Bind_Matrix("g_WorldMatrix",&m_WorldMatrix))) return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix))) return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix))) return E_FAIL;

	// 4배 다운 샘플링
	m_pContext->RSSetViewports(1,&m_ViewPortDescs[SIZE_DOWN_4]);
	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_4"),nullptr,false))) return E_FAIL;
	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_DiffuseTexture",TEXT("Target_Bloom")))) return E_FAIL;
	m_pShader->Begin(11);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_4")))) return E_FAIL;

	// 8배 다운샘플링
	m_pContext->RSSetViewports(1,&m_ViewPortDescs[SIZE_DOWN_8]);
	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_8"),nullptr,false))) return E_FAIL;
	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_DiffuseTexture",TEXT("Target_Bloom_4")))) return E_FAIL;
	m_pShader->Begin(11);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();
	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_8")))) return E_FAIL;

	// 8배 텍스처 블러 (X)
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("dX",&m_fdX[SIZE_DOWN_8],sizeof(_float)),E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("dY",&m_fdY[SIZE_DOWN_8],sizeof(_float)),E_FAIL);
	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_8_Temp")))) return E_FAIL;
	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_DiffuseTexture",TEXT("Target_Bloom_8")))) return E_FAIL;
	m_pShader->Begin(12);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();
	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_8_Temp")))) return E_FAIL;

	// 8배 텍스처 블러 (Y → 4배)
	m_pContext->RSSetViewports(1,&m_ViewPortDescs[SIZE_DOWN_4]);
	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_4"),nullptr,false))) return E_FAIL;
	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_DiffuseTexture",TEXT("Target_Bloom_8_Temp")))) return E_FAIL;
	m_pShader->Begin(13);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();
	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_4")))) return E_FAIL;

	// 4배 텍스처 블러 (X)
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("dX",&m_fdX[SIZE_DOWN_4],sizeof(_float)),E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("dY",&m_fdY[SIZE_DOWN_4],sizeof(_float)),E_FAIL);
	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_4_Temp")))) return E_FAIL;
	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_DiffuseTexture",TEXT("Target_Bloom_4")))) return E_FAIL;
	m_pShader->Begin(12);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();
	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_4_Temp")))) return E_FAIL;

	// 4배 텍스처 블러 (Y → 원본 해상도)
	m_pContext->RSSetViewports(1,&m_ViewPortDescs[SIZE_ORIGINAL]);
	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom")))) return E_FAIL;
	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_DiffuseTexture",TEXT("Target_Bloom_4_Temp")))) return E_FAIL;
	m_pShader->Begin(13);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();
	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom")))) return E_FAIL;



	//// 16배 다운 샘플링
	//m_pContext->RSSetViewports(1,&m_ViewPortDescs[SIZE_DOWN_44]);
	//if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_44"),nullptr,false))) return E_FAIL;
	//if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_DiffuseTexture",TEXT("Target_Bloom_4")))) return E_FAIL;
	//m_pShader->Begin(11);
	//m_pVIBuffer->Bind_Buffers();
	//m_pVIBuffer->Render();

	//if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_44")))) return E_FAIL;
	//// 64배 다운 샘플링
	//m_pContext->RSSetViewports(1,&m_ViewPortDescs[SIZE_DOWN_444]);
	//if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_444"),nullptr,false))) return E_FAIL;
	//if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_DiffuseTexture",TEXT("Target_Bloom_44")))) return E_FAIL;
	//m_pShader->Begin(11);
	//m_pVIBuffer->Bind_Buffers();
	//m_pVIBuffer->Render();

	//if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_444")))) return E_FAIL;

	//// 64배 텍스쳐에 대한 블러와 업스케일링, 44 텍스쳐에 대한 add 연산
	//FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("dX",&m_fdX[SIZE_DOWN_444],sizeof(_float)),E_FAIL);
	//FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("dY",&m_fdY[SIZE_DOWN_444],sizeof(_float)),E_FAIL);

	//// X 블러링
	//if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_444_Temp")))) return E_FAIL;
	//if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_DiffuseTexture",TEXT("Target_Bloom_444")))) return E_FAIL;
	//m_pShader->Begin(12);
	//m_pVIBuffer->Bind_Buffers();
	//m_pVIBuffer->Render();
	//if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_444_Temp")))) return E_FAIL;

	//// Y 블러링
	//m_pContext->RSSetViewports(1,&m_ViewPortDescs[SIZE_DOWN_44]);
	//if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_44"),nullptr,false)))
	//	return E_FAIL;
	//if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_DiffuseTexture",TEXT("Target_Bloom_444_Temp"))))
	//	return E_FAIL;
	//m_pShader->Begin(13);
	//m_pVIBuffer->Bind_Buffers();
	//m_pVIBuffer->Render();
	//if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_44")))) return E_FAIL;

	//// 16배 텍스쳐에 대한 블러와 업스케일링, 4 텍스쳐에 대한 add 연산
	//FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("dX",&m_fdX[SIZE_DOWN_44],sizeof(_float)),E_FAIL);
	//FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("dY",&m_fdY[SIZE_DOWN_44],sizeof(_float)),E_FAIL);

	//// X 블러링
	//if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_44_Temp")))) return E_FAIL;
	//if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_DiffuseTexture",TEXT("Target_Bloom_44")))) return E_FAIL;
	//m_pShader->Begin(12);
	//m_pVIBuffer->Bind_Buffers();
	//m_pVIBuffer->Render();
	//if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_44_Temp")))) return E_FAIL;

	//// Y 블러링
	//m_pContext->RSSetViewports(1,&m_ViewPortDescs[SIZE_DOWN_4]);
	//if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_4"),nullptr,false)))
	//	return E_FAIL;
	//if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_DiffuseTexture",TEXT("Target_Bloom_44_Temp"))))
	//	return E_FAIL;
	//m_pShader->Begin(13);
	//m_pVIBuffer->Bind_Buffers();
	//m_pVIBuffer->Render();
	//if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_4")))) return E_FAIL;

	//// 4배 텍스쳐에 대한 블러와 업스케일링, 블룸 텍스쳐에 대한 add 연산
	//FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("dX",&m_fdX[SIZE_DOWN_4],sizeof(_float)),E_FAIL);
	//FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("dY",&m_fdY[SIZE_DOWN_4],sizeof(_float)),E_FAIL);

	//// X 블러링
	//if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_4_Temp")))) return E_FAIL;
	//if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_DiffuseTexture",TEXT("Target_Bloom_4")))) return E_FAIL;
	//m_pShader->Begin(12);
	//m_pVIBuffer->Bind_Buffers();
	//m_pVIBuffer->Render();
	//if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_4_Temp")))) return E_FAIL;

	//// Y 블러링
	//m_pContext->RSSetViewports(1,&m_ViewPortDescs[SIZE_ORIGINAL]);
	//if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom")))) return E_FAIL;
	//if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_DiffuseTexture",TEXT("Target_Bloom_4_Temp"))))
	//	return E_FAIL;
	//m_pShader->Begin(13);
	//m_pVIBuffer->Bind_Buffers();
	//m_pVIBuffer->Render();
	//if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom")))) return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BloomFinal()
{
	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Final_After_Bloom"))))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_WorldMatrix",&m_WorldMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;

	// 마지막 그린 것에 블룸 효과 준 애 바인드
	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_FinalTexture",TEXT("Target_Final"))))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_BlurTexture",TEXT("Target_Bloom"))))
		return E_FAIL;
	m_pShader->Begin(10);

	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Final_After_Bloom"))))
		return E_FAIL;


	return S_OK;
}
HRESULT CRenderer::Render_Fog()
{
	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Fog"))))
		return E_FAIL;


	if(FAILED(m_pShader->Bind_Matrix("g_WorldMatrix",&m_WorldMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;

	float fStart = 1.f;


	if(FAILED(m_pShader->Bind_RawValue("g_FogStart",&fStart,sizeof(float))))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_RawValue("g_FogEnd",&m_fEnd,sizeof(float))))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_RawValue("g_bFog",&m_bFog,sizeof(bool))))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_FinalTexture",TEXT("Target_Final_After_Bloom"))))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_DepthTexture",TEXT("Target_Depth"))))
		return E_FAIL;
	m_pShader->Begin(14);

	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Fog"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blur()
{
	if(FAILED(m_pShader->Bind_Matrix("g_WorldMatrix",&m_WorldMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BlurX"))))
		return E_FAIL;
	// 블룸까지 합친 이미지로 마지막 블러
	//if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_FinalTexture", TEXT("Target_Final_After_Bloom"))))
	//	return E_FAIL;
	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_FinalTexture",TEXT("Target_Fog"))))
		return E_FAIL;
	m_pShader->Begin(4);

	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_BlurX"))))
		return E_FAIL;


	if(FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BlurY"))))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_FinalTexture",TEXT("Target_BlurX"))))
		return E_FAIL;

	m_pShader->Begin(5);

	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if(FAILED(m_pGameInstance->End_MRT(TEXT("MRT_BlurY"))))
		return E_FAIL;

	return S_OK;
}
HRESULT CRenderer::Render_BlurFinal()
{
	if(FAILED(m_pShader->Bind_Matrix("g_WorldMatrix",&m_WorldMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pShader->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_FinalTexture", TEXT("Target_Final_After_Bloom"))))
	//	return E_FAIL;
	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_FinalTexture",TEXT("Target_Fog"))))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader,"g_BlurTexture",TEXT("Target_BlurY"))))
		return E_FAIL;

	m_pShader->Begin(6);

	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	return S_OK;
}


HRESULT CRenderer::Render_NonLight()
{
	for(auto& pRenderGameObject : m_RenderGameObjects[RG_NONLIGHT])
	{
		if(nullptr != pRenderGameObject)
			pRenderGameObject->Render();

		Safe_Release(pRenderGameObject);
	}

	m_RenderGameObjects[RG_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	m_RenderGameObjects[RG_BLEND].sort([](CGameObject* pSour,CGameObject* pDest)->_bool
	{
		return static_cast<CBlendObject*>(pSour)->Get_Depth() > static_cast<CBlendObject*>(pDest)->Get_Depth();
	});

	for(auto& pRenderGameObject : m_RenderGameObjects[RG_BLEND])
	{
		if(nullptr != pRenderGameObject)
			pRenderGameObject->Render();

		Safe_Release(pRenderGameObject);
	}

	m_RenderGameObjects[RG_BLEND].clear();

	return S_OK;
}



HRESULT CRenderer::Render_UI()
{
	for(auto& pRenderGameObject : m_RenderGameObjects[RG_UI])
	{
		if(nullptr != pRenderGameObject)
			pRenderGameObject->Render();

		Safe_Release(pRenderGameObject);
	}

	m_RenderGameObjects[RG_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI_Last()
{
	for(auto& pRenderGameObject : m_RenderGameObjects[RG_UI_LAST])
	{
		if(nullptr != pRenderGameObject)
			pRenderGameObject->Render();

		Safe_Release(pRenderGameObject);
	}

	m_RenderGameObjects[RG_UI_LAST].clear();

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Render_Debug()
{
	for(auto& pDebugCom : m_DebugComponents)
	{
		if(nullptr != pDebugCom)
		{
		//	pDebugCom->Render();
			Safe_Release(pDebugCom);
		}
	}
	m_DebugComponents.clear();

	if(FAILED(m_pShader->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix)))
		return E_FAIL;

	if(FAILED(m_pShader->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;

	m_pVIBuffer->Bind_Buffers();

	//m_pGameInstance->Render_RT_Debug(TEXT("MRT_GameObjects"),m_pShader,m_pVIBuffer);
	//m_pGameInstance->Render_RT_Debug(TEXT("MRT_LightAcc"),m_pShader,m_pVIBuffer);
	//m_pGameInstance->Render_RT_Debug(TEXT("MRT_Height"),m_pShader,m_pVIBuffer);
	//m_pGameInstance->Render_RT_Debug(TEXT("MRT_Shadow"),m_pShader,m_pVIBuffer);
	//m_pGameInstance->Render_RT_Debug(TEXT("MRT_Final_After_Bloom"),m_pShader,m_pVIBuffer);
	//// m_pGameInstance->Render_RT_Debug(TEXT("MRT_Final"), m_pShader, m_pVIBuffer);
	//// m_pGameInstance->Render_RT_Debug(TEXT("MRT_BrightExtract"), m_pShader, m_pVIBuffer);
	//// m_pGameInstance->Render_RT_Debug(TEXT("MRT_Bloom"), m_pShader, m_pVIBuffer);	
	//m_pGameInstance->Render_RT_Debug(TEXT("MRT_Fog"),m_pShader,m_pVIBuffer);

	return S_OK;


}

#endif


HRESULT CRenderer::Render_Last()
{

	for(auto& pRenderGameObject : m_RenderGameObjects[RG_LAST])
	{
		if(nullptr != pRenderGameObject)
			pRenderGameObject->Render();
		Safe_Release(pRenderGameObject);
	}
	m_RenderGameObjects[RG_LAST].clear();

	return S_OK;
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice,ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice,pContext);

	if(FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderer::Free()
{
	__super::Free();
	Safe_Release(m_LightDepthStencilView);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);

	for(auto& pDebugCom : m_DebugComponents)
		Safe_Release(pDebugCom);
	m_DebugComponents.clear();

	for(auto& GameObjects : m_RenderGameObjects)
	{
		for(auto& pRenderGameObject : GameObjects)
			Safe_Release(pRenderGameObject);
		GameObjects.clear();
	}

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

}