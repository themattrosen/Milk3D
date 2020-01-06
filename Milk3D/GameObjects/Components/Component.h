#pragma once
#include "../Serialization/Serializer.h"

namespace Milk3D
{
	class GameObject;

	class Component
	{
	public:
		Component(GameObject* parent);
		virtual ~Component();

		GameObject* GetParent() { return m_parent; }

		virtual void OnUpdate(float dt) {}
		virtual void OnRender() {}
		virtual void OnActivate() {}
		virtual void OnDeactivate() {}
		virtual void OnCreate() {}
		virtual void OnDelete() {}

		virtual void Serialize(Serializer& s) = 0;

	private:
		GameObject* m_parent;
	};

	class ComponentIDBase
	{
	public:
		virtual unsigned GetComponentID() const = 0;
		static unsigned GetIDBase();
	
		static unsigned s_componentIDBase;
	};

	template<typename T>
	class ComponentID
	{
	public:
		unsigned GetComponentID() const { return s_componentID; }
		static unsigned GetComponentTypeID() { return s_componentID; }

	private:
		static unsigned s_componentID;
	};

	template<typename T>
	unsigned ComponentID<T>::s_componentID = ComponentIDBase::s_componentIDBase++;

	const constexpr unsigned INVALID_COMPONENT_TYPEID = (unsigned)(-1);

	template<typename T>
	class ComponentType : 
		public Component,
		public ComponentID<T>
	{
	public:
		ComponentType(GameObject* parent) : Component(parent) {}
	};
}
