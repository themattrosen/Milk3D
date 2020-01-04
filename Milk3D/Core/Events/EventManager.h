//Matthew Rosen
#pragma once
#include <vector>
#include <queue>
#include <unordered_set>
#include <Core\Memory\StackAllocator.h>
#include "EventListenerBase.h"
#include "EventInclude.h"

#define EVENT_STACK_SIZE sizeof(int) * 100

namespace Milk3D
{
	// to see how to listen for events, look at the file EventListener.h
	// to queue events, use Milk3D::QueueEvent(yourevent* here);
	// to process all queued events use Milk3D::ProcessEvents();
	// to send an event immediately, use Milk3D::SendEvent(yourevent* here);
	// max size of events queued in bytes defined by EVENT_STACK_SIZE at the top of the file
	class EventManager
	{
	public:

		static EventManager& Instance()
		{
			static EventManager e;
			return e;
		}

		template<typename E>
		void QueueEvent(E* ev)
		{
			E* evCopy = m_eventMemory.Construct<E>(*ev);
			m_queuedEvents.push(evCopy);
		}

		template<typename E>
		void SendEvent(E* ev)
		{
			auto& listenerSet = m_listeners[static_cast<int>(ev->type)];
			for (auto* listener : listenerSet)
			{
				listener->OnDispatch(ev);
			}
		}

		void ProcessEvents()
		{
			while (!m_queuedEvents.empty())
			{
				Event* nextEvent = m_queuedEvents.front();
				m_queuedEvents.pop();
				int index = static_cast<int>(nextEvent->type);

				auto& listenerSet = m_listeners[index];
				for (auto* listener : listenerSet)
				{
					listener->OnDispatch(nextEvent);
				}
			}

			m_eventMemory.Clear();
		}

		void Subscribe(EventType type, EventListenerBase* listener)
		{
			int index = static_cast<int>(type);
			m_listeners[index].insert(listener);
		}

		void Unsubscribe(EventType type, EventListenerBase* listener)
		{
			m_listeners[type].erase(listener);
		}

		void UnsubscribeAll(EventListenerBase* listener)
		{
			for (size_t i = 0; i < m_listeners.size(); ++i)
			{
				m_listeners[i].erase(listener);
			}
		}

	private:
		EventManager()
			: m_listeners(), m_eventMemory(EVENT_STACK_SIZE)
		{
			m_listeners.resize(et_EventCount);
		}

		std::vector<std::unordered_set<EventListenerBase*>> m_listeners;
		StackAllocator m_eventMemory;
		std::queue<Event*> m_queuedEvents;
	};

	// API
	static void ProcessEvents()
	{
		EventManager::Instance().ProcessEvents();
	}

	template<typename E>
	void QueueEvent(E* ev)
	{
		EventManager::Instance().QueueEvent(ev);
	}

	template<typename E>
	void SendEvent(E* ev)
	{
		EventManager::Instance().SendEvent(ev);
	}
}
