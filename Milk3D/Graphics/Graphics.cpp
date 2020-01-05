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
#include "Graphics/Core/RenderTarget.h"
#include "Graphics/Core/Cubemap.h"
#include "Graphics/Core/ForwardRenderer.h"
#include "Core/Input/Input.h"
#include "Editor/Editor.h"
#include <iostream>

namespace Milk3D
{
	Shader * mainShader = nullptr;
	Shader * textureShader = nullptr;
	Model * model = nullptr;
	Texture * texture1 = nullptr;
	Texture * texture2 = nullptr;
	Cubemap * cubemap = nullptr;
	RenderTarget * renderTarget = nullptr;
	Camera camera;
	#define MAX_LIGHTS 8
	struct LightBuffer
	{
		Light lights[MAX_LIGHTS];
		UINT activeLights = 0;
		DirectX::XMFLOAT3 padding;
	};

	static bool useCubemap = false;
	static bool useRenderTarget = true;

	static LightBuffer lightBuffer;

	struct SceneBuffer
	{
		DirectX::XMFLOAT3 cameraPosition;
		float padding = 0.0f;
	};

	static SceneBuffer sceneBuffer;

	Scene * mainScene = nullptr;
	GameObject * newObject = nullptr;

	void CreateLights()
	{
		using namespace DirectX;
#if 0
	

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

#endif
		Light light;
		light.enabled = true;
		light.position = XMFLOAT3(0, 0, 0);
		light.direction = XMFLOAT3(0, 0, 1);
		light.color = XMFLOAT3(1, 1, 1);
		light.type = LightType::directional;
		mainScene->AddLight(light);
	}

	void GraphicsSystem::OnEvent(SystemInitEvent * e)
	{
		m_window = new Window("Milk3D", 1600, 900, 100, 100, false);
		unsigned width = 0;
		unsigned height = 0;
		m_window->GetDimensions(width, height);
		GraphicsDevice::GetInstance().Initialize(m_window->GetHandle(), width, height, true, false);
		Editor::Initialize();
		ForwardRenderer::Initialize();

		mainScene = ForwardRenderer::AddScene();
		CreateLights();

		mainScene->GetCamera().SetPosition({ 0,0,-5.0f });

		// Make object
		Actor * actor = new Actor;
		newObject = actor;
		Transform & transform = newObject->GetTransform();
		transform.scale = Vec3(1, 1, 1);

		mainShader = new Shader;
		mainShader->Initialize("Shaders/Phong.hlsl", ShaderType::Pixel | ShaderType::Vertex);

		model = new Model;
		model->Initialize("Assets/Models/Cube.obj");

		texture1 = new Texture;
		texture1->Initialize("Assets/Image2.png");
		auto sampler = Sampler::GetSamplerWrap();

		Material material;
		material.albedo = texture1;
		material.samplers.push_back(sampler);

		actor->SetShader(mainShader);
		actor->SetMesh(model);
		actor->SetMaterial(material);

		std::vector<GameObject*> objects{ newObject };
		mainScene->SetObjects(objects);
#if 0

		std::string mainShaderPath = "Shaders/Phong.hlsl";
		if (useCubemap)
			mainShaderPath = "Shaders/Cubemap.hlsl";
		mainShader = new Shader;
		mainShader->Initialize(mainShaderPath.c_str(), ShaderType::Pixel | ShaderType::Vertex);

		textureShader = new Shader;
		textureShader->Initialize("Shaders/Texture.hlsl", ShaderType::Pixel | ShaderType::Vertex);

		model = new Model;
		model->Initialize("Assets/Models/Cube.obj");

		texture1 = new Texture;
		texture1->Initialize("Assets/Image2.png");

		texture2 = new Texture;
		texture2->Initialize("Assets/BRDF.dds");

		cubemap = new Cubemap;
		cubemap->Initialize("Assets/Skybox.dds");

		camera.SetPosition({ 0,0,-5.0f });
		
		CreateLights();

		BufferManager::GetInstance().CreateConstantBuffer(ShaderType::Pixel, 0, sizeof(LightBuffer));

		renderTarget = new RenderTarget;
		RenderTargetInfo renderTargetInfo(1, 1600, 900, DXGI_FORMAT_R32G32B32A32_FLOAT);
		renderTarget->Initialize(renderTargetInfo);

#endif
	}

