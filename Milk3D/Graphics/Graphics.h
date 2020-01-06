#pragma once

#include <Core\System\System.h>

namespace Milk3D
{
	class GraphicsSystem : public System
	{
	public:

		void OnEvent(SystemInitEvent* e) override;
		void OnEvent(SystemUpdateEvent* e) override;
		void OnEvent(SystemExitEvent* e) override;
		void OnEvent(SystemRenderEvent* e) override;
		void OnEvent(AddToSceneEvent* e);
		void OnEvent(RemoveFromSceneEvent* e);
	private:
		class Window* m_window;
	};
}
