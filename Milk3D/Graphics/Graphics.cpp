#include "Graphics.h"
#include "Core\Window.h"
#include <Core\Engine.h>
#include "Graphics/Core/GraphicsDevice.h"

#include "Graphics/Core/Shader.h"
#include "Graphics/Core/Model.h"
#include "Graphics/Core/Texture.h"
#include "Graphics/Core/Sampler.h"
#include "Graphics/Core/BufferManager.h"
#include "Graphics/Core/Camera.h"
#include "Graphics/Core/GPUBuffer.h"
#include "Graphics/Core/Light.h"

namespace Milk3D
{
	Shader * shader = nullptr;
	Model * model = nullptr;
	Texture * texture1 = nullptr;
	Texture * texture2 = nullptr;
	Camera camera;
	#define MAX_LIGHTS 8
	struct LightBuffer
	{
		Light lights[MAX_LIGHTS];
		UINT activeLights = 0;
		DirectX::XMFLOAT3 padding;
	};

	static LightBuffer lightBuffer;

	struct SceneBuffer
	{
		DirectX::XMFLOAT3 cameraPosition;
		float padding = 0.0f;
	};

	static SceneBuffer sceneBuffer;

	void CreateLights()
	{
		using namespace DirectX;

		lightBuffer.activeLights = 1;
		//lightBuffer.lights[1].enabled = false;
		//lightBuffer.lights[1].position = XMFLOAT3(0, 0, -2);
		//lightBuffer.lights[1].direction = XMFLOAT3(0, 0, 1);
		//lightBuffer.lights[1].color = XMFLOAT3(1, 1, 1);
		//lightBuffer.lights[1].type = LightType::point;

		lightBuffer.lights[0].enabled = true;
		lightBuffer.lights[0].position = XMFLOAT3(0, 0, 0);
		lightBuffer.lights[0].direction = XMFLOAT3(0, 0, 1);
		lightBuffer.lights[0].color = XMFLOAT3(1, 1, 1);
		lightBuffer.lights[0].type = LightType::directional;
	}

	void GraphicsSystem::OnEvent(SystemInitEvent * e)
	{
		m_window = new Window("Milk3D", 1600, 900, 100, 100, false);
		unsigned width = 0;
		unsigned height = 0;
		m_window->GetDimensions(width, height);
		GraphicsDevice::GetInstance().Initialize(m_window->GetHandle(), width, height, true, false);

		shader = new Shader;
		shader->Initialize("Shaders/Phong.hlsl", ShaderType::Pixel | ShaderType::Vertex);

		model = new Model;
		model->Initialize("Assets/Models/Cube.obj");

		texture1 = new Texture;
		texture1->Initialize("Assets/Image2.png");

		texture2 = new Texture;
		texture2->Initialize("Assets/BRDF.dds");

		camera.SetPosition({ 0,0,-5.0f });
		
		CreateLights();

		BufferManager::GetInstance().CreateConstantBuffer(ShaderType::Pixel, 0, sizeof(LightBuffer));
	}

	void GraphicsSystem::OnEvent(SystemUpdateEvent * e)
	{
		bool shouldContinue = m_window->Update();

		if (!model->Failed() && !shader->Failed())
		{
			GraphicsDevice & gd = GraphicsDevice::GetInstance();
			auto device = gd.GetDevice();
			auto deviceContext = gd.GetDeviceContext();

			gd.StartFrame();

			shader->Use();
			model->Use();
			auto sampler = Sampler::GetSamplerWrap();
			deviceContext->PSSetSamplers(0, 1, &sampler);
			auto texture = texture1->GetShaderResource();
			deviceContext->PSSetShaderResources(0, 1, &texture);

			static float r = 0.0f;
			r += 0.01f;
			Matrix world = DirectX::XMMatrixRotationRollPitchYaw(r, r, r);
			Matrix const & view = camera.GetViewingMatrix();
			Matrix const & projection = Window::GetMainWindow()->GetProjection();
			BufferManager::SetMatrixBuffer(world, view, projection);

			BufferManager::GetInstance().SendBuffer(&lightBuffer, Pixel, 0);

			sceneBuffer.cameraPosition = camera.GetPosition();
			BufferManager::GetInstance().SendBuffer(&sceneBuffer, Vertex, 1);

			gd.SetRenderingTopology();
			UINT indexCount = static_cast<UINT>(model->IndexCount());
			gd.DrawIndexedInstanced(indexCount, 1);

			gd.EndFrame();
		}

		if (!shouldContinue)
		{
			ENGINE->StopRunning();
		}
	}

	void GraphicsSystem::OnEvent(SystemExitEvent * e)
	{
		delete m_window;
		delete model;
		delete shader;
		delete texture1;
		delete texture2;
	}

	void GraphicsSystem::OnEvent(SystemRenderEvent * e)
	{
	}

}