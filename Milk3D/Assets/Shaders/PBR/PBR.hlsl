#include "../Common.hlsli"
#include "IBL.hlsl"

SamplerState samplerWrap : register(s0);
Texture2D normalMap : register(t0);

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
	float3 normal : NORMAL;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

PixelInputType VS(VertexInputType input)
{
    PixelInputType output;
    output.position = mul(input.position, worldMatrix);
    output.worldPos = output.position;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
  
    output.tex = input.tex;
	
	
    float3 normal = normalize(mul(input.normal, (float3x3)worldMatrix));
    float3 tangent = normalize(mul(input.tangent, (float3x3)worldMatrix));
    float3 binormal = normalize(mul(input.binormal, (float3x3)worldMatrix));
	
	// Bump map
	float2 bumpMap = normalMap.Sample(SampleType, input.tex).xy;
	
	// Expand the range of the normal value from (0, +1) to (-1, +1).
	bumpMap = (bumpMap * 2.0f) - 1.0f;
	
	// Calculate the normal from the data in the bump map
	output.normal = normalize(normal + bumpMap.x * tangent + bumpMap.y * binormal);
    
    return output;
}


//*** PIXEL SHADER ***//

// Albedo, used for reflectivity in our case
Texture2D gAlbedo : register(t0);

// Metallic, used for metal properties of the object
Texture2D gMetallic : register(t1);

// Roughness, map of the surface's rough texture
Texture2D gRoughness : register(t2);

// Ambient Occlusion, specifies small lighting and shadow details
Texture2D gAO : register(t3);

// Skybox texture, used for reflecting
TextureCube irradianceMap : register(t4);

// SampleType wrap UV coordinates around
SamplerState SampleTypeWrap : register(s0);

// SampleType clamp the UV coordinates at the edge
SamplerState SampleTypeClamp : register(s1);

// SampleType with custom comparison function for skybox
SamplerState SkyboxSampler : register(s2);

// Lights in the scene
cbuffer LightBuffer : register(b0)
{
	Light lights[MAX_LIGHTS];
	uint activeLights;
};

// Scene specific information
cbuffer SceneBuffer : register(b1)
{
	//! Camera's position in the scene, in world space
    float3 cameraPosition;
	float padding;
};


float3 FresnelSchlick(float lightIntensity, float3 surfaceReflection)
{
	return surfaceReflection + (1.0f - surfaceReflection) * pow(1.0f - lightIntensity, 5.0f);
}

float3 FresnelSchlickRoughness(float lightIntensity, float3 surfaceReflection, float roughness)
{
	return surfaceReflection + (max(float3(1.0f - roughness, 1.0f - roughness, 1.0f - roughness), 
		   surfaceReflection) - surfaceReflection) * pow(1.0f - lightIntensity, 5.0f);
}

float3 CalculateAttenuation(float3 L, float3 color, float3 position, int light)
{
	float d = length(lights[light].lightPosition - position);
			
	#if 0
	// Falloff
	bool falloffEnabled = FALLOFF_ENABLED(lights[light].flags);
	bool customFalloff = CUSTOM_FALLOFF(lights[light].flags);
	if (customFalloff)
		d = lights[light].falloff;
	
	if (falloffEnabled)
	{
		float3 pos = lights[light].lightPosition.xyz - position;
		color *= ((d*d) / dot(pos, pos));
	}
	
	float atten = (lights[light].attenuation.x + (lights[light].attenuation.y * d)) + (lights[light].attenuation.z * (d*d));
	if (atten != 0.0f)
		color /= atten;
	#endif
	
	color *= (1.0f / (d*d));
		
	return color;
}

float4 PS(PixelInputType input) : SV_TARGET
{
    float4 position = input.position;
	
	// Find current normal
    float3 normal = input.normal;
	
	// Sample all textures for material components
    //float3 diffuse = gDiffuse.Sample(SampleTypeWrap, input.tex).rgb;
    float3 albedo   = GammaCorrection(gAlbedo.Sample(SampleTypeWrap, input.tex).rgb);
    // float3 spec  = gSpecular.Sample(SampleTypeWrap, input.tex).rgb;
    float metallic  = gMetallic.Sample(SampleTypeWrap, input.tex).r;
    float roughness = gRoughness.Sample(SampleTypeWrap, input.tex).r;
    float AO        = gAO.Sample(SampleTypeWrap, input.tex).r;
	
	/*********** Lighting ***********/
	
	// TODO: Make less arbitrary
	float3 surfaceReflection = float3(0.04f, 0.04f, 0.04f);
	surfaceReflection = lerp(surfaceReflection, albedo, metallic);
	
	float3 view = normalize(cameraPosition - position.xyz);
	
	float3 Lo = float3(0.0f, 0.0f, 0.0f);
	
	for (int i = 0; i < activeLights; i++)
	{
		// Radiance
		float3 L = -normalize(lights[i].direction);
        
		// Point/Spot light
		if (lights[i].lightType != DIRECTIONAL)
		{
			L = -normalize(position.xyz - lights[i].lightPosition.xyz);
		}
		
		float3 radiance = CalculateAttenuation(L, lights[i].diffuseColor.rgb, position.xyz, i);
		
		float3 H = normalize(view + L);
		
		// BRDF
		float NDF = DistributionGGX(normal, H, roughness);
		float G = GeometrySmith(normal, view, L, roughness);
		float3 F = FresnelSchlick(max(dot(H, view), 0.0f), surfaceReflection);
		
		float3 KS = F;
		float3 KD = 1.0f - KS;
		KD *= 1.0f - metallic;
		
		float lightIntensity = max(dot(normal, L), 0.0f);
		
		float3 num = NDF * G * F;
		float3 denom = (4.0f * max(dot(normal, view), 0.0f) * lightIntensity);
		float3 specular = num / max(denom, 0.001f);
		
		Lo += (KD * albedo / PI + specular) * radiance * lightIntensity;
	}
	
	float3 KS = FresnelSchlickRoughness(max(dot(normal, view), 0.0f), surfaceReflection, roughness);
	float3 KD = 1.0f - KS;
	KD *= (1.0f - metallic);
	
	float3 irradiance = irradianceMap.Sample(SampleTypeWrap, normal).rgb;
	
	float3 diffuse = irradiance * albedo;
	float3 ambient = (KD * diffuse) * AO;
	
	float3 color = ambient + Lo;
	
	// HDR tonemapping
	color = HDRTonemap(color):

	// Apply gamma correction to the current pixel otherwise lit pixels might appear oversaturated or too dark
	return GammaCorrection(float4(color, 1.0f));
}
