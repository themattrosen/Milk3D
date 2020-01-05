// General defines
#define POINT 0
#define SPOT 1
#define DIRECTIONAL 2
#define MAX_LIGHTS 8
#define DEFAULT_GAMMA 2.2f
#define BIAS 0.0005f
#define PI 3.141592654f;
#define TWO_PI 6.283185307f;
#define ONE_DIV_PI 0.318309886f;
#define ONE_DIV_TWO_PI 0.159154943f;
#define PIDIV2 1.570796327f;
#define PIDIV4 0.785398163f;
#define INFINITY 1.#INF

// Material bits
#define ALBEDO_BIT 0
#define NORMALMAP_BIT 1
#define SPECULAR_BIT 2
#define METALLIC_BIT 3
#define ROUGHNESS_BIT 4
#define AO_BIT 5

// Helper macros
#define CHECK_BIT(NUMBER, BIT) (((NUMBER>>BIT) & 1))
#define CREATE_SAMPLER(NAME, SLOT, FILTER, ADDRESS_U, ADDRESS_V, ADDRESS_W, COMPARISON) \
SamplerState NAME : register(SLOT) \
{ \
    Filter = FILTER; \
    AddressU = ADDRESS_U; \
    AddressV = ADDRESS_V; \
	AddressW = ADDRESS_W; \
	ComparisonFunc = COMPARISON; \
}; \

#define CREATE_SAMPLER_WRAP(NAME, SLOT, FILTER, COMPARISON) CREATE_SAMPLER(NAME, SLOT, FILTER, Wrap, Wrap, Wrap, COMPARISON)
#define CREATE_SAMPLER_CLAMP(NAME, SLOT, FILTER, COMPARISON) CREATE_SAMPLER(NAME, SLOT, FILTER, Clamp, Clamp, Clamp, COMPARISON)
#define CREATE_SAMPLER_LINEAR(NAME, SLOT, ADDRESS_U, ADDRESS_V, ADDRESS_W, COMPARISON) CREATE_SAMPLER(NAME, SLOT, MIN_MAG_MIP_LINEAR, ADDRESS_U, ADDRESS_V, ADDRESS_W, COMPARISON)
#define CREATE_SAMPLER_LINEAR_ALWAYS(NAME, SLOT, ADDRESS_U, ADDRESS_V, ADDRESS_W, COMPARISON) CREATE_SAMPLER(NAME, SLOT, MIN_MAG_MIP_LINEAR, ADDRESS_U, ADDRESS_V, ADDRESS_W, ALWAYS)
#define CREATE_SAMPLER_LINEAR_WRAP(NAME, SLOT, COMPARISON) CREATE_SAMPLER(NAME, SLOT, MIN_MAG_MIP_LINEAR, Wrap, Wrap, Wrap, COMPARISON)
#define CREATE_SAMPLER_LINEAR_CLAMP(NAME, SLOT, COMPARISON) CREATE_SAMPLER(NAME, SLOT, MIN_MAG_MIP_LINEAR, Clamp, Clamp, Clamp, COMPARISON)
#define CREATE_DEFAULT_SAMPLER(NAME, SLOT) CREATE_SAMPLER(NAME, SLOT, MIN_MAG_MIP_LINEAR, Wrap, Wrap, Wrap, ALWAYS)


// GLSL defines
#define vec2 float2
#define vec3 float3
#define vec4 float4
#define mat4 matrix
#define mat3 float3x3
#define mat2 float2x2
#define mix lerp

//------------------------------------------------------------------------------
// Structures/Classes:
//------------------------------------------------------------------------------

struct Light
{
	int enabled;
	float3 position;
	int type;
	float3 direction;
	float3 color;
	float padding;
};

//------------------------------------------------------------------------------
// Constant Buffers:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Helper Functions:
//------------------------------------------------------------------------------

inline float3 GammaCorrection(float3 color, float gamma = DEFAULT_GAMMA)
{
    float g = 1.0f / gamma;
    return float3(pow(saturate(color.xyz), float3(g,g,g)));
}
inline float4 GammaCorrection(float4 color, float gamma = DEFAULT_GAMMA)
{
    return float4(GammaCorrection(color.xyz, gamma), color.w);
}

inline float3 HDRTonemap(float3 color)
{
	return color / (color + float3(1.0f, 1.0f, 1.0f));
}

inline float4 HDRTonemap(float4 color)
{
	return float4(HDRTonemap(color.xyz), color.w);
}

// Get a random number based off of seed and given input.
// Example use: Random(input.position.xyz, seed = rand() from CPU cbuffer will give pseudo random scalar
inline float Random(float3 vec, float seed)
{
	return frac(seed * sin(dot(vec.xyz,float3(12.9898f,78.233f,118.7813f))) * (43758.5453f));
}

