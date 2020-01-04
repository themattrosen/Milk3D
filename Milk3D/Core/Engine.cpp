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
		Init();

		while (IsRunning())
		{
			Update();
		}

		Exit();
	}

	void Engine::Init()
	{
#define REGISTER_SYSTEM(name) m_systems.push_back(new name);
#include <Core\System\SystemRegistry.h>
#undef REGISTER_SYSTEM

		for (auto* sys : m_systems)
		{
			sys->Init();
		}

		m_frameRate.Init();
	}

	void Engine::Update()
	{
		int size = m_systems.size();
		float dt = m_frameRate.DeltaTime();
		for (int i = 0; i < size; ++i)
		{
			m_systems[i]->Update(dt);
		}

		m_frameRate.Update();

		for (int i = 0; i < size; ++i)
		{
			m_systems[i]->LateUpdate();
		}
	}

	void Engine::Exit()
	{
		for (auto* sys : m_systems)
		{
			delete sys;
			sys = nullptr;
		}


		m_systems.clear();
	}

}