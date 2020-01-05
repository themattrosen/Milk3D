#include "Engine.h"
#include <Core\System\System.h>
#include <Core\System\SystemInclude.h>
#include <Common.h>

namespace Milk3D
{
	Engine* ENGINE = nullptr;
	Engine::Engine() :
		m_systems(), m_frameRate(), m_isRunning(true)
	{
		M_ASSERT(ENGINE == nullptr);
		ENGINE = this;
	}

	Engine::~Engine()
	{
		ENGINE = nullptr;
	}

	void Engine::Run()
	{
		EngineStartEvent start;
		Milk3D::SendEvent(&start);
		Init();

		while (IsRunning())
		{
			Update();
		}

		Exit();
		EngineStopEvent stop;
		Milk3D::SendEvent(&stop);
	}

	void Engine::Init()
	{
		EventManager::Instance();

#define REGISTER_SYSTEM(name) m_systems.push_back(new name);
#include <Core\System\SystemRegistry.h>
#undef REGISTER_SYSTEM
		
		m_frameRate.Init();

		SystemInitEvent init;
		Milk3D::SendEvent(&init);
	}

	void Engine::Update()
	{
		float dt = m_frameRate.DeltaTime();
		SystemUpdateEvent update;
		update.dt = dt;
		Milk3D::SendEvent(&update);

		m_frameRate.Update();

		SystemRenderEvent render;
		Milk3D::SendEvent(&render);
	}

	void Engine::Exit()
	{
		SystemExitEvent exit;
		Milk3D::SendEvent(&exit);

		for (auto* sys : m_systems)
		{
			delete sys;
			sys = nullptr;
		}

		m_systems.clear();
	}

}