#include "../Common.hlsli"

TextureCube environmentMap : register(t0);
SamplerState SampleType : register(s0);

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
	
	output.tex = input.position.xyz;
	
	// Remove the translation portion from the viewing matrix
	matrix rotView = viewMatrix;
	rotView[0].xyz = 0.0f;
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, rotView);
    output.position = mul(output.position, projectionMatrix);
	output.position = output.position.xyww;
	
    return output;
}

float4 PS(PixelInputType input) : SV_TARGET
{
	float3 color = HDRTonemap(environmentMap.SampleLevel(SampleType, input.tex, 0.0f).rgb);
	return float4(GammaCorrection(color), 1.0f);
}
