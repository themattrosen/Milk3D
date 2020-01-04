#include "Graphics.h"
#include "Core\Window.h"
#include <Core\Engine.h>
#include "Graphics/Core/GraphicsDevice.h"

namespace Milk3D
{
	void GraphicsSystem::Init()
	{
		m_window = new Window("Milk3D", 1600, 900, 100, 100, true, false);
		unsigned width = 0;
		unsigned height = 0;
		m_window->GetDimensions(width, height);
		GraphicsDevice::GetInstance().Initialize(m_window->GetHandle(), m_window->GetWidth(), width, height, false);
	}

	void GraphicsSystem::Update(float dt)
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

	void GraphicsSystem::LateUpdate()
	{
	}

	void GraphicsSystem::Exit()
	{

		delete m_window;
	}

}