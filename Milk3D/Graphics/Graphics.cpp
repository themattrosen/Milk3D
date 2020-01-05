#include "Graphics.h"
#include "Core\Window.h"
#include <Core\Engine.h>
#include "Graphics/Core/GraphicsDevice.h"

#include "Graphics/Core/Shader.h"
#include "Graphics/Core/Model.h"

namespace Milk3D
{
	Shader * shader = nullptr;
	Model * model = nullptr;

	void GraphicsSystem::OnEvent(SystemInitEvent * e)
	{
		m_window = new Window("Milk3D", 1600, 900, 100, 100, false);
		unsigned width = 0;
		unsigned height = 0;
		m_window->GetDimensions(width, height);
		GraphicsDevice::GetInstance().Initialize(m_window->GetHandle(), width, height, true, false);

		shader = new Shader;
		shader->Initialize("Shaders/TestShader3DLighting.hlsl", ShaderType::Pixel | ShaderType::Vertex);

		model = new Model;
		model->Initialize("Assets/Models/Cube.obj");
	}

	void GraphicsSystem::OnEvent(SystemUpdateEvent * e)
	{
		bool shouldContinue = m_window->Update();
		GraphicsDevice & gd = GraphicsDevice::GetInstance();
		auto device = gd.GetDevice();
		auto deviceContext = gd.GetDeviceContext();

		gd.StartFrame();

		shader->Use();
		model->Use();
		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->DrawIndexedInstanced(static_cast<UINT>(model->IndexCount()), 1, 0, 0, 0);

		gd.EndFrame();

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
	}

	void GraphicsSystem::OnEvent(SystemRenderEvent * e)
	{
	}

}