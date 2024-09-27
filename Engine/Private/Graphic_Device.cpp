#include "..\Public\Graphic_Device.h"


CGraphic_Device::CGraphic_Device()
    : m_pDevice{ nullptr },
    m_pDeviceContext{ nullptr }
{
}

HRESULT CGraphic_Device::Initialize(HWND hWnd, _bool isWindowed, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContext)
{
    _uint iFlag = 0;

#ifdef _DEBUG
    iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif
    D3D_FEATURE_LEVEL   FeatureLV;

    // DirectX9은 장치 초기화를 하기 위한 설정을 쭉 한 뒤 최종적으로 장치 객체를 생성한다
    // DirectX11은 우선적으로 장치 객체를 생성한 뒤 생성한 장치 객체를 통해서 기타 초기화 작업 및 설정을 해나간다


    // 그래픽 장치 초기화
    if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, &FeatureLV, &m_pDeviceContext)))
        return E_FAIL;

    // SwapChain : 더블버퍼링, 전면과 후면 버퍼를 번갈아가며 화면에 보여준다 ( Present )
    // 스왑체인 객체를 생성하였고 생성한 스왑체인 객체가 백버퍼를 내장한다.
    // 백버퍼를 생성하기 위한 ID3D11Texture2D를 만든 것이다.
    // 스왑체인 객체를 만들면서 백버퍼에 해당하는 ID3D11Texture2D 객체를 만들어서 스왑체인 객체가 내장한다.
    if (FAILED(Ready_SwapChain(hWnd, isWindowed, iWinSizeX, iWinSizeY)))
        return E_FAIL;

    // 스왑체인이 들고 있는 텍스처 2D를 가져와서 이를 바탕으로 백버퍼 렌더 타겟 뷰를 만든다
    if (FAILED(Ready_BackBufferRenderTargetView()))
        return E_FAIL;

    if(FAILED(Ready_DepthStencilView(iWinSizeX, iWinSizeY)))
        return E_FAIL;

    // 장치에 바인드해 놓을 렌더 타겟들과 뎁스스텐실뷰를 세팅한다
    // 장치는 동시에 최대 8개의 렌더 타겟을 들고 있을 수 있다.
    ID3D11RenderTargetView* pRTVs[1] = { m_pBackBufferRTV, };

    m_pDeviceContext->OMSetRenderTargets(1, pRTVs, m_pDepthStencilView);

    D3D11_VIEWPORT			ViewPortDesc;
    ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
    ViewPortDesc.TopLeftX = 0;
    ViewPortDesc.TopLeftY = 0;
    ViewPortDesc.Width = (_float)iWinSizeX;
    ViewPortDesc.Height = (_float)iWinSizeY;
    ViewPortDesc.MinDepth = 0.f;
    ViewPortDesc.MaxDepth = 1.f;

    m_pDeviceContext->RSSetViewports(1, &ViewPortDesc);

    *ppDevice = m_pDevice;
    *ppDeviceContext = m_pDeviceContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pDeviceContext);

    return S_OK;

}

HRESULT CGraphic_Device::Clear_BackBuffer_View(_float4 vClearColor)
{
    if (nullptr == m_pDeviceContext)
        return E_FAIL;

    m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, (_float*)&vClearColor);
    return S_OK;
}   

HRESULT CGraphic_Device::Clear_DepthStencil_View()
{
    if (nullptr == m_pDeviceContext)
        return E_FAIL;

    m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    return S_OK;
}

HRESULT CGraphic_Device::Present()
{
   
    if (nullptr == m_pSwapChain)
        return E_FAIL;

    // 전면 버퍼와 후면 버퍼를 교체하여 후면 버퍼를 전면으로 보여주는 역할을 한다
    // 후면 버퍼를 직접 화면에 보여줄게
    return m_pSwapChain->Present(0, 0);
}

HRESULT CGraphic_Device::Ready_SwapChain(HWND hWnd, _bool isWindowed, _uint iWinCX, _uint iWinCY)
{
    IDXGIDevice* pDevice = nullptr;
    m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

    IDXGIAdapter* pAdapter = nullptr;
    pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

    IDXGIFactory* pFactory = nullptr;
    pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

    // 스왑체인을 생성한다. = 텍스쳐를 생성하는 행위 + 스왑하는 형태
    DXGI_SWAP_CHAIN_DESC    SwapChain;
    ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

    // 백버퍼 == 텍스처
    // 텍스처 (백버퍼 == ID3D11Texture2D)를 생성하는 행위
    SwapChain.BufferDesc.Width = iWinCX;
    SwapChain.BufferDesc.Height = iWinCY;

    // 만든 픽셀 하나의 데이터 정보 : 32BIT 픽셀 생성하되 부호가 없는 정규화된 수를 저장한다
    SwapChain.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // Render_Target = 그림을 당하는 대상
    SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChain.BufferCount = 1;

    // 스왑하는 형태 : 모니터 주사율에 따라 조절해도 된다
    SwapChain.BufferDesc.RefreshRate.Numerator = 60;
    SwapChain.BufferDesc.RefreshRate.Denominator = 1;
    
    // 멀티 샘플링 : 안티얼라이징 ( 계단 현상을 방지 )
   // 나중에 후처리 렌더링 : 멀티 샘플링을 지원하지 않는다.
    SwapChain.SampleDesc.Quality = 0;
    SwapChain.SampleDesc.Count = 1;

    // 백버퍼라는 텍스처를 생성했다.
    SwapChain.OutputWindow = hWnd;
    SwapChain.Windowed = isWindowed;
    SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


    if (FAILED(pFactory->CreateSwapChain(m_pDevice, &SwapChain, &m_pSwapChain)))
        return E_FAIL;

    Safe_Release(pFactory);
    Safe_Release(pAdapter);
    Safe_Release(pDevice);

    return S_OK;
}

HRESULT CGraphic_Device::Ready_BackBufferRenderTargetView()
{
    // pBackBufferTexture 에 스왑체인의 백 버퍼를 받아온다. 받아온 백버퍼를 바탕으로 여러 View 들을 생성한다.
    // 
    if (nullptr == m_pDevice)
        return E_FAIL;

    // 내가 앞으로 사용하기 위한 용도의 텍스처를 생성하기 위한 베이스 데이터를 가지고 있는 객체
    // 내가 앞으로 사용하기 위한 용도의 텍스처  = ID3D11RenderTargetView, ID3D11ShaderResourceView, ID3D11DepthStencilView
    ID3D11Texture2D* pBackBufferTexture = nullptr;

    // 스왑체인이 들고있던 텍스처를 가져와라
    if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture)))
        return E_FAIL;


    // 실제 렌더 타겟 용도로 사용할 수 있는 텍스쳐 타입 (ID3D11RenderTargetView ) 의 객체를 생성합니다.
    if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pBackBufferRTV)))
        return E_FAIL;

    Safe_Release(pBackBufferTexture);

    return S_OK;

}

HRESULT CGraphic_Device::Ready_DepthStencilView(_uint iWinCX, _uint iWinCY)
{
   
    if (nullptr == m_pDevice)
        return E_FAIL;

    ID3D11Texture2D* pDepthStencilTexture = nullptr;

    
    D3D11_TEXTURE2D_DESC TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    // 깊이 버퍼의 픽셀은 백버퍼의 픽셀과 갯수가 동일해야만 깊이 테스트가 가능해진다
    // 픽셀의 수가 다르면 아예 렌더링을 하지 못한다
    TextureDesc.Width = iWinCX;
    TextureDesc.Height = iWinCY;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    // 동적인가 정적인가, 우린 정적으로 했음
    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    // 추후 어떤 용도로 바인딩될 수 있는 View 타입의 텍스쳐를 만들기 위한 Texture2D 인가?
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; /*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;



    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
        return E_FAIL;

        /* RenderTargetView */
        /* ShaderResourceView */
        /* DepthStencilView */

    if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilView)))
        return E_FAIL;

    Safe_Release(pDepthStencilTexture);
    return S_OK;
   
}

CGraphic_Device* CGraphic_Device::Create(HWND hWnd, _bool isWindowed, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut)
{
    CGraphic_Device* pInstance = new CGraphic_Device();
    if (FAILED(pInstance->Initialize(hWnd, isWindowed, iWinSizeX, iWinSizeY, ppDeviceOut, ppDeviceContextOut)))
    {
        MSG_BOX("Failed tot Created : CGraphic_Device");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CGraphic_Device::Free()
{
    Safe_Release(m_pSwapChain);
    Safe_Release(m_pDepthStencilView);
    Safe_Release(m_pBackBufferRTV);
    Safe_Release(m_pDeviceContext);
    Safe_Release(m_pDepthTexture);

    Safe_Release(m_pDevice);
}
