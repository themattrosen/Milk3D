#include "Graphics.h"
#include "Core\Window.h"
#include <Core\Engine.h>

namespace Milk3D
{
	void GraphicsSystem::Init()
	{
		m_window = new Window("Milk3D", 1600, 900, 100, 100, true, false);
	}

	void GraphicsSystem::Update(float dt)
	{
		bool shouldContinue = m_window->Update();
		m_window->StartFrame();


		m_window->EndFrame();

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