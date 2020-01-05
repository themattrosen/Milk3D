#include "GameObject.h"
#include "Components\Component.h"

namespace Milk3D
{
	GameObject::GameObject()
	{
		Subscribe(et_GameObjectUpdateEvent);
		Subscribe(et_GameObjectRenderEvent);
		m_components.resize(ComponentIDBase::GetIDBase(), nullptr);
	}
	GameObject::~GameObject()
	{
		for (auto* next : m_components)
		{
			if (next)
			{
				delete next;
				next = nullptr;
			}
		}
	}
	void GameObject::RemoveChild(GameObjectID child)
	{
		int size = (int)m_children.size();
		for (int i = 0; i < size; ++i)
		{
			if (m_children[i] == child)
			{
				OnRemoveChild(child, i);
				m_children.erase(m_children.begin() + i);
				break;
			}
		}
	}

	void GameObject::AddChild(GameObjectID child)
	{
		OnAddChild(child);
		m_children.push_back(child);
	}

	void GameObject::Activate()
	{
		if (m_isActive) return;
		OnActivate();
		// activate components
		for (auto* c : m_components)
			if (c) c->OnActivate();
		m_isActive = true;
	}

	void GameObject::Deactivate()
	{
		if (!m_isActive) return;
		OnDeactivate();
		// deactivate components
		for (auto* c : m_components)
			if(c) c->OnDeactivate();
		m_isActive = false;
	}

	void GameObject::OnEvent(GameObjectUpdateEvent * e)
	{
		OnUpdate(e->dt);
		for (auto* c : m_components)
			if (c) c->OnUpdate(e->dt);
	}

	void GameObject::OnEvent(GameObjectRenderEvent * e)
	{
		OnRender();
		for (auto* c : m_components)
			if (c) c->OnRender();
	}

	void GameObject::Serialize(Serializer & s)
	{
		s % m_id % m_parent % m_isActive;
		s % m_name % m_children % Serializer::Endline();

		// serialize components
		for (auto* c : m_components)
			if (c) c->Serialize(s);
	}

}