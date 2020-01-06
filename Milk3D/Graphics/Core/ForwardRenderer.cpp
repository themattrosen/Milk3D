#include "ForwardRenderer.h"
#include "Window.h"
#include "GraphicsDevice.h"
#include "BufferManager.h"
#include "Sampler.h"
#include "GameObjects/GameObjectSubclasses/Actor.h"
#include "GameObjects\Components\GraphicsComponent.h"
#include "Model.h"
#include "Assets\AssetTypes\AssetTypeInclude.h"

namespace Milk3D
{
	std::vector<Scene> ForwardRenderer::s_scenes;
	ForwardRenderer::LightBuffer ForwardRenderer::m_lightBuffer;
	ForwardRenderer::SceneBuffer ForwardRenderer::m_sceneBuffer;

	void ForwardRenderer::Initialize()
	{
		BufferManager::GetInstance().CreateConstantBuffer(ShaderType::Pixel, 0, sizeof(LightBuffer));
		BufferManager::GetInstance().CreateConstantBuffer(ShaderType::Vertex, 1, sizeof(SceneBuffer));
	}

	void ForwardRenderer::Shutdown()
	{

	}
	Scene * ForwardRenderer::AddScene()
	{
		s_scenes.push_back(Scene());
		return &s_scenes.back();
	}
	void ForwardRenderer::Render(Scene * scene)
	{
		auto window = Window::GetMainWindow();
		if (!scene || !window) return;

		auto const & objects = scene->GetObjects();
		auto & lights = scene->GetLights();
		auto & camera = scene->GetCamera();

		auto it = lights.begin();
		auto end = lights.end();
		UINT activeLights = 0;
		for (; it != end;)
		{
			auto & light = *it;
			if (!light.enabled)
			{
				it = lights.erase(it);
				continue;
			}
			else
			{
				activeLights++;
				++it;
			}
		}

		// Copy lights over
		std::copy(lights.begin(), lights.end(), m_lightBuffer.lights);
		m_lightBuffer.activeLights = activeLights;

		GraphicsDevice & gd = GraphicsDevice::GetInstance();
		auto device = gd.GetDevice();
		auto deviceContext = gd.GetDeviceContext();

		gd.SetRenderingTopology();

		Matrix const & view = camera.GetViewingMatrix();
		Matrix const & projection = Window::GetMainWindow()->GetProjection();


		// TODO: Sort objects based off of same shader, then same material, then same model
		for (auto const & object : objects)
		{
			if (!object) continue;

			auto actor = dynamic_cast<Actor*>(object);
			auto* component = object->GetComponent<GraphicsComponent>();

			if (!component) continue;

			auto & transform = object->GetTransform();
			auto* textureasset = component->GetTexture();
			auto* meshasset = component->GetMesh();
			auto* shaderasset = component->GetShader();
			auto& material = component->GetMaterial();
			if (component->IsCamera())
			{
				camera.SetPosition(transform.position);
				camera.SetRotation(transform.rotation);
				camera.SetUp(Vec3(0, 1, 0));
			}

			auto* mesh = meshasset->GetMesh();
			auto* texture = textureasset->GetTexture();
			auto* shader = shaderasset->GetShader();

			if (mesh->Failed() || shader->Failed()) continue;

			Matrix world = Translate(transform.position) * transform.rotation.GetMatrix() * Scale(transform.scale);

			UINT indexCount = static_cast<UINT>(mesh->IndexCount());
			mesh->Use();
			shader->Use();

			BufferManager::GetInstance().SendBuffer(&m_lightBuffer, Pixel, 0);

			m_sceneBuffer.cameraPosition = camera.GetPosition();
			BufferManager::GetInstance().SendBuffer(&m_sceneBuffer, Vertex, 1);

			BufferManager::SetMatrixBuffer(world, view, projection);

			material.Use();

			gd.DrawIndexedInstanced(indexCount, 1);
		}
	}
	void ForwardRenderer::RenderAll()
	{
		// TODO: Huge optimization in reordering render for the following order:
		/*
		1.) For each object that uses this shader
		2.) For each object that uses this material (textures)
		3.) For each object that uses this mesh
		4.) Render object
		*/

		for (auto & scene : s_scenes)
		{
			Render(&scene);
		}
	}
}