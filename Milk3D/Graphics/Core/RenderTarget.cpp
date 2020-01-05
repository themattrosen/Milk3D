#include "RenderTarget.h"
#include "GraphicsDevice.h"
#include "Window.h"
#include <iostream>

namespace Milk3D
{
	void RenderTarget::Initialize(RenderTargetInfo const & renderTargetInfo_)
	{
		m_renderTargetInfo = (renderTargetInfo_);
		m_shaderResources = std::vector<ID3D11ShaderResourceView*>(renderTargetInfo_.attachments.size(), nullptr);
		m_renderTargets = std::vector<ID3D11RenderTargetView*>(renderTargetInfo_.attachments.size(), nullptr);
		m_textureCount = (static_cast<UINT>(renderTargetInfo_.attachments.size()));
		m_viewport = D3D11_VIEWPORT({ 0.0f, 0.0f, static_cast<float>(renderTargetInfo_.width), static_cast<float>(renderTargetInfo_.height), 0.0f, 1.0f });

		Setup();
		if (renderTargetInfo_.resizeWithWindow)
		{
			// TODO: Connect to window resize event
			// CONNECT(RenderTarget::resize, this);
		}
	}

	RenderTarget::~RenderTarget()
	{
		Release();
	}
	
	void RenderTarget::Use(float depth, UINT8 stencil)
	{
		if (m_isComplete)
		{
			ID3D11DeviceContext * deviceContext = GraphicsDevice::GetDeviceContext();

			// TODO: Fix this, make GraphicsDevice::GetDepthBuffer function
			ID3D11DepthStencilView * depthStencil = m_depthBuffer.Get();
			if (!m_renderTargetInfo.createDepthBuffer)
			{
				depthStencil = GraphicsDevice::GetInstance().GetDepthBuffer();
			}
			deviceContext->OMSetRenderTargets(m_textureCount, m_renderTargets.data(), depthStencil);

			for (auto target : m_renderTargets)
			{
				deviceContext->ClearRenderTargetView(target, m_clearColor);
			}

			deviceContext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH, depth, stencil);
			deviceContext->RSSetViewports(1, &m_viewport);
		}
	}
	void RenderTarget::Use(ID3D11DepthStencilView * depthStencil, bool clearDepth, float depth, UINT8 stencil)
	{
		if (m_isComplete)
		{
			ID3D11DeviceContext * deviceContext = GraphicsDevice::GetDeviceContext();

			deviceContext->OMSetRenderTargets(m_textureCount, m_renderTargets.data(), depthStencil);

			for (auto target : m_renderTargets)
			{
				deviceContext->ClearRenderTargetView(target, m_clearColor);
			}

			if (clearDepth)
			{
				deviceContext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH, depth, stencil);
			}
	
			deviceContext->RSSetViewports(1, &m_viewport);
		}
	}
	void RenderTarget::Disable()
	{
		if (m_isComplete)
		{
			GraphicsDevice & gd = GraphicsDevice::GetInstance();
			auto backBuffer = gd.GetRenderTarget();
			auto depthStencil = gd.GetDepthBuffer();
			gd.GetDeviceContext()->OMSetRenderTargets(1, &backBuffer, depthStencil);
		}
	}
	
	void RenderTarget::SetClearColor(float clearColor_[4])
	{
		memcpy(m_clearColor, clearColor_, sizeof(m_clearColor));

	}
	void RenderTarget::SetClearColor(float r, float g, float b, float a)
	{
		m_clearColor[0] = r;
		m_clearColor[1] = g;
		m_clearColor[2] = b;
		m_clearColor[3] = a;
	}
	

	void RenderTarget::Resize(UINT width_, UINT height_)
	{
		m_renderTargetInfo.width = width_;
		m_renderTargetInfo.height = height_;
		Release();
		Setup();
	}

	void RenderTarget::GetClearColor(float(&clearColor)[4])
	{
		memcpy(clearColor, m_clearColor, sizeof(clearColor));
	}

	//------------------------------------------------------------------------------
	// Private:
	//------------------------------------------------------------------------------

	void RenderTarget::Release()
	{
		m_depthBuffer.Reset();
		for (auto & renderTarget : m_renderTargets)
			SAFE_RELEASE(renderTarget);
		m_renderTargets.clear();
		for (auto & shaderResource : m_shaderResources)
			SAFE_RELEASE(shaderResource);
		m_shaderResources.clear();
	}

	void RenderTarget::Setup()
	{
		ID3D11Device * device = GraphicsDevice::GetDevice();

		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		UINT MSAA = GraphicsDevice::GetInstance().GetMSAA();

		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.SampleDesc.Count = MSAA;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Width = m_renderTargetInfo.width;
		textureDesc.Height = m_renderTargetInfo.height;

		// TODO: More customization?
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = m_renderTargetInfo.isCubemap ? (D3D11_RESOURCE_MISC_TEXTURECUBE) : 0;

		std::vector<ID3D11Texture2D*> textures(m_shaderResources.size(), nullptr);

		for (size_t i = 0; i < textures.size(); i++)
		{
			textureDesc.Format = m_renderTargetInfo.attachments[i];

			if (FAILED(device->CreateTexture2D(&textureDesc, NULL, &textures[i])))
			{
				std::cout << "Failed to create texture attachment: " << i << " for Frame Buffer." << std::endl;
				m_isComplete = false;
				return;
			}
		}

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = { };
		ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));

		// Handle multisampling and cubemaps
		if (MSAA != 1)
			renderTargetViewDesc.ViewDimension = m_renderTargetInfo.isCubemap ? D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY : D3D11_RTV_DIMENSION_TEXTURE2DMS;
		else
			renderTargetViewDesc.ViewDimension = m_renderTargetInfo.isCubemap ? D3D11_RTV_DIMENSION_TEXTURE2DARRAY : D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		for (size_t i = 0; i < m_renderTargets.size(); i++)
		{
			renderTargetViewDesc.Format = m_renderTargetInfo.attachments[i];
			if (FAILED(device->CreateRenderTargetView(textures[i], &renderTargetViewDesc, &m_renderTargets[i])))
			{
				std::cout << "Failed to create render target: " << i << " for Frame Buffer." << std::endl;
				m_isComplete = false;
				return;
			}
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = { };
		ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));

		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		if (MSAA != 1)
			shaderResourceViewDesc.ViewDimension = m_renderTargetInfo.isCubemap ? D3D11_SRV_DIMENSION_TEXTURECUBE : D3D11_SRV_DIMENSION_TEXTURE2DMS;
		else
			shaderResourceViewDesc.ViewDimension = m_renderTargetInfo.isCubemap ? D3D11_SRV_DIMENSION_TEXTURECUBE : D3D11_SRV_DIMENSION_TEXTURE2D;

		for (size_t i = 0; i < m_shaderResources.size(); i++)
		{
			shaderResourceViewDesc.Format = m_renderTargetInfo.attachments[i];

			if (FAILED(device->CreateShaderResourceView(textures[i], &shaderResourceViewDesc, &m_shaderResources[i])))
			{
				std::cerr << "Failed to create shader resource: " << i << " for Frame Buffer." << std::endl;
				m_isComplete = false;
				return;
			}
		}

		if (m_renderTargetInfo.createDepthBuffer)
		{
			D3D11_TEXTURE2D_DESC depthBufferDesc;
			ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

			// Set up the description of the depth buffer
			depthBufferDesc.Width = m_renderTargetInfo.width;
			depthBufferDesc.Height = m_renderTargetInfo.height;
			depthBufferDesc.MipLevels = 1;
			depthBufferDesc.ArraySize = 1;
			depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

			// Multisampling
			depthBufferDesc.SampleDesc.Count = MSAA;
			depthBufferDesc.SampleDesc.Quality = 0;
			depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			depthBufferDesc.CPUAccessFlags = 0;
			depthBufferDesc.MiscFlags = 0;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTexture;

			// Create the texture for the depth buffer using the filled out description
			if (FAILED(device->CreateTexture2D(&depthBufferDesc, NULL, &depthTexture)))
			{
				return;
			}

			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
			// Initialize the depth stencil view
			ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

			// Set up the depth stencil view description
			depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

			// Multisampling
			depthStencilViewDesc.ViewDimension = MSAA == 1 ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DMS;
			depthStencilViewDesc.Texture2D.MipSlice = 0;

			// Create the depth stencil view
			if (FAILED(device->CreateDepthStencilView(depthTexture.Get(), &depthStencilViewDesc, &m_depthBuffer)))
			{
				return;
			}
		}
	}
}

