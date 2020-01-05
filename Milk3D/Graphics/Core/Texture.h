#pragma once

#include "Graphics/Headers/GraphicsCommon.h"
#include "Graphics/Headers/DX11.h"

namespace Milk3D
{

	class Texture
	{
		public:
			Texture() = default;
			virtual void Initialize(const char * filename, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, UINT mipLevel = 1);
			virtual ~Texture();
			void Release();

			DELETE_COPY(Texture);

			//*** Getters ***//
			ID3D11ShaderResourceView * const GetShaderResource() const { return m_shaderResource.Get(); }
			int Width() const { return m_width; }
			int Height() const { return m_height; }
			int Channels() const { return m_colorChannels; }

		protected:
			int GetNumChannels(DXGI_FORMAT format);
			void LoadDDS(const char * filename);
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResource;
			int m_width = 0;
			int m_height = 0;
			int m_colorChannels = 0;
	};
	
	//------------------------------------------------------------------------------
	// Public Functions:
	//------------------------------------------------------------------------------


} // Namespace: DX11

//------------------------------------------------------------------------------
// Other:
//------------------------------------------------------------------------------

