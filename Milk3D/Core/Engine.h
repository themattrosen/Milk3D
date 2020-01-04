#pragma once

#include <vector>
#include <Core\FrameRate\FrameRateManager.h>
#include <Core\Events\EventCommon.h>

namespace Milk3D
{
	class System;

	class Engine
	{
	public:
		Engine();
		~Engine();

		void Run();
		bool IsRunning() const { return m_isRunning; }
		void StopRunning() { m_isRunning = false; }

	private:
		void Init();
		void Update();
		void Exit();

		std::vector<System*> m_systems;
		FrameRateManager m_frameRate;
		bool m_isRunning;
	};

	extern Engine* ENGINE;
} // namespace Milk3D
