#include "../Common.hlsli"
#include "IBL.hlsl"

TextureCube environmentMap : register(t0);
SamplerState SampleType : register(s0);

// Buffer for the current object's material
cbuffer MaterialBuffer : register(b5)
{
	uint inUse;
	float F0;
	// Current LOD level
	float roughness;
	// In the case of THIS shader, the padding is used for viewport resolution
	float resolution;
};

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 tex : TEXCOORD0;
};

PixelInputType VS(VertexInputType input)
{
    PixelInputType output;
    
    input.position.w = 1.0f;
    output.position = mul(input.position, worldMatrix);
	output.tex = output.position.xyz;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    return output;
}

float4 PS(PixelInputType input) : SV_TARGET
{
	float3 normal = normalize(input.tex);
	return environmentMap.Sample(SampleType, normal);
	
	float3 R = normal;
	float3 V = R;
	
	const uint sampleCount = 1024;
	
	float3 prefilteredColor = 0;
	float totalWeight = 0.0f;
	
	for (uint i = 0; i < sampleCount; i++)
	{
		float2 Xi = Hammersley(i, sampleCount);
		float3 H = ImportanceSampleGGX(Xi, normal, roughness);
		float3 L = (2.0f * dot(V, H) * H - V);
		
		float lightIntensity = saturate(dot(normal, L));
		if (lightIntensity > 0.0f)
		{
			#if IBL_USE_MIPMAPS
				float D = DistributionGGX(normal, H, roughness);
				float NH = max(dot(normal, H), 0.0f);
				float HV = max(dot(H, V), 0.0f);
				float pdf = D * NH / (4.0f * HV) + 0.0001f;
				float saTexel = 4.0f * PI / (6.0f * resolution * resolution);
				float saSample = 1.0f / (float(sampleCount) * pdf + 0.0001f);
				
				float mipLevel = roughness == 0.0f ? 0.0f : 0.5f * log2(saSample / saTexel);
				
				prefilteredColor += environmentMap.SampleLevel(SampleType, L, mipLevel).rgb * lightIntensity;
				totalWeight += lightIntensity;
			#else
				prefilteredColor += environmentMap.SampleLevel(SampleType, L, 0).rgb * lightIntensity;
				totalWeight += lightIntensity;
			#endif
			
		}
	}
	
	return float4(prefilteredColor / totalWeight, 1.0f);
}
