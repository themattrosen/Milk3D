#pragma once

#include "Graphics/Headers/DX11.h"
#include <vector>

namespace Milk3D
{
	enum AddressMode
	{
		Wrap = D3D11_TEXTURE_ADDRESS_WRAP,
		Clamp = D3D11_TEXTURE_ADDRESS_CLAMP,
		Mirror = D3D11_TEXTURE_ADDRESS_MIRROR,
		Border = D3D11_TEXTURE_ADDRESS_BORDER,
		MirrorOnce = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE,
		DefaultAddressMode = Wrap
	};
	enum ComparisonFunction
	{
		Never = D3D11_COMPARISON_NEVER,
		Less = D3D11_COMPARISON_LESS,
		Equal = D3D11_COMPARISON_EQUAL,
		LessEqual = D3D11_COMPARISON_LESS_EQUAL,
		Greater = D3D11_COMPARISON_GREATER,
		GreaterEqual = D3D11_COMPARISON_GREATER_EQUAL,
		NotEqual = D3D11_COMPARISON_NOT_EQUAL,
		Always = D3D11_COMPARISON_ALWAYS,
		DefaultComparison = Always
	};

	enum Filter
	{
		MinMagMipPoint = D3D11_FILTER_MIN_MAG_MIP_POINT,
		MinMagPointMipLinear = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,
		MinPointMagLinearMipPoint = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
		MinPointMagMipLinear = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR,
		MinLinearMagMipPoint = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT,
		MinLinearMagPointMipLinear = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		MinMagLinearMipPoint = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
		MinMagMipLinear = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		Anisotropic = D3D11_FILTER_ANISOTROPIC,
		ComparisonMinMagMipPoint = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
		ComparisonMinMagPointMipLinear = D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR,
		ComparisonMinPointMagLinearMipPoint = D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT,
		ComparisonMinPointMagMipLinear = D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR,
		ComparisonMinLinearMagMipPoint = D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT,
		ComparisonMinLinearMagPointMipLinear = D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		ComparisonMinMagLinearMipPoint = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
		ComparisonMinMagMipLinear = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
		ComparisonAnisotropic = D3D11_FILTER_COMPARISON_ANISOTROPIC,
		MinimumMinMagMipPoint = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT,
		MinimumMinMagPointMipLinear = D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR,
		MinimumMinPointMagLinearMipPoint = D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
		MinimumMinPointMagMipLinear = D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR,
		MinimumMinLinearMagMipPoint = D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT,
		MinimumMinLinearMagPointMipLinear = D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		MinimumMinMagLinearMipPoint = D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT,
		MinimumMinMagMipLinear = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR,
		MinimumAnisotropic = D3D11_FILTER_MINIMUM_ANISOTROPIC,
		MaximumMinMagMipPoint = D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT,
		MaximumMinMagPointMipLinear = D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR,
		MaximumMinPointMagLinearMipPoint = D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
		MaximumMinPointMagMipLinear = D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR,
		MaximumMinLinearMagMipPoint = D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT,
		MaximumMinLinearMagPointMipLinear = D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		MaximumMinMagLinearMipPoint = D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT,
		MaximumMinMagMipLinear = D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR,
		DefaultFilter = MinMagMipLinear
	};

	class Sampler
	{
	public:
		static ID3D11SamplerState * const GetSamplerWrap() { return DefaultSamplers::GetInstance().samplerWrap.Get(); }
		static ID3D11SamplerState * const GetSamplerClamp() { return DefaultSamplers::GetInstance().samplerClamp.Get(); }
		static ID3D11SamplerState * const CreateSampler(D3D11_SAMPLER_DESC const & samplerDesc);
		static ID3D11SamplerState * const CreateSampler(Filter filter, AddressMode addressMode, ComparisonFunction comparisonFunction);

	private:

		struct DefaultSamplers
		{
			static DefaultSamplers & GetInstance() { static DefaultSamplers instance; return instance; }
			DefaultSamplers();
			Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerWrap;
			Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerClamp;
		};
		static std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;
	};
}