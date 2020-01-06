#pragma once
#include <Core\Events\EventCommon.h>
#include "Serialization\Serializer.h"
#include "Reflection\GameObjectTypeID.h"
#include <GameObjects\Transform.h>

#include <vector>
#include <string>

namespace Milk3D
{

	class GameObject : 
		public EventListener<GameObject>
	{
	public:
		ENABLE_EVENT_LISTENER;

		GameObject();
		~GameObject();
		GameObjectID GetID() const { return m_id; }
		const std::string& GetName() const { return m_name; }
		void SetName(const std::string& name) { m_name = name; }
		GameObjectID GetParent() const { return m_parent; }
		const std::vector<GameObjectID>& GetChildren() const { return m_children; }
		Transform& GetTransform() { return m_transform; }

		template<typename T>
		T* GetComponent()
		{
			return reinterpret_cast<T*>(m_components[T::GetComponentTypeID()]);
		}
		
		template<typename T>
		T* AddComponent(T* component)
		{
			unsigned index = T::GetComponentTypeID();
			if (m_components[index] != nullptr)
			{
				delete m_components[index];
			}
			m_components[index] = component;
			return reinterpret_cast<T*>(m_components[index]);
		}

		template<typename T>
		T* AddComponent()
		{
			unsigned index = T::GetComponentTypeID();
			if (m_components[index] != nullptr)
			{
				delete m_components[index];
			}
			m_components[index] = new T(this);
			m_components[index]->OnCreate();
			m_components[index]->OnActivate();
			return reinterpret_cast<T*>(m_components[index]);
		}

		template<typename T>
		void DeleteComponent(T* component = nullptr)
		{
			unsigned index = T::GetComponentTypeID();
			if (m_components[index] != nullptr)
			{
				m_components[index]->OnDeactivate();
				m_components[index]->OnDelete();
				delete m_components[index];
			}

			m_components[index] = nullptr;
		}

		void RemoveChild(GameObjectID child);
		void AddChild(GameObjectID child);

		void Activate();
		void Deactivate();
		bool IsActive() const { return m_isActive; }

		void OnEvent(GameObjectUpdateEvent* e);
		void OnEvent(GameObjectRenderEvent* e);

		// derived classes can override these
		virtual void OnUpdate(float dt) {}
		virtual void OnRender() {}
		virtual void OnActivate() {}
		virtual void OnDeactivate() {}
		virtual void OnRemoveChild(GameObjectID child, int index) {}
		virtual void OnAddChild(GameObjectID child) {}
		virtual void OnDelete() {}
		virtual void OnCreate() {}
		virtual void OnCollisionEnter(GameObject* other) {}
		virtual void OnCollisionExit(GameObject* other) {}

		virtual void Serialize(Serializer& s);

	private:
		GameObjectID m_id;
		GameObjectID m_parent;

		bool m_isActive;
		Transform m_transform;

		std::string m_name;
		std::vector<GameObjectID> m_children;
		std::vector<class Component*> m_components;

		friend class GameSystem;
	};

	template<typename T>
	T* GetSibling(GameObjectID parent)
	{
		GetGameObjectEvent get;
		get.id = parent;
		SendEvent(&get);
		return get.setObj->GetComponent<T>();
	}
}