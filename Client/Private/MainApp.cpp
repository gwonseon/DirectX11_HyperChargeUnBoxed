#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "GameInstance.h"

#include "Level_Loading.h"
#include "BackGround.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Box.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	D3D11_RASTERIZER_DESC		RSDesc{};
	D3D11_BLEND_DESC			BSDesc{};
	D3D11_DEPTH_STENCIL_DESC	DSSDesc{};


	/*m_pDevice->CreateRasterizerState(&RSDesc, &pRSState);
	m_pContext->RSSetState(pRSState);
	m_pContext->OMSetDepthStencilState();
	m_pContext->OMSetBlendState();*/

	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	/* 내 게임에 필요한 필수 기능들에 대한 초기화과정을 수행한다. */
	/* 그래픽 카드를 초기화하낟. */
	/* 입력 장치를 초기화한다. */
	/* 오브젝트, 컴포넌트 매니져를 사용할 준비를 한다. */
	/* 등등등등 */
	ENGINE_DESC			EngineDesc{};
	EngineDesc.hInstance = g_hInst;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.isWindowed = true;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = LEVEL_END;

	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;


	// 구미시 폰트
	if (FAILED(m_pGameInstance->Add_Font(TEXT("GumiFont"), TEXT("../Bin/Resources/Fonts/GumiFont.spritefont"))))
		return E_FAIL;
	// 던파 폰트
	if (FAILED(m_pGameInstance->Add_Font(TEXT("DunFont"), TEXT("../Bin/Resources/Fonts/Dunfight.spritefont"))))
		return E_FAIL;

	
	ImGui::CreateContext(); // 무조건 먼저 호출 되어야 함, 그렇지 않을 경우 다른 코드들이 정상적으로 호출되지 않음.
	ImGui::StyleColorsDark(); // Gui Style : StyleColorsClassic, StyleColorsDark, StyleColorsLight

	ImGui_ImplWin32_Init(g_hWnd); // win32 관련 초기화
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update(fTimeDelta);
}

void CMainApp::Render()
{
	if (FAILED(m_pGameInstance->Render_Begin(_float4(0.f, 0.f, 1.f, 1.f)))) // 색 11은 255 255 아니라 이거임
		return;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	m_pGameInstance->Draw();
	


	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


	if (FAILED(m_pGameInstance->Render_End()))
		return;
}

HRESULT CMainApp::Open_Level(LEVELID eLevelID)
{
	if(FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,	TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Box"),
		CVIBuffer_Box::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	// 로딩페이지 검은 배경
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/T_U_BackgroundStats_Soldiers.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/Coin/T_U_HyperCoin_%d.png"),35))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GameTitle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/LogoLevel/T_U_HyperchargeLogoBase.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GameName"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/GameLevelName%d.png"),1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BackGround_GameName"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/T_U_MenuBannerSide.dds")))))
		return E_FAIL;
	// 터레인
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.bmp"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxBoxColor"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxBoxColor.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxItem"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxItem.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCoin"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCoin.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* 트랩 셰이더 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTrap"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTrap.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCircleGuage"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCircle_Gauge.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	// 네비게이션
	if (m_pGameInstance->Find_Prototype(TEXT("Prototype_Component_Navigation")) == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
			CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Navigation.dat")))))
			return E_FAIL;
	}

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();


	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);
	CGameInstance::Release_Engine();

}

