#include "Common.hlsli"

Texture2D mainTexture : register(t0);

SamplerState samplerWrap : register(s0); 

cbuffer MatrixBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

cbuffer LightBuffer : register(b0)
{
	Light lights[MAX_LIGHTS];
	uint activeLights;
	float3 lightPadding;
};

cbuffer SceneBuffer : register(b1)
{
	float3 cameraPosition;
	float scenePadding;
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
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD2;
};

PixelInput VS(VertexInput input)
{
	PixelInput output;
	output.position = mul(input.position, world);
	output.worldPos = output.position.xyz;
	output.position = mul(output.position, mul(view, projection));
	
	// World space lighting
	output.normal = mul(input.normal, (float3x3)world);
	output.texCoord = input.texCoord;
	
	output.viewDirection = normalize(cameraPosition - output.worldPos);
	
	return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
	float4 textureColor = mainTexture.Sample(samplerWrap, input.texCoord);
	
	float3 outColor = 0.0f;
	
	for (uint i = 0u; i < activeLights; i++)
	{
		if (!lights[i].enabled) 
			continue;
		
		float3 lightDir = lights[i].direction;
		if (lights[i].type == POINT)
			lightDir = normalize(input.worldPos - lights[i].position);
		
		// Diffuse
		float NL = saturate(dot(-lightDir, input.normal));
		float3 diffuse = textureColor.xyz * lights[i].color * NL;
		
		// Specular
		float3 reflection = reflect(lightDir, input.normal);
		const float specularPower = 5.0f;
		float3 specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
         
        float d = length(lightDir);
         
		 #if 0
        // Falloff
        bool flags = FALLOFF_ENABLED(lights[i].flags);
        bool customFalloff = CUSTOM_FALLOFF(lights[i].flags);
        if (customFalloff)
            d = lights[i].falloff;
        
        if (flags)
            outColor *= ((d*d) / dot(lights[i].lightPosition.xyz - input.position.xyz, lights[i].lightPosition.xyz - input.position.xyz));
        
		// Attenuation
        float atten = (lights[i].attenuation.x + (lights[i].attenuation.y * d)) + (lights[i].attenuation.z * (d*d));
        if (atten != 0.0f)
            outColor /= atten;
		#endif
        
		// Spotlight
		const float spotAngle = 0.1f;
        if (lights[i].type == SPOT)
            outColor *= pow(max(dot(lightDir, lights[i].direction), 0.0f), spotAngle);
			
			
		// Result color
		outColor += saturate(diffuse + specular);
	}
	
	return saturate(float4(outColor, 1.0f));
}
