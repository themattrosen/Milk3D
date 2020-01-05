#include "GraphicsDevice.h"
#include <iostream>

namespace Milk3D
{
	void GraphicsDevice::Initialize
	(
		HWND hwnd_, 
		UINT screenWidth_, 
		UINT screenHeight_, 
		bool vsync_, 
		bool fullscreen_, 
		UINT bufferCount_, 
		UINT multisampleCount_, 
		D3D_FEATURE_LEVEL featureLevel_
	)
	{
		m_width = (screenWidth_);
		m_height = (screenHeight_);
		m_windowHandle = (hwnd_);
		m_fullscreen = (fullscreen_);
		m_bufferCount = (bufferCount_);
		m_featureLevel = (featureLevel_);
		m_viewport = D3D11_VIEWPORT({ 0.0f, 0.0f, static_cast<float>(screenWidth_), static_cast<float>(screenHeight_), 0.0f, 1.0f });
		SetMSAA(multisampleCount_);
		SetVsync(vsync_);

		SetupInitialDevice();

		CreateSwapChain();
		SetupRenderTargets();
		SetupDepthBuffer();
	}

	GraphicsDevice::~GraphicsDevice()
	{
		ReleaseResources();
	}

	void GraphicsDevice::SetVsync(bool vsync)
	{
		m_vsyncEnabled = vsync;
	}

	void GraphicsDevice::ToggleVsync()
	{
		m_vsyncEnabled = !m_vsyncEnabled;
	}

	void GraphicsDevice::SetMSAA(UINT multisampleCount_)
	{
		if (multisampleCount_ > D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT) return;

		m_multisampleCount = multisampleCount_;

		// Send out event for MSAA change
	}

	void GraphicsDevice::GetClearColor(float(&color)[4])
	{
		memcpy(&color, m_clearColor, sizeof(color));
	}

	void GraphicsDevice::EnableZBuffer()
	{
		m_deviceContext->OMSetDepthStencilState(m_depthEnabledState.Get(), 1);
	}
	void GraphicsDevice::DisableZBuffer()
	{
		m_deviceContext->OMSetDepthStencilState(m_depthDisabledState.Get(), 1);
	}

	void GraphicsDevice::StartFrame(float depth, UINT8 stencil)
	{
		m_deviceContext->OMSetRenderTargets(1, m_renderTarget.GetAddressOf(), m_depthBuffer.Get());
		m_deviceContext->ClearRenderTargetView(m_renderTarget.Get(), m_clearColor);

		// Clear the depth buffer
		m_deviceContext->ClearDepthStencilView(m_depthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);

		m_deviceContext->RSSetViewports(1, &m_viewport);
	}
	void GraphicsDevice::EndFrame()
	{
		ThrowIfFailed(m_swapChain->Present(m_vsyncEnabled ? 1 : 0, 0), "Swap Chain failed to Present to screen.");

		// If we're multisampling, we need to copy our offscreen render target into the back buffer
		if (m_multisampleCount > MSAA::None)
		{
			m_deviceContext->ResolveSubresource(m_backBuffer.Get(), 0, m_offscreenRenderTarget.Get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);
		}
	}
	void GraphicsDevice::Resize(unsigned newWidth, unsigned newHeight)
	{
		m_width = newWidth;
		m_height = newHeight;

		m_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);

		// Release all outstanding references to the swap chain's buffers
		m_renderTarget.Reset();
		m_offscreenRenderTarget.Reset();
		m_backBuffer.Reset();

