#ifndef __IBL_HLSL__
#define __IBL_HLSL__

#include "../Common.hlsli"

#define IBL_USE_MIPMAPS 0
#if IBL_USE_MIPMAPS
	#define MAX_REFLECTION_LOD 5.0f
#else
	#define MAX_REFLECTION_LOD 1.0f
#endif

float DistributionGGX(float3 normal, float3 H, float roughness)
{
	float a = roughness * roughness;
	float num = a * a;
	float NH = max(dot(normal, H), 0.0f);
	float NH2 = NH * NH;
	
	float denom = (NH2 * (num - 1.0f) + 1.0f);
	denom = PI * denom * denom;
	
	return num / denom;
}

float GeometrySchlickGGX(float NV, float roughness)
{
	float r = (roughness + 1.0f);
	float k = (r * r) / 8.0f;
	float denom = NV * (1.0f - k) + k;
	return NV / denom;
}

float IBLGeometrySchlickGGX(float NV, float roughness)
{
	float a = roughness;
    float k = (a * a) / 2.0f;
    float denom = NV * (1.0f - k) + k;

    return NV / denom;
}

float GeometrySmith(float3 normal, float3 view, float3 L, float roughness)
{
	float NV = max(dot(normal, view), 0.0f);
	float NL = max(dot(normal, L), 0.0f);
	float ggxv = GeometrySchlickGGX(NV, roughness);
	float ggxl = GeometrySchlickGGX(NL, roughness);
	return ggxv * ggxl;
}

float IBLGeometrySmith(float3 normal, float3 view, float3 L, float roughness)
{
	float NV = max(dot(normal, view), 0.0f);
	float NL = max(dot(normal, L), 0.0f);
	float ggxv = IBLGeometrySchlickGGX(NV, roughness);
	float ggxl = IBLGeometrySchlickGGX(NL, roughness);
	return ggxv * ggxl;
}

// Credit to: http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// Efficient VanDerCorpus calculation.
float RadicalInverseVDC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // 0x100000000
}

float2 Hammersley(uint i, uint N)
{
	return float2(float(i)/float(N), RadicalInverseVDC(i));
}

float3 ImportanceSampleGGX(float2 Xi, float3 normal, float roughness)
{
	float a = roughness * roughness;
	float phi = 2.0f * PI * Xi.x;
	float cosTheta = sqrt((1.0f - Xi.y) / (1.0f + (a*a - 1.0f) * Xi.y));
	float sinTheta = sqrt(1.0f - cosTheta*cosTheta);
	
	// Spherical coordinates -> cartesian coordinates
	float3 H = 0;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;
	
	// Tangent space -> world space
	float3 up = abs(normal.z) < 0.999f ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f);
	float3 tangent = normalize(cross(up, normal));
	float3 bitangent = cross(normal, tangent);
	
	float3 sampleVec = tangent * H.x + bitangent * H.y + normal * H.z;
	return (sampleVec);
}

#endif
