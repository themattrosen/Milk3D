#pragma comment(lib, "DirectXTK.lib")

#include "Texture.h"
#include "Graphics/Core/GraphicsDevice.h"
#ifndef STB_DEFINE
#define STB_DEFINE     
#define STB_IMAGE_IMPLEMENTATION
#endif

#include "Graphics/Headers/STB.h"

// DDS texture loading
#include <DirectXTK/DDSTextureLoader.h>
#include <iostream>
#include <string>

namespace Milk3D
{
	void Texture::Initialize(const char * filename, DXGI_FORMAT format, UINT mipLevel)
	{
		std::string file(filename);
		ToLower(file);
		if (file.find("dds") != std::string::npos)
		{
			LoadDDS(filename);
			return;
		}
		
		auto device = GraphicsDevice::GetDevice();
		auto deviceContext = GraphicsDevice::GetDeviceContext();

		unsigned char * data = stbi_load(filename, &m_width, &m_height, &m_colorChannels, STBI_rgb_alpha);

		if (!data)
		{
			std::cout << "Failed to load texture with filename: " << filename << std::endl;
			return;
		}
		
		D3D11_TEXTURE2D_DESC textureDesc = { 0 };

		// Setup the description of the texture
		textureDesc.Height = m_height;
		textureDesc.Width = m_width;
		textureDesc.MipLevels = mipLevel;
		textureDesc.ArraySize = 1;
		textureDesc.Format = format;

		// Setup multisampling qualities
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = mipLevel != 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

		D3D11_SUBRESOURCE_DATA subresourceData = { 0 };
		subresourceData.pSysMem = data;
		subresourceData.SysMemPitch = (m_width * 4) * sizeof(unsigned char);
		subresourceData.SysMemSlicePitch = 0;

		ID3D11Texture2D * texture = nullptr;

		// Create the texture
		if (FAILED(device->CreateTexture2D(&textureDesc, &subresourceData, &texture)))
		{
			std::cout << "Failed to create texture with filename: " << filename << std::endl;
			return;
		}

		stbi_image_free(data);
		data = nullptr;

		// Setup the shader resource view description
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = { };
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		// Create the shader resource view for the texture
		if (FAILED(device->CreateShaderResourceView(texture, &srvDesc, &m_shaderResource)))
		{
			std::cerr << "Failed to create shader resource for Texture with given pixel data." << std::endl;
			return;
		}

		// Generate mipmaps for this texture
		if (mipLevel != 1)
		{
			deviceContext->GenerateMips(m_shaderResource.Get());
		}
		SAFE_RELEASE(texture);
	}

	Texture::~Texture()
	{
		Release();
	}

	void Texture::Release()
	{
		m_shaderResource.Reset();
	}
	
	int Texture::GetNumChannels(DXGI_FORMAT format)
	{
		switch (format)
		{
			default:
			case DXGI_FORMAT_R32G32B32A32_UINT: 
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
			case DXGI_FORMAT_R32G32B32A32_SINT:
			case DXGI_FORMAT_R32G32B32A32_TYPELESS:
			case DXGI_FORMAT_R16G16B16A16_UINT:
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
			case DXGI_FORMAT_R16G16B16A16_SINT:
			case DXGI_FORMAT_R16G16B16A16_TYPELESS:
			case DXGI_FORMAT_R8G8B8A8_UINT:
			case DXGI_FORMAT_R8G8B8A8_SINT:
			case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return 4;

			case DXGI_FORMAT_R32G32B32_UINT:
			case DXGI_FORMAT_R32G32B32_FLOAT:
			case DXGI_FORMAT_R32G32B32_SINT:
			case DXGI_FORMAT_R32G32B32_TYPELESS:
			return 3;

			case DXGI_FORMAT_R32G32_UINT:
			case DXGI_FORMAT_R32G32_FLOAT:
			case DXGI_FORMAT_R32G32_SINT:
			case DXGI_FORMAT_R32G32_TYPELESS:
			case DXGI_FORMAT_R16G16_UINT:
			case DXGI_FORMAT_R16G16_FLOAT:
			case DXGI_FORMAT_R16G16_SINT:
			case DXGI_FORMAT_R16G16_TYPELESS:
			case DXGI_FORMAT_R8G8_UINT:
			case DXGI_FORMAT_R8G8_SINT:
			case DXGI_FORMAT_R8G8_TYPELESS:
			return 2;

			case DXGI_FORMAT_R32_UINT:
			case DXGI_FORMAT_R32_FLOAT:
			case DXGI_FORMAT_R32_SINT:
			case DXGI_FORMAT_R32_TYPELESS:
			case DXGI_FORMAT_R16_UINT:
			case DXGI_FORMAT_R16_FLOAT:
			case DXGI_FORMAT_R16_SINT:
			case DXGI_FORMAT_R16_TYPELESS:
			case DXGI_FORMAT_R8_UINT:
			case DXGI_FORMAT_R8_SINT:
			case DXGI_FORMAT_R8_TYPELESS:
			return 1;
		}
		return 4; // RGBA
	}
	void Texture::LoadDDS(const char * filename)
	{
		auto device = GraphicsDevice::GetDevice();
		auto deviceContext = GraphicsDevice::GetDeviceContext();

		// Load DDS file
		if (FAILED(DirectX::CreateDDSTextureFromFile(device, ToWideString(filename).data(), 0, &m_shaderResource)))
		{
			std::cerr << "Failed to load DDS Texture with file: " << filename << std::endl;
			return;
		}

		// Get texture information
		ComPtr<ID3D11Texture2D> texInterface;
		ComPtr<ID3D11Resource> res;
		m_shaderResource->GetResource(&res);
		res->QueryInterface<ID3D11Texture2D>(&texInterface);
		D3D11_TEXTURE2D_DESC textureDesc = { 0 };
		texInterface->GetDesc(&textureDesc);
		m_height = textureDesc.Height;
		m_width = textureDesc.Width;
		m_colorChannels = GetNumChannels(textureDesc.Format);

		// Generate mipmaps for this texture
		if (textureDesc.MipLevels > 0)
			deviceContext->GenerateMips(m_shaderResource.Get());
	}
}
