//Matthew Rosen
#pragma once

#include "EventListenerBase.h"
#include "EventInclude.h"
#include "EventManager.h"
#include <initializer_list>
#include <string>

#define ENABLE_EVENT_LISTENER template<typename T> void OnEvent(T* e) {}

namespace Milk3D
{
	// To listen for events, inherit from EventListener<your class here>
	// in the public section of your class put the line
	// ENABLE_EVENT_LISTENER;
	// In your class constructor initializer list:
	// : EventListener<YourType>{ et_Event1Event, et_PlayEvent, ... all other events you want }
	// then for each event you subscribe to make a function
	// this function will be called whenever the event is sent out
	//
	// void OnEvent(YourEvent* e);
	//
	template<typename T>
	class EventListener : public EventListenerBase
	{
	public:
		EventListener() = default;
		EventListener(const std::initializer_list<EventType> & list)
		{
			for (auto i = list.begin(); i != list.end(); ++i)
			{
				Subscribe(*i);
			}
		}

		virtual ~EventListener()
		{
			UnsubscribeAll();
		}

		void OnDispatch(Event* ev)
		{
#			undef REGISTER_EVENT
#			define REGISTER_EVENT(type, ...) \
				case et_##type##Event :	\
				{	\
					T* base = static_cast<T*>(this);	\
					base->OnEvent(reinterpret_cast<type##Event*>(ev));	\
					break;	\
				}

			switch (ev->type)
			{
#			include "EventRegistry.h"
			default:
				M_ASSERT(false && "Unkown event type!");
			}

#			undef REGISTER_EVENT
#			define REGISTER_EVENT(type, ...) 
		}

		virtual void OnEvent(void* e) 
		{ 
			(void)e; 
			M_ASSERT(false &&
				"this should not have been called, yell at Matt until its fixed"); 
		}

		M_INLINE void Subscribe(EventType type)
		{
			EventManager::Instance().Subscribe(type, this);
		}

		M_INLINE void Unsubscribe(EventType type)
		{
			EventManager::Instance().Unsubscribe(type, this);
		}

		M_INLINE void UnsubscribeAll()
		{
			EventManager::Instance().UnsubscribeAll(this);
		}
	};
}
