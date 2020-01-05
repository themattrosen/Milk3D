#pragma once

#include "Graphics/Core/Texture.h"
#include "Graphics/Core/Sampler.h"
#include "Graphics/Core/GraphicsDevice.h"

/*
/ Albedo, used for reflectivity in our case
Texture2D gAlbedo : register(t0);

// Metallic, used for metal properties of the object
Texture2D gMetallic : register(t1);

// Roughness, map of the surface's rough texture
Texture2D gRoughness : register(t2);

// Ambient Occlusion, specifies small lighting and shadow details
Texture2D gAO : register(t3);

*/

namespace Milk3D
{
	struct Material
	{
	public:
		Material() = default;
		Material(Material const & other) { *this = other; }
		Material & operator=(Material const & other)
		{
			albedo = other.albedo;
			metallic = other.metallic;
			roughness = other.roughness;
			ambientOcclusion = other.ambientOcclusion;
			return *this;
		}
		void Use(UINT textureSlotStart = 0, UINT samplerSlotStart = 0) const
		{
			// TODO: Much faster to set this as one array
			auto deviceContext = GraphicsDevice::GetInstance().GetDeviceContext();
			if (albedo)
			{
				ID3D11ShaderResourceView * albedoView = albedo->GetShaderResource();
				deviceContext->PSSetShaderResources(textureSlotStart, 1, &albedoView);
			}
			if (metallic)
			{
				ID3D11ShaderResourceView * metallicView = metallic->GetShaderResource();
				deviceContext->PSSetShaderResources(textureSlotStart + 1, 1, &metallicView);
			}

			if (roughness)
			{
				ID3D11ShaderResourceView * roughnessView = roughness->GetShaderResource();
				deviceContext->PSSetShaderResources(textureSlotStart + 2, 1, &roughnessView);
			}

			if (ambientOcclusion)
			{
				ID3D11ShaderResourceView * aoView = ambientOcclusion->GetShaderResource();
				deviceContext->PSSetShaderResources(textureSlotStart + 3, 1, &aoView);
			}

			if (!samplers.empty())
			{
				UINT samplerCount = static_cast<UINT>(samplers.size());
				deviceContext->PSSetSamplers(samplerSlotStart, samplerCount, samplers.data());
			}
		}
		Texture * albedo = nullptr;
		Texture * metallic = nullptr;
		Texture * roughness = nullptr;
		Texture * ambientOcclusion = nullptr;
		std::vector<ID3D11SamplerState*> samplers;
	};
}