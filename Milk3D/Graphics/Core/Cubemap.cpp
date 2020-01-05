/*!------------------------------------------------------------------------------
//
 *****
 \file   Cubemap.cpp
 \author Christopher Taylor
 \par    Project: DX11
 \par    C++ Source File
 *****
//------------------------------------------------------------------------------
*/

//------------------------------------------------------------------------------
// Includes/Defines:
//------------------------------------------------------------------------------

#include "Cubemap.h"
#include "GraphicsDevice.h"
#include "Graphics/Headers/GraphicsCommon.h"
#include "Graphics/Headers/STB.h"

#include <DirectXTK/DDSTextureLoader.h>
#include <iostream>

namespace Milk3D
{
	void Cubemap::Initialize(const char * filename, DXGI_FORMAT, UINT mipLevel)
	{
		auto device = GraphicsDevice::GetDevice();
		auto deviceContext = GraphicsDevice::GetDeviceContext();

		std::wstring wStr = ToWideString(filename);
		if (FAILED((DirectX::CreateDDSTextureFromFileEx(device, deviceContext, wStr.data(), 0,
			D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, false, nullptr, &m_shaderResource))))
		{
			std::cout << "Failed to load Cubemap Texture with file: " << filename << std::endl;
			return;
		}

		// Get texture information
		Microsoft::WRL::ComPtr<ID3D11Resource> res;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		m_shaderResource->GetResource(&res);
		res->QueryInterface<ID3D11Texture2D>(&texture);

		D3D11_TEXTURE2D_DESC textureDesc = { 0 };
		texture->GetDesc(&textureDesc);
		m_height = textureDesc.Height;
		m_width = textureDesc.Width;
		m_colorChannels = GetNumChannels(textureDesc.Format);

		// Generate mipmaps for this texture
		if (textureDesc.MipLevels > 1 || mipLevel > 1)
			deviceContext->GenerateMips(m_shaderResource.Get());
	}
	void Cubemap::Initialize(std::array<const char *, 6> const & filenames, DXGI_FORMAT format, UINT mipLevel)
	{
		auto device = GraphicsDevice::GetDevice();
		auto deviceContext = GraphicsDevice::GetDeviceContext();

		// Load the image data into memory
		unsigned char * data[6] = { nullptr };

		// Load all, uniform width and height
		data[0] = stbi_load(filenames[0], &m_width, &m_height, &m_colorChannels, STBI_rgb_alpha);
		data[1] = stbi_load(filenames[1], &m_width, &m_height, &m_colorChannels, STBI_rgb_alpha);
		data[2] = stbi_load(filenames[2], &m_width, &m_height, &m_colorChannels, STBI_rgb_alpha);
		data[3] = stbi_load(filenames[3], &m_width, &m_height, &m_colorChannels, STBI_rgb_alpha);
		data[4] = stbi_load(filenames[4], &m_width, &m_height, &m_colorChannels, STBI_rgb_alpha);
		data[5] = stbi_load(filenames[5], &m_width, &m_height, &m_colorChannels, STBI_rgb_alpha);

		// Setup the description of the texture
		D3D11_TEXTURE2D_DESC textureDesc = { };
		textureDesc.Width = m_width;
		textureDesc.Height = m_height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 6;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		// Create the empty texture
		D3D11_SUBRESOURCE_DATA pData[6];

		// Set the row pitch of the targa image data
		unsigned rowPitch = (m_width * 4) * sizeof(unsigned char);
		for (int i = 0; i < 6; i++)
		{
			if (!data[i])
			{
				std::cout << "Failed to load Cubemap Texture with filename: " << filenames[i] << std::endl;
				for (int j = 0; j < i; j++)
					stbi_image_free(data[j]);
				return;
			}
			pData[i].pSysMem = data[i];
			pData[i].SysMemPitch = rowPitch;
			pData[i].SysMemSlicePitch = 0;
		}

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		if (FAILED(device->CreateTexture2D(&textureDesc, &pData[0], &texture)))
		{
			std::cout << "Failed to create Cubemap Texture. Make sure you provided 6 different textures." << std::endl;
			return;
		}

		// Setup the shader resource view description
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = { };
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;

		// Create the shader resource view for the texture
		if (FAILED(device->CreateShaderResourceView(texture.Get(), &srvDesc, &m_shaderResource)))
		{
			std::cout << "Failed to create shader resource for cubemap texture.\n";
			return;
		}

		// Generate mipmaps for this texture
		if (mipLevel != 1)
			deviceContext->GenerateMips(m_shaderResource.Get());

		// Release the image data now that the image data has been loaded into the texture
		for (int i = 0; i < 6; i++)
			stbi_image_free(data[i]);
	}

	//------------------------------------------------------------------------------
	// Private:
	//------------------------------------------------------------------------------
	

} // Namespace: DX11

//------------------------------------------------------------------------------
// Other:
//------------------------------------------------------------------------------
