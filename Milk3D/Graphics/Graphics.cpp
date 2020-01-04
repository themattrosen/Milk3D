#include "Graphics.h"
#include "Core\Window.h"
#include <Core\Engine.h>
#include "Graphics/Core/GraphicsDevice.h"

#include "Graphics/Core/Shader.h"

namespace Milk3D
{
	void GraphicsSystem::OnEvent(SystemInitEvent * e)
	{
		m_window = new Window("Milk3D", 1600, 900, 100, 100, true, false);
		unsigned width = 0;
		unsigned height = 0;
		m_window->GetDimensions(width, height);
		GraphicsDevice::GetInstance().Initialize(m_window->GetHandle(), m_window->GetWidth(), width, height, false);

		Shader myShader;
		myShader.Initialize("Shaders/Shader.hlsl", ShaderType::Pixel | ShaderType::Vertex);
		myShader.Release();
	}

	void GraphicsSystem::OnEvent(SystemUpdateEvent * e)
	{
		bool shouldContinue = m_window->Update();
		GraphicsDevice & gd = GraphicsDevice::GetInstance();

		gd.StartFrame();

		gd.EndFrame();

		if (!shouldContinue)
		{
			ENGINE->StopRunning();
		}
	}

	void GraphicsSystem::OnEvent(SystemExitEvent * e)
	{
		delete m_window;

	}

	void GraphicsSystem::OnEvent(SystemRenderEvent * e)
	{
	}

}