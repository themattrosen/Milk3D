#pragma once

#include <Core\System\System.h>

namespace Milk3D
{
	class GraphicsSystem : public System
	{
	public:
		void Init() override;
		void Update(float dt) override;
		void LateUpdate() override;
		void Exit() override;


	private:
		class Window* m_window;
	};
}
