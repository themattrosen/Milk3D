#pragma once

#include "Graphics/Headers/DX11.h"
#include "Graphics/Headers/GraphicsCommon.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace Milk3D
{
	struct GPUInfo
	{
		size_t vram = 0;
		char description[128] = { 0 };
		DXGI_ADAPTER_DESC fullDescription = { 0 };
	};

	//! Enum for Multisample Antialiasing count level
	enum MSAA
	{
		None = 1, // 1
		Half = 2, // 2x
		Max = 4,  // 4x
	};

	class GraphicsDevice
	{
	public:
		static GraphicsDevice & GetInstance() { static GraphicsDevice instance; return instance; }

		void Initialize(HWND hwnd, UINT screenWidth, UINT screenHeight, bool vsync, bool fullscreen,
			UINT bufferCount = 2, UINT multisampleCount = None, D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1);

		~GraphicsDevice();

		void SetVsync(bool vsync);
		void ToggleVsync();
		void SetMSAA(UINT multisampleCount);

		void StartFrame(float depth = 1.0f, UINT8 stencil = 0);
		void EndFrame();
		void Resize(unsigned width, unsigned height);
		void SetFullscreen(bool fullscreen);
		void ReleaseResources();
		D3D_FEATURE_LEVEL GetFeatureLevel() const;

		void EnableZBuffer();
		void DisableZBuffer();

		ID3D11Device * const GetDevice();
		ID3D11DeviceContext * const GetDeviceContext();

		ID3D11DepthStencilView * const GetDepthBuffer() const;
		ID3D11RenderTargetView * const GetRenderTarget() const { return m_renderTarget.Get(); }

		// Don't allow copying of a Device
		DELETE_COPY(GraphicsDevice);

	private:
		GraphicsDevice() = default;

		//*** Graphics resources ***//

		ComPtr<ID3D11Device> m_device;
		ComPtr<ID3D11DeviceContext> m_deviceContext;

		ComPtr<IDXGISwapChain4> m_swapChain;
		ComPtr<ID3D11Texture2D> m_backBuffer;
		ComPtr<ID3D11Texture2D> m_offscreenRenderTarget;
		ComPtr<ID3D11DepthStencilView> m_depthBuffer;

		ComPtr<ID3D11RenderTargetView> m_renderTarget;

		D3D11_VIEWPORT m_viewport;
		D3D_FEATURE_LEVEL m_featureLevel;

		//*** Rendering States ***//

		ComPtr<ID3D11RasterizerState> m_rasterState;
		ComPtr<ID3D11BlendState> m_alphaEnableBlendingState;
		ComPtr<ID3D11BlendState> m_alphaDisableBlendingState;
		ComPtr<ID3D11DepthStencilState> m_depthEnabledState;
		ComPtr<ID3D11DepthStencilState> m_depthDisabledState;

		//*** General Members ***//
		float m_clearColor[4] = { 0,0,0,1 };
		UINT m_width = 0;
		UINT m_height = 0;
		UINT m_bufferCount = 0;
		UINT m_multisampleCount;
		bool m_fullscreen = false;
		bool m_vsyncEnabled;
		GPUInfo m_gpuInfo;

		//*** Graphics resources ***//
		HWND m_windowHandle = nullptr;

		//*** Setup functions ***//

		void SetupInitialDevice();

		void StoreGPUInfo();

		// Create ID3D11 device
		void CreateDevice();

		void CreateSwapChain();
		void SetupRenderTargets();
		void SetupDepthBuffer();
		void SetupRenderStates();
	};

	//------------------------------------------------------------------------------
	// Public Functions:
	//------------------------------------------------------------------------------


} // Namespace: DX11

//------------------------------------------------------------------------------
// Other:
//------------------------------------------------------------------------------

