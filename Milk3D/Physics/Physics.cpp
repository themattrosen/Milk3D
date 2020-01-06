#include "Physics.h"
#include <GameObjects\GameObject.h>

namespace Milk3D
{
	void PhysicsSystem::OnEvent(SystemInitEvent * e)
	{
		Subscribe(et_AddToPhysicsEvent);
		Subscribe(et_RemoveFromPhysicsEvent);
	}

	void PhysicsSystem::OnEvent(SystemExitEvent * e)
	{
		for (auto& pair : m_kinematics)
		{
			delete pair.second;
			pair.second = nullptr;
		}
		m_kinematics.clear();

		for (auto& pair : m_colliders)
		{
			delete pair.second;
			pair.second = nullptr;
		}
		m_colliders.clear();

		for (auto& pair : m_bodies)
		{
			delete pair.second;
			pair.second = nullptr;
		}
		m_bodies.clear();
	}

	void PhysicsSystem::OnEvent(SystemUpdateEvent * e)
	{
		UpdateMotion(e->dt);

		//GenerateContacts();

		//SeparateContacts();
	}

	void PhysicsSystem::OnEvent(AddToPhysicsEvent * e)
	{
		Body* b = new Body;
		Kinematics* k = new Kinematics(b, pmt_Custom);
		SphereCollider* s = new SphereCollider(b);
		b->collider = s;
		b->movement = k;
		b->parent = e->parent;
		b->transform = &(e->parent->GetTransform());
		e->setBody = b;

		m_bodies.insert_or_assign(e->parent->GetID(), b);
		m_kinematics.insert_or_assign(e->parent->GetID(), k);
		m_colliders.insert_or_assign(e->parent->GetID(), s);
	}

	void PhysicsSystem::OnEvent(RemoveFromPhysicsEvent * e)
	{
		GameObjectID id = e->parent->GetID();
		auto it1 = m_bodies.find(id);
		auto it2 = m_kinematics.find(id);
		auto it3 = m_colliders.find(id);

		if (it1 == m_bodies.end()) return;

		delete it1->second;
		delete it2->second;
		delete it3->second;

		m_bodies.erase(it1);
		m_kinematics.erase(it2);
		m_colliders.erase(it3);
	}

	void PhysicsSystem::OnEvent(SetGravityEvent * e)
	{
		m_gravity = e->direction;
		m_gravityMode = e->mode;
	}

	void PhysicsSystem::UpdateMotion(float dt)
	{
		for (auto& pair : m_kinematics)
		{
			Kinematics* k = pair.second;
			k->RecalculateMass();
			if (k->isStatic) continue;
			Transform* transform = k->parent->transform;

			// apply gravity
			float gravityScaleMultiplier = (k->gravityScale * k->mass * 0.1f);
			if (m_gravityMode == GRAVITY_MODE_DIRECTION)
			{
				k->ApplyImpulse(m_gravity * gravityScaleMultiplier, Vec3());
			}
			else if (m_gravityMode == GRAVITY_MODE_POINT)
			{
				Vec3 direction = m_gravity - transform->position;
				direction.Normalize();
				k->ApplyImpulse(direction * gravityScaleMultiplier, Vec3());
			}

			Vec3 t = transform->position;
			Vec3 v = k->velocity;
			Vec3 a = k->acceleration;

			v += a * dt;
			v += k->forceAccum * k->GetInverseMass() * dt;
			t += v * dt;

			Vec3 av = k->angularVelocity;
			if (av.Length() > 0.f)
			{
				if (av.X() > 0.f) av.SetX(av.X() - ROTATIONAL_DRAG);
				else av.SetX(av.X() + ROTATIONAL_DRAG);
				if (av.Y() > 0.f) av.SetY(av.Y() - ROTATIONAL_DRAG);
				else av.SetY(av.Y() + ROTATIONAL_DRAG);
				if (av.Z() > 0.f) av.SetZ(av.Z() - ROTATIONAL_DRAG);
				else av.SetZ(av.Z() + ROTATIONAL_DRAG);
			}

			auto r = transform->rotation;
			r.AddRotation(Quaternion(av.Z(), av.X(), av.Y()));

			k->angularVelocity = av;
			k->velocity = v;
			transform->position = t;
			transform->rotation = r;
			k->forceAccum = Vec3();
		}
	}

	void PhysicsSystem::GenerateContacts()
	{
		for (auto iter = m_colliders.begin(); iter != m_colliders.end(); ++iter)
		{
			Collider* c1 = iter->second;
			for (auto iter2 = ++iter; iter2 != m_colliders.end(); ++iter2)
			{
				Collider* c2 = iter2->second;
				Manifold nextM;
				nextM.A = c1->parent;
				nextM.B = c2->parent;
				bool wereColliding = c1->justCollided || c2->justCollided;

				bool result = collisionTable[c1->type][c2->type](nextM, c1, c2);
				if (result)
				{
					c1->justCollided = true;
					c2->justCollided = true;
					m_contacts.push_back(nextM);

					if (!wereColliding)
					{
						nextM.A->parent->OnCollisionEnter(nextM.B->parent);
						nextM.B->parent->OnCollisionEnter(nextM.A->parent);
					}
				}
				else
				{
					c1->justCollided = false;
					c2->justCollided = false;

					if (wereColliding)
					{
						nextM.A->parent->OnCollisionExit(nextM.B->parent);
						nextM.B->parent->OnCollisionExit(nextM.A->parent);
					}
				}
			}
		}
	}

	void PhysicsSystem::SeparateContacts()
	{
		int size = (int)m_contacts.size();
		for (int i = 0; i < size; ++i)
		{
			Manifold& pair = m_contacts[i];
			Body* a = pair.A;
			Body* b = pair.B;

			Transform* tA = a->transform;
			Transform* tB = b->transform;
			Kinematics* rA = a->movement;
			Kinematics* rB = b->movement;

			Vec3 rv = rB->velocity - rA->velocity;
			float velAlongNormal = rv.Dot(pair.normal);
			if (velAlongNormal < 0) continue;

			float invMassSum = rA->GetInverseMass() + rB->GetInverseMass();
			if (!invMassSum) continue;

			float e = (rA->restitution + rB->restitution) / 2.f;
			float j = -(1.f + e) * velAlongNormal;
			j /= invMassSum;

			Vec3 impulse = pair.normal * j;
			rA->ApplyImpulse(impulse, Vec3());
			rB->ApplyImpulse(impulse * -1.f, Vec3());

			// object a is penetrating into object b
			// moving in a direction opposite to the normal
			//const f32 percent = 0.3f; // could be between 0.2 and 0.8
			//const f32 slop = 0.085f; // usually between 0.01 and 0.1
			//vec2 correction = MAX(abs(pair.penetration) - slop, 0.f) / invMassSum * percent * pair.normal;
			//tA->SetPosition(tA->GetPosition() + correction * aInvMass);
			//tB->SetPosition(tB->GetPosition() - correction * bInvMass);
		}

		m_contacts.clear();
	}

}