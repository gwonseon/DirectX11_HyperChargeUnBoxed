#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	// 그래픽 디바이스 초기화, 핸들, 윈도우 사이즈, 디바이스랑 컨텍스트 피룡
	HRESULT Initialize(HWND hWnd, _bool isWindowed, _uint iWinSizeX, _uint iWinSizeY, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppDeviceContext);
	HRESULT Clear_BackBuffer_View(_float4 vClearColor); // 백버퍼 인자 색으로 지운다
	HRESULT Clear_DepthStencil_View(); // 깊이 버퍼 + 스텐실 버퍼를 지운다
	HRESULT Present(); // 후면 버퍼를 전면 버퍼로 교체한다. ( 백 버퍼를 화면에 직접 보여준다) 

private:
	// 메모리 할당 ( 정점 버퍼, 인덱스 버퍼, 텍스처 로드, 쉐이더 객체를 생성한다. ) 컴객체의 생성과 관련된 역할
	// 추가적으로 생성된 모든 스레드에서 상용하는데 전혀 문제가 없다
	ID3D11Device*				m_pDevice = { nullptr }; 

	// 기능 실행 ( 바인딩 작업, 정점 버퍼를 SetVertexBuffer(), SetIndexBuffer(), Apply())
	// 그린다 DrawIndexed()
	// 컨텍스트 객체를 생성해낸 스레드 외에 스레드에서는 사용해서는 안된다
	// 고정기능렌더링파이프라인 : 월드 뷰 투영행렬을 바인딩 + 텍스처 정보를 바인딩
	// 생성된 스레드로 그려도 된다.
	ID3D11DeviceContext*		m_pDeviceContext = { nullptr };

	// 후면 버퍼와 전면 버퍼를 교체해가면서 화면에 보여주는 역할을 한다.
	IDXGISwapChain* m_pSwapChain = { nullptr };


private:
	// 실제로 사용하기 위한 텍스쳐 타입 뒤에는 View 가 붙여져 있다.


	// 셰이더에 전달할 수 있는 텍스처 타입
	/// ID3D11ShaderResourceView 

	// 렌더 타겟으로 사용될 수 있는 텍스처 타입
	ID3D11RenderTargetView*		m_pBackBufferRTV = { nullptr };

	// 텍스처를 표현하는 사전 객체 타입이다.
	// 실제 그리기를 수행하는 역할이 아닌 실제 용도에 맞게 그리기위한 텍스처 객체를 만들기 위해 존재하는 느낌이다.
	// 픽셀의 락언락을 통해 색을 강제로 바꾸거나 파일로 출력하거나 등등의 일은 가능하다
	// 픽셀의 색을 샘플링해서 화면에 그리는 작업은 불가능하다.
	// 렌더 타겟용으로 사용이 불가능하다
	// 깊이 버퍼용으로 사용이 불가능하다.

	ID3D11Texture2D*			m_pDepthTexture = { nullptr };

	// 깊이 스텐실 버퍼로서 사용될 수 있는 타입
	ID3D11DepthStencilView*		m_pDepthStencilView = { nullptr };


private:
	// 스왑체인에게 필수적으로 필요한 데이터는 백버퍼가 필요하여 백버퍼를 생성하기 위한 정보를 던져준다.
	// 스왑체인을 만들었다 == 백버퍼(텍스처) 가 생성된다.
	//SwapChain 객체를 만들면서 백버퍼에 해당하는 ID3D11Texture2D 객체를 만들어서 스왑체인 객체가 내장하게 한다.
	HRESULT Ready_SwapChain(HWND hWnd, _bool isWindowed, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView(); // 내가 앞으로 사용하기 위한 용도의 텍스쳐를 만드는 함수
	HRESULT Ready_DepthStencilView(_uint iWinCX, _uint iWinCY); // 깊이 스텐실 텍스처를 만드는 함수

public:
	static CGraphic_Device* Create(HWND hWnd, _bool isWindowed, _uint iWinSizeX, _uint iWinSizeY, _Out_ ID3D11Device** ppDeviceOut, _Out_ ID3D11DeviceContext** ppDeviceContextOut);
	virtual void Free() override;

};

END