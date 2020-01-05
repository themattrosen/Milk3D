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
	float3 worldPos : TEXCOORD0;
};

static matrix identity =
{
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 }
};

void CreateMVP(out matrix mvp, out matrix model)
{
	// Hard coded model matrix
	model = identity;
	
	// Hard coded view matrix
	matrix view = identity;
	const float3 position = float3(0,0,-5);
	const float3 lookAt = float3(0,0,1);
	const float3 up = float3(0,1,0);
	const float3 f = (normalize(lookAt - position));
	const float3 s = (normalize(cross(up, f)));
	const float3 u = (cross(f, s));

	view[0][0] = s.x;
	view[1][0] = s.y;
	view[2][0] = s.z;
	view[0][1] = u.x;
	view[1][1] = u.y;
	view[2][1] = u.z;
	view[0][2] = f.x;
	view[1][2] = f.y;
	view[2][2] = f.z;
	view[3][0] = -dot(s, position);
	view[3][1] = -dot(u, position);
	view[3][2] = -dot(f, position);
	
	// Hard coded projection matrix
	matrix projection = 0.0f;
	const float near = 0.1f;
	const float far = 1000.0f;
	const float fov = 0.785398163f;
	const float aspect = 1600.0f / 900.0f;
	const float tanHalfFov = tan(fov / 2.0f);
		
	projection[0][0] = 1.0f / (aspect * tanHalfFov);
	projection[1][1] = 1.0f / (tanHalfFov);
	projection[2][3] = 1.0f;

	projection[2][2] = (far + near) / (far - near);
	projection[3][2] = - (2.0f * far * near) / (far - near);
	
	mvp = mul(mul(model, view), projection);
}

PixelInput VS(VertexInput input)
{
	matrix mvp, model;
	CreateMVP(mvp, model);
	
	PixelInput output;
	output.position = mul(input.position, mvp);
	output.worldPos = mul(input.position, model);
	// World space lighting
	output.normal = normalize(mul(input.normal, (float3x3)model));
	return output;
}

#define POINT 0
#define DIRECTIONAL 1
#define SPOT 2
#define NUM_LIGHTS 2
struct Light
{
	int type;
	float3 position;
	float3 direction;
	float3 diffuse;
};
// TODO: cbuffers
void CreateLights(out Light lights[NUM_LIGHTS])
{
	lights[0].type = DIRECTIONAL;
	lights[0].position = float3(0,0,0);
	lights[0].direction = float3(0,0,-1);
	lights[0].diffuse = float3(1,0,0);
	
	lights[1].type = POINT;
	lights[1].position = float3(0,1,0);
	lights[1].direction = float3(0,0,1);
	lights[1].diffuse = float3(1,1,0);
}

float4 PS(PixelInput input) : SV_TARGET
{
	return float4(1,0,0,1);
	Light lights[NUM_LIGHTS];
	CreateLights(lights);
	
	float3 color = 0.0f;
	
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		float3 L = lights[i].direction;
		if (lights[i].type == POINT)
			L = normalize(input.worldPos - lights[i].position);
		
		color += lights[i].diffuse * dot(-L, input.normal);
	}
	
	return float4(color, 1.0f);
}
