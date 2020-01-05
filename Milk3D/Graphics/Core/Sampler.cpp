#pragma once

#include "Sampler.h"
#include "GraphicsDevice.h"
#include <iostream>

namespace Milk3D
{

	std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>> Sampler::samplers;

	Sampler::DefaultSamplers::DefaultSamplers() :
		samplerWrap(CreateSampler(DefaultFilter, Wrap, Always)),
		samplerClamp(CreateSampler(DefaultFilter, Clamp, Always))
	{
	}

	ID3D11SamplerState * const Sampler::CreateSampler(D3D11_SAMPLER_DESC const & samplerDesc)
	{
		ID3D11SamplerState * samplerPtr = nullptr;
		auto desc = reinterpret_cast<D3D11_SAMPLER_DESC const*>(&samplerDesc);
		if (FAILED(GraphicsDevice::GetDevice()->CreateSamplerState(desc, &samplerPtr)))
		{
			std::cout << "Failed to create Sampler." << std::endl;
			return nullptr;
		}
		samplers.push_back(samplerPtr);
		return samplerPtr;
	}
	ID3D11SamplerState * const Sampler::CreateSampler(Filter filter, AddressMode addressMode, ComparisonFunction comparisonFunction)
	{
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = static_cast<decltype(samplerDesc.Filter)>(filter);
		samplerDesc.AddressU = static_cast<decltype(samplerDesc.AddressU)>(addressMode);
		samplerDesc.AddressV = static_cast<decltype(samplerDesc.AddressV)>(addressMode);
		samplerDesc.AddressW = static_cast<decltype(samplerDesc.AddressW)>(addressMode);
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = static_cast<decltype(samplerDesc.ComparisonFunc)>(comparisonFunction);
		samplerDesc.BorderColor[0] = 0.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 0.0f;
		samplerDesc.BorderColor[3] = 0.0f;
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		return CreateSampler(samplerDesc);
	}

}