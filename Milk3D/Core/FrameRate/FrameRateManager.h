//Matthew Rosen
#pragma once

#include "GameTimer.h"

namespace Milk3D
{
	class FrameRateManager : public GameTimer
	{
	public:
		FrameRateManager() = default;
		~FrameRateManager() = default;

		void Init();
		void Update();

		void EnableFrameLimiter(float maxDT);
		void DisableFrameLimiter();

	private:
		double m_limitRate;
	};
}