//Matthew Rosen
#pragma once

namespace Milk3D
{

	class GameTimer
	{
	public:
		GameTimer();
		virtual ~GameTimer() {}

		void Tick();

		void Reset();

		void Pause(bool paused);
		void Start();
		void Stop();

		float DeltaTime() const;
		float GameTime() const;
		float TotalTime() const;

		void SetTimeScale(float multiplier);
		void ResetTimeScale();

	protected:
		double m_secondsPerCount;
		double m_deltaTime;

		__int64 m_baseTime;
		__int64 m_pausedTime;
		__int64 m_stopTime;
		__int64 m_prevTime;
		__int64 m_currTime;

		bool m_paused;

		double m_timeScale;

	};

}