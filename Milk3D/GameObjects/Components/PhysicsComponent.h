#pragma once

#include "Component.h"
#include "Physics\Body.h"
#include "Physics\Collider.h"

namespace Milk3D
{
	class PhysicsComponent : public ComponentType<PhysicsComponent>
	{
	public:
		PhysicsComponent(GameObject* parent) : 
			ComponentType<PhysicsComponent>(parent),
			m_body(nullptr)
		{}
		void OnActivate() override;
		void OnDeactivate() override;

		void Serialize(Serializer& s) override;

		Body* GetBody() { return m_body; }
	private:
		Body* m_body;
	};
}