		ThrowIfFailed(m_swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0), "Failed to resize buffers for swap chain.");

		SetupRenderTargets();

		m_depthBuffer.Reset();

		SetupDepthBuffer();

		m_viewport.Width = static_cast<float>(newWidth);
		m_viewport.Height = static_cast<float>(newHeight);
	}
	void GraphicsDevice::SetFullscreen(bool fullscreenState)
	{
		// TODO: After calling SetFullscreenState, the app must call ResizeBuffers before Present.
		ThrowIfFailed(m_swapChain->SetFullscreenState(fullscreenState, NULL), "Failed to set fullscreen state of Graphics device.");
		// ThrowIfFailed(swapChain->ResizeBuffers(bufferCount, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0), "AHHH");
	}

	void GraphicsDevice::ReleaseResources()
	{
		if (m_swapChain)
		{
			if (FAILED(m_swapChain->SetFullscreenState(false, NULL)))
			{
				std::cout << "Failed to set swap chain full screen state." << std::endl;
			}
		}
		m_renderTarget.Reset();
		m_swapChain.Reset();
		m_backBuffer.Reset();
		m_offscreenRenderTarget.Reset();
		m_depthBuffer.Reset();
		m_rasterState.Reset();
		m_alphaEnableBlendingState.Reset();
		m_alphaDisableBlendingState.Reset();
		m_depthEnabledState.Reset();
		m_depthDisabledState.Reset();
		m_device.Reset();
		m_deviceContext.Reset();
	}

	D3D_FEATURE_LEVEL GraphicsDevice::GetFeatureLevel() const
	{
		return m_featureLevel;
	}

	void GraphicsDevice::SetRenderingTopology(RenderingTopology topology)
	{
		m_deviceContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topology));
	}
	void GraphicsDevice::SetRenderingTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
	{
		m_deviceContext->IASetPrimitiveTopology(topology);
	}
	void GraphicsDevice::DrawIndexed(UINT indexCount, UINT startIndex, UINT startVertex)
	{
		m_deviceContext->DrawIndexed(indexCount, startIndex, startVertex);
	}
	void GraphicsDevice::DrawIndexedInstanced(UINT indexCount, UINT instanceCount, UINT startIndex, UINT baseVertex, UINT startInstance)
	{
		m_deviceContext->DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertex, startInstance);
	}

	ID3D11Device * const GraphicsDevice::GetDevice()
	{
		return GetInstance().m_device.Get();
	}
	ID3D11DeviceContext * const GraphicsDevice::GetDeviceContext()
	{
		return GetInstance().m_deviceContext.Get();
	}

	ID3D11DepthStencilView * const GraphicsDevice::GetDepthBuffer() const
	{
		return m_depthBuffer.Get();
	}

	void GraphicsDevice::SetupInitialDevice()
	{
		StoreGPUInfo();
		CreateDevice();
		SetupRenderStates();
	}

	void GraphicsDevice::StoreGPUInfo()
	{
		// Create a using namespace graphics interface factory
		ComPtr<IDXGIFactory4> factory;

		#ifdef _DEBUG
		UINT createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
		#else
		UINT createFactoryFlags = 0;
		#endif

		ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&factory)), "Failed to create DXGIFactory for current device.");

		// Adapter for GPU info and display mode
		ComPtr<IDXGIAdapter> adapter;

		// Use the factory to create an adapter for the primary GPU
		ThrowIfFailed(factory->EnumAdapters(0, &adapter), "Failed to create an adapter for GPU.");

		// Get the adapter (video card) description
		ThrowIfFailed(adapter->GetDesc(&m_gpuInfo.fullDescription), "Failed to get GPU Description.");

		// Store the dedicated video card memory in megabytes
		m_gpuInfo.vram = (m_gpuInfo.fullDescription.DedicatedVideoMemory / 1024 / 1024);

		// Convert the name of the video card to a character array and store it
		size_t stringLength = 0;
		if (wcstombs_s(&stringLength, m_gpuInfo.description, 128, m_gpuInfo.fullDescription.Description, 128) != 0)
		{
			std::cerr << "Failed to convert GPU Description to standard string." << std::endl;
		}
	}

	void GraphicsDevice::CreateDevice()
	{
		// Create the swap chain, Direct3D device, and Direct3D device context
		ThrowIfFailed(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &m_featureLevel, 1,
			D3D11_SDK_VERSION, &m_device, NULL, &m_deviceContext), "Failed to create a using namespace11 Device.");
	}

	void GraphicsDevice::CreateSwapChain()
	{
		// Create a using namespace graphics interface factory
		ComPtr<IDXGIFactory4> factory;

		#ifdef _DEBUG
		UINT createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
		#else
		UINT createFactoryFlags = 0;
		#endif

		ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&factory)), "Failed to create DXGIFactory for current device.");

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

		swapChainDesc.Width = m_width;
		swapChainDesc.Height = m_height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = FALSE;
		// Flip model requires swap chain's back buffers to have no multisampling, we create an offsreen render target with the desired samples instead
		swapChainDesc.SampleDesc = { 1, 0 };
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = m_bufferCount;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		//swapChainDesc.Flags = allowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
		swapChainDesc.Flags = 0;

		ComPtr<IDXGISwapChain1> swapChain1;
		ThrowIfFailed(factory->CreateSwapChainForHwnd
		(
			m_device.Get(),
			m_windowHandle,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1
		), "Failed to create swap chain.");

		factory->MakeWindowAssociation(m_windowHandle, DXGI_MWA_NO_ALT_ENTER);

		ThrowIfFailed(swapChain1.As(&m_swapChain), "Failed to convert swap chain.");

		factory.Reset();
	}

	void GraphicsDevice::SetupRenderTargets()
	{
		// Get the pointer to the back buffer
		ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&m_backBuffer)), "Failed to get back buffer pointer from Swap Chain.");

		// Create the render target view with the back buffer pointer
		ThrowIfFailed(m_device->CreateRenderTargetView(m_backBuffer.Get(), nullptr, &m_renderTarget), "Failed to create a render target view.");

		if (m_multisampleCount > MSAA::None)
		{
			m_renderTarget.Reset();
			D3D11_TEXTURE2D_DESC offscreenTextureDesc;
			offscreenTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			offscreenTextureDesc.MipLevels = 1;
			offscreenTextureDesc.ArraySize = 1;
			offscreenTextureDesc.SampleDesc.Count = m_multisampleCount;
			offscreenTextureDesc.SampleDesc.Quality = 0;
			offscreenTextureDesc.Width = m_width;
			offscreenTextureDesc.Height = m_height;
			offscreenTextureDesc.Usage = D3D11_USAGE_DEFAULT;
			offscreenTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
			offscreenTextureDesc.CPUAccessFlags = 0;
			offscreenTextureDesc.MiscFlags = 0;

			ThrowIfFailed(m_device->CreateTexture2D(&offscreenTextureDesc, nullptr, &m_offscreenRenderTarget));
			ThrowIfFailed(m_device->CreateRenderTargetView(m_offscreenRenderTarget.Get(), nullptr, &m_renderTarget));
		}
		else
		{
			m_backBuffer.Reset();
		}
	}
	void GraphicsDevice::SetupDepthBuffer()
	{
		// Initialize the description of the depth buffer
		D3D11_TEXTURE2D_DESC depthBufferDesc = { 0 };

		// Set up the description of the depth buffer
		depthBufferDesc.Width = m_width;
		depthBufferDesc.Height = m_height;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = m_multisampleCount;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> depthBufferTexture;

		// Create the texture for the depth buffer using the filled out description
		ThrowIfFailed(m_device->CreateTexture2D(&depthBufferDesc, NULL, &depthBufferTexture), "Failed to create depth buffer.");

		// Initialize the description of the stencil state
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = { 0 };

		// Set up the description of the stencil state
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create the depth stencil state
		ThrowIfFailed(m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthEnabledState), "Failed to create depth stencil state");

		// Initialize the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		// Set up the depth stencil view description
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = m_multisampleCount == None ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DMS;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		// Create the depth stencil view
		ThrowIfFailed(m_device->CreateDepthStencilView(depthBufferTexture.Get(), &depthStencilViewDesc, &m_depthBuffer), "Failed to create depth stencil view.");
	}
	void GraphicsDevice::SetupRenderStates()
	{
		//*** Rasterizer State ***//
		D3D11_RASTERIZER_DESC rasterDesc = {};
		ZeroMemory(&rasterDesc, sizeof(rasterDesc));

		// Setup the raster description which will determine how and what polygons will be drawn
		rasterDesc.AntialiasedLineEnable = true;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = m_multisampleCount != None ? true : false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		// Create the rasterizer state from the description we just filled out
		ThrowIfFailed(m_device->CreateRasterizerState(&rasterDesc, &m_rasterState), "Failed to create rasterizer state.");

		// Now set the rasterizer state
		m_deviceContext->RSSetState(m_rasterState.Get());

		//*** Depth Disabled State ***//

		D3D11_DEPTH_STENCIL_DESC depthDisabledDesc = { 0 };

		// Now create a second depth stencil state which turns off the Z buffer for 2D rendering
		depthDisabledDesc.DepthEnable = false;
		depthDisabledDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthDisabledDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthDisabledDesc.StencilEnable = true;
		depthDisabledDesc.StencilReadMask = 0xFF;
		depthDisabledDesc.StencilWriteMask = 0xFF;
		depthDisabledDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthDisabledDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthDisabledDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthDisabledDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create the state using the device
		ThrowIfFailed(m_device->CreateDepthStencilState(&depthDisabledDesc, &m_depthDisabledState), "Failed to create depth stencil state.");

		// Texture blend state description
		D3D11_BLEND_DESC blendStateDescription = { 0 };

		//*** Alpha Blending Enabled State ***//

		UINT count = sizeof(blendStateDescription.RenderTarget) / sizeof(blendStateDescription.RenderTarget[0]);
		for (UINT i = 0; i < count; i++)
		{
			blendStateDescription.RenderTarget[i].BlendEnable = TRUE;
			blendStateDescription.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendStateDescription.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendStateDescription.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			blendStateDescription.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendStateDescription.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendStateDescription.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendStateDescription.RenderTarget[i].RenderTargetWriteMask = 0x0f;
		}

		// Create the blend state using the description
		ThrowIfFailed(m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState), "Failed to create blend state with alpha blending enabled.");

		//*** Alpha Blending Disabled State ***//
		for (UINT i = 0; i < count; i++)
		{
			blendStateDescription.RenderTarget[i].BlendEnable = FALSE;
		}
		ThrowIfFailed(m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState), "Failed to create blend state with alpha blending disabled.");
	}

}
