#pragma once
#include <Core/Events/EventListener.h>
#include <iostream>
namespace Milk3D
{
	class System : public EventListener<System>
	{
	public:
		//ENABLE_EVENT_LISTENER;
		template<typename T> void OnEvent(T* e) {
			std::cout << "OH HECK" << std::endl;
		}

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

#undef REGISTER_EVENT
#define REGISTER_EVENT(type, ...) virtual void OnEvent(type##Event * e) {}
#include "Core\Events\EventRegistry.h"
#undef REGISTER_EVENT
#define REGISTER_EVENT(type, ...) 
	};
}
