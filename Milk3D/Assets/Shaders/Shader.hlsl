
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
	float3 normal : NORMAL;
};

PixelInput VS(VertexInput input)
{
	PixelInput output;
	output.position = input.position;
	output.normal = input.normal;
	return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
	return float4(1,0,0,1);
	//return float4(1,1,1,1) * saturate(dot(input.normal, float3(0,0,-1)));
}

