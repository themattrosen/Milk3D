#include "PhysicsComponent.h"
#include "Core\Events\EventCommon.h"
#include "Physics\Collider.h"

namespace Milk3D
{

	void PhysicsComponent::OnActivate()
	{
		AddToPhysicsEvent add;
		add.parent = GetParent();
		SendEvent(&add);
		m_body = add.setBody;
	}

	void PhysicsComponent::OnDeactivate()
	{
		RemoveFromPhysicsEvent remove;
		remove.parent = GetParent();
		SendEvent(&remove);
		m_body = nullptr;
	}

	void PhysicsComponent::Serialize(Serializer & s)
	{
		M_ASSERT(m_body != nullptr &&
			"Can't serialize inactive component");
		// 1. serialize kinematics 
		Kinematics* k = m_body->movement;
		int pmType = (int)k->type;
		s % k->gravityScale % 
			k->velocity % 
			k->acceleration %
			k->angularVelocity %
			pmType %
			k->restitution %
			k->density %
			k->mass %
			k->inertia %
			k->autoCalcMass %
			k->isStatic %
			k->canRotate %
			Serializer::Endline();

		k->type = (PhysicsMaterialType)pmType;

		// 2. serialize collision data
		Collider* c = m_body->collider;
		int sType = (int)c->type;
		s % c->isTrigger %
			c->justCollided %
			sType %
			c->offset;
		c->type = (ShapeType)sType;

		switch (c->type)
		{
			case st_Sphere:
			{
				SphereCollider* sc = reinterpret_cast<SphereCollider*>(c);
				s % sc->radius;
				break;
			}

			case st_Box:
			{
				break;
			}
		}

		s % Serializer::Endline();
	}

}