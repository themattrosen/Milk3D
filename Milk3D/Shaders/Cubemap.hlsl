TextureCube mainTexture : register(t0);
SamplerState samplerWrap : register(s0); 

cbuffer MatrixBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

struct VertexInput
{
	float4 position : POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 worldPos : TEXCOORD1;
};

PixelInput VS(VertexInput input)
{
	PixelInput output;
	output.position = mul(input.position, world);
	output.worldPos = output.position.xyz;
	output.position = mul(output.position, mul(view, projection));
	output.texCoord = input.texCoord;

	return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
	float4 textureColor = mainTexture.Sample(samplerWrap, input.worldPos);
	return textureColor;
}
