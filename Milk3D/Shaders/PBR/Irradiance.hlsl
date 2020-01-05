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
    output.position = mul(input.position, worldMatrix);
	output.tex = output.position.xyz;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    return output;
}

float4 PS(PixelInputType input) : SV_TARGET
{
	float3 normal = normalize(input.tex);
    float3 irradiance = 0;   
	
    float3 up = vec3(0.0f, 1.0f, 0.0f);
    float3 right = cross(up, normal);
    up = cross(normal, right);
       
    const float sampleDelta = 0.025f;
	const float twoPI = 2.0f * PI;
	const float halfPi = 0.5f * PI;
	
	float samples = 0.0f;
	
    for(float phi = 0.0f; phi < twoPI; phi += sampleDelta)
    {
        for(float theta = 0.0f; theta < halfPi; theta += sampleDelta)
        {
            // Spherical to cartesian (in tangent space)
            float3 tangentSample = float3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
			
            // Tangent space to world
            float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

            irradiance += environmentMap.Sample(SampleType, sampleVec).rgb * cos(theta) * sin(theta);
            samples++;
        }
    }
    irradiance = PI * irradiance * (1.0f / samples);
    
	return (float4(irradiance, 1.0f));
}