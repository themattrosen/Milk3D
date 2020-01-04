//Matthew Rosen
#pragma once

namespace Milk3D
{
	struct Event;

	class EventListenerBase
	{
	public:
		EventListenerBase() = default;
		~EventListenerBase() = default;

		virtual void OnDispatch(Event* ev) = 0;
	};
}
