#pragma once

#include "Graphics/Headers/GraphicsCommon.h"
#include "Graphics/Headers/DX11.h"
#include <vector>

namespace Milk3D
{

	struct RenderTargetInfo
	{
		RenderTargetInfo() = default;
		RenderTargetInfo(std::vector<DXGI_FORMAT> const & attachments, UINT width, UINT height, bool resizable = false, bool isCubemap = false) : 
			attachments(attachments), width(width), height(height), resizable(resizable), isCubemap(isCubemap)
		{
		}
		RenderTargetInfo(UINT numAttachments, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT, bool resizable = false, bool isCubemap = false) :
			attachments(numAttachments, format), width(width), height(height), resizable(resizable), isCubemap(isCubemap)
		{
		}
		UINT width = 0;
		UINT height = 0;
		bool createDepthBuffer = true;
		bool resizeWithWindow = false;
		bool isCubemap = false;
		bool resizable = false;
		std::vector<DXGI_FORMAT> attachments;
	};

	class RenderTarget
	{
		public:
			RenderTarget() = default;
			void Initialize(RenderTargetInfo const & info);
			~RenderTarget();
			void Release();

			DELETE_COPY(RenderTarget);

			//*** General Functions ***//

			void Use(float depth = 1.0f, UINT8 stencil = 0);
			void Use(ID3D11DepthStencilView * depthStencil, bool clearDepth = true, float depth = 1.0f, UINT8 stencil = 0);
			void Disable();

			void SetClearColor(float color[4]);
			void SetClearColor(float r, float g, float b, float a);

			void Resize(UINT width, UINT height);

			//*** Getters ***//

			void GetClearColor(float(&color)[4]);

			UINT TextureCount() const { return m_textureCount; }

			std::vector<ID3D11ShaderResourceView*> const & GetShaderResources() const
			{
				return m_shaderResources;
			}

			std::vector<ID3D11RenderTargetView*> const & GetRenderTargets() const
			{
				return m_renderTargets;
			}

			ID3D11DepthStencilView * const getDepthBuffer() { return m_depthBuffer.Get(); }
	private:

		UINT m_textureCount = 0;

		// These are not ComPtrs to make it easily sent to the GPU as an array
		std::vector<ID3D11RenderTargetView*> m_renderTargets;
		std::vector<ID3D11ShaderResourceView*> m_shaderResources;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthBuffer;
		D3D11_VIEWPORT m_viewport;
		float m_clearColor[4] = { 0,0,0,1 };

		bool m_isComplete = true;
		RenderTargetInfo m_renderTargetInfo;

		void Setup();
	};
}

