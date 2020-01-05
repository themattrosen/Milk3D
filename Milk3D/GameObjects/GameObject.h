#pragma once
#include <Core\Events\EventCommon.h>

#include <vector>
#include <string>

namespace Milk3D
{

	class GameObject : public EventListener<GameObject>
	{
	public:
		GameObjectID GetID() const { return m_id; }
		const std::string& GetName() const { return m_name; }
		void SetName(const std::string& name) { m_name = name; }
		GameObjectID GetParent() const { return m_parent; }
		const std::vector<GameObjectID>& GetChildren() const { return m_children; }

		virtual void OnDelete() {}
		virtual void OnCreate() {}

		void Activate() {}
		void Deactivate() {}
		bool IsActive() const { return m_isActive; }

		virtual void OnActivate() {}
		virtual void OnDeactivate() {}

		virtual void Save

	private:
		GameObjectID m_id;
		GameObjectID m_parent;

		bool m_isActive;

		std::string m_name;
		std::vector<GameObjectID> m_children;

		friend class GameSystem;
	};
}