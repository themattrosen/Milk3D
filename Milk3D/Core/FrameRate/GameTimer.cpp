//Matthew Rosen

#include "GameTimer.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Milk3D
{
	GameTimer::GameTimer()
	{
		__int64 countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER *)(&countsPerSec));

		m_secondsPerCount = 1.0 / static_cast<double>(countsPerSec);

		Reset();
	}

	void GameTimer::Tick()
	{
		if (m_paused)
		{
			m_deltaTime = 0.0;
			return;
		}

		// get the time this frame
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER *)&currTime);
		m_currTime = currTime;

		// calculate time difference from prev frame
		m_deltaTime = (m_currTime - m_prevTime) * m_secondsPerCount;

		// prepare for next frame
		m_prevTime = m_currTime;

		// catch negative dt
		if (m_deltaTime < 0.0)
		{
			m_deltaTime = 0.0;
		}
	}

	void GameTimer::Reset()
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER *)(&currTime));

		m_baseTime = currTime;
		m_prevTime = currTime;
		m_stopTime = 0;
		m_paused = false;
		m_timeScale = 1.0;
	}

	void GameTimer::Pause(bool paused)
	{
		(paused) ? Stop() : Start();
	}

	void GameTimer::Start()
	{
		// only un-pause if paused
		if (m_paused)
		{
			__int64 currTime;
			QueryPerformanceCounter((LARGE_INTEGER *)(&currTime));

			// accumulate the paused time
			m_pausedTime += (currTime - m_stopTime);

			m_prevTime = currTime;
			m_stopTime = 0;

			m_paused = false;
		}
	}

	void GameTimer::Stop()
	{
		// only pause if un-paused
		if (!m_paused)
		{
			__int64 currTime;
			QueryPerformanceCounter((LARGE_INTEGER *)(&currTime));

			// save the time when game was paused
			m_stopTime = currTime;
			m_paused = true;
		}
	}

	float GameTimer::DeltaTime() const
	{
		return static_cast<float>(m_deltaTime * m_timeScale);
	}

	float GameTimer::GameTime() const
	{
		return static_cast<float>(static_cast<double>(m_currTime) * m_secondsPerCount);
	}

	float GameTimer::TotalTime() const
	{
		if (m_paused)
		{
			return static_cast<float>(
				static_cast<double>(
				(m_stopTime - m_pausedTime) - m_baseTime
					) * m_secondsPerCount
				);
		}
		else
		{
			return static_cast<float>(
				static_cast<double>(
				((m_currTime - m_pausedTime) - m_baseTime)
					) * m_secondsPerCount
				);
		}
	}

	void GameTimer::SetTimeScale(float multiplier)
	{
		m_timeScale = static_cast<double>(multiplier);
	}

	void GameTimer::ResetTimeScale()
	{
		m_timeScale = 1.0;
	}

}
