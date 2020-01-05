#pragma once

#include <Core\System\System.h>
#include "Body.h"
#include "Collider.h"
#include "PhysicsUtil.h"

#include <unordered_map>
#include <vector>

#define GRAVITY_MODE_DIRECTION true
#define GRAVITY_MODE_POINT false

#define ROTATIONAL_DRAG 8.f

namespace Milk3D
{
	class PhysicsSystem : public System
	{
	public:
		void OnEvent(SystemInitEvent* e);
		void OnEvent(SystemExitEvent* e);
		void OnEvent(SystemUpdateEvent* e);

		void OnEvent(AddToPhysicsEvent* e);
		void OnEvent(RemoveFromPhysicsEvent* e);
		void OnEvent(SetGravityEvent* e);

	private:
		void UpdateMotion(float dt);
		void GenerateContacts();
		void SeparateContacts();

		Vec3 m_gravity;
		bool m_gravityMode;

		std::vector<Manifold> m_contacts;

		std::unordered_map<GameObjectID, Body*> m_bodies;
		std::unordered_map<GameObjectID, Kinematics*> m_kinematics;
		std::unordered_map<GameObjectID, Collider*> m_colliders;
	};
}