	void MoveCamera()
	{
		Camera & cam = mainScene->GetCamera();
		DirectX::XMFLOAT3 camPos = cam.GetPosition();
		static const float delta = 0.1f;
		if (Input::Pressed(VK_W))
		{
			camPos.z += delta;
			cam.SetPosition(camPos);
		}

		if (Input::Pressed(VK_S))
		{
			camPos.z -= delta;
			cam.SetPosition(camPos);
		}

		if (Input::Pressed(VK_A))
		{
			camPos.x -= delta;
			cam.SetPosition(camPos);
		}

		if (Input::Pressed(VK_D))
		{
			camPos.x += delta;
			cam.SetPosition(camPos);
		}
	}
	void GraphicsSystem::OnEvent(SystemUpdateEvent * e)
	{
		bool shouldContinue = m_window->Update();

		MoveCamera();

		Transform & transform = newObject->GetTransform();
		static float r = 0.01f;
		r += 0.01f;
		transform.rotation = Quaternion(r,r,r);

		Editor::Start();

		ImGui::Begin("Test");

		ImGui::End();

		// End frame for rendering

		if (!shouldContinue)
		{
			ENGINE->StopRunning();
		}
	}

#define SAFE_DELETE(P) { if (P) { delete P; P = nullptr; }}
	void GraphicsSystem::OnEvent(SystemExitEvent * e)
	{
		Editor::Shutdown();
		ForwardRenderer::Shutdown();
		SAFE_DELETE(m_window);
		SAFE_DELETE(model);
		SAFE_DELETE(mainShader);
		SAFE_DELETE(textureShader);
		SAFE_DELETE(texture1);
		SAFE_DELETE(texture2);
		SAFE_DELETE(renderTarget);
		SAFE_DELETE(cubemap);
	}

	void GraphicsSystem::OnEvent(SystemRenderEvent * e)
	{
		GraphicsDevice & gd = GraphicsDevice::GetInstance();
		gd.StartFrame();
		ForwardRenderer::Render(mainScene);
		Editor::Render();
		gd.EndFrame();
#if 0
		if (!model->Failed() && !mainShader->Failed() || !textureShader->Failed())
		{
			GraphicsDevice & gd = GraphicsDevice::GetInstance();
			auto device = gd.GetDevice();
			auto deviceContext = gd.GetDeviceContext();

			gd.StartFrame();


			static float r = 0.0f;
			r += 0.01f;
			Matrix world = DirectX::XMMatrixRotationRollPitchYaw(r, r, r);
			Matrix const & view = camera.GetViewingMatrix();
			Matrix const & projection = Window::GetMainWindow()->GetProjection();

			UINT indexCount = static_cast<UINT>(model->IndexCount());
			model->Use();

			gd.SetRenderingTopology();


			// Render to texture
			{
				if (useRenderTarget)
				{
					float clearColor[] = { 1,0,0,1 };
					renderTarget->SetClearColor(clearColor);
					renderTarget->Use();
				}

				mainShader->Use();

				BufferManager::SetMatrixBuffer(world, view, projection);
				
				auto sampler = Sampler::GetSamplerWrap();
				deviceContext->PSSetSamplers(0, 1, &sampler);

				ID3D11ShaderResourceView * texture = texture1->GetShaderResource();
				if (useCubemap)
					texture = cubemap->GetShaderResource();
				deviceContext->PSSetShaderResources(0, 1, &texture);

				BufferManager::GetInstance().SendBuffer(&lightBuffer, Pixel, 0);

				sceneBuffer.cameraPosition = camera.GetPosition();
				BufferManager::GetInstance().SendBuffer(&sceneBuffer, Vertex, 1);

				gd.DrawIndexedInstanced(indexCount, 1);

				if (useRenderTarget)
					renderTarget->Disable();
			}

			// Render cube with render target texture
			if (useRenderTarget)
			{
				
				textureShader->Use();

				BufferManager::SetMatrixBuffer(world, view, projection);

				auto sampler = Sampler::GetSamplerWrap();
				deviceContext->PSSetSamplers(0, 1, &sampler);

				auto textures = renderTarget->GetShaderResources().front();
				UINT numViews = 1;
				deviceContext->PSSetShaderResources(0, numViews, &textures);

				gd.DrawIndexedInstanced(indexCount, 1);
			}

			Editor::Render();

			gd.EndFrame();
		}
#endif
	}

}