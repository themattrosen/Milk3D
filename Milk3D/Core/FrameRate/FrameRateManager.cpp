//Matthew Rosen

#include "FrameRateManager.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Milk3D
{
	void FrameRateManager::Init()
	{
		__int64 countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER *)(&countsPerSec));

		m_secondsPerCount = 1.0 / static_cast<double>(countsPerSec);
		m_limitRate = -1.f;

		Reset();
	}

	void FrameRateManager::EnableFrameLimiter(float maxDT)
	{
		m_limitRate = static_cast<double>(maxDT);
	}

	void FrameRateManager::DisableFrameLimiter()
	{
		m_limitRate = -1.0;
	}

	void FrameRateManager::Update()
	{
		Tick();

		// case frame limiter is enabled
		if (m_limitRate >= 0.f && m_limitRate > m_deltaTime)
		{
			double timeRemaining = m_limitRate - m_deltaTime;
			DWORD msRemaining = static_cast<DWORD>(timeRemaining * 1000.0);

			Sleep(msRemaining);

			m_deltaTime += timeRemaining;

		}
	}

}