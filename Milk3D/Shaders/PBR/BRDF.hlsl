#include "../Common.hlsli"
#include "IBL.hlsl"


cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

PixelInputType VS(VertexInputType input)
{
    PixelInputType output;
	input.position.w = 1.0f;
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
  
    output.tex = input.tex;
    
    return output;
}

float2 IntegrateBRDF(float NV, float roughness)
{
	float3 V = float3(sqrt(1.0f - NV * NV), 0.0f, NV);
	float3 N = float3(0, 0, 1);
	float A = 0;
	float B = 0;
	
	const uint sampleCount = 1024;
	for (uint i = 0; i < sampleCount; i++)
	{
		float2 Xi = Hammersley(i, sampleCount);
		float3 H = ImportanceSampleGGX(Xi, N, roughness);
		float3 L = normalize(2.0f * dot(V, H) * H - V);
		
		float NL = saturate(L.z);
		float NH = saturate(H.z);
		float VH = saturate(dot(V, H));
		
		if (NL > 0.0f)
		{
			float G = IBLGeometrySmith(N, V, L, roughness);
			float GVis = (G * VH) / (NH * NV);
			float Fc = pow(1.0f - VH, 5.0f);
			
			A += (1.0f - Fc) * GVis;
			B += Fc * GVis;
		}
	}
	
	return float2(A, B) / sampleCount;
}

float2 PS(PixelInputType input) : SV_TARGET
{
	// Flip the y-axis for the texture
	return IntegrateBRDF(input.tex.x, 1.0f - input.tex.y);
}
