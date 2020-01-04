#pragma once
#include <Core/Events/EventListener.h>

namespace Milk3D
{
	class System : public EventListener<System>
	{
	public:
		ENABLE_EVENT_LISTENER;
		System() :
			EventListener<System>({
				et_SystemInitEvent,
				et_SystemUpdateEvent,
				et_SystemExitEvent,
				et_SystemRenderEvent
			})
		{

		}
		virtual ~System() {}

		virtual void OnEvent(SystemInitEvent* e) = 0;
		virtual void OnEvent(SystemExitEvent* e) = 0;
		virtual void OnEvent(SystemUpdateEvent* e) = 0;
		virtual void OnEvent(SystemRenderEvent* e) {};
	};
}
