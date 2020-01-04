#include "Graphics.h"
#include "Core\Window.h"
#include <Core\Engine.h>

namespace Milk3D
{
	void GraphicsSystem::OnEvent(SystemInitEvent * e)
	{
		m_window = new Window("Milk3D", 1600, 900, 100, 100, true, false);

	}

	void GraphicsSystem::OnEvent(SystemUpdateEvent * e)
	{
		bool shouldContinue = m_window->Update();
		m_window->StartFrame();


		m_window->EndFrame();

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