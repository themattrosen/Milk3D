#pragma once
#include <Core\Math\Math.h>
#include <GameObjects\Transform.h>

namespace Milk3D
{
	struct Kinematics;
	struct Collider;

	struct Body
	{
		class GameObject* parent;
		Transform* transform;
		Kinematics* movement;
		Collider* collider;
	};

	enum PhysicsMaterialType
	{
		pmt_Custom = 0,
		pmt_Rock,
		pmt_Wood,
		pmt_Metal,
		pmt_Ice,
		pmt_BouncyBall,
		pmt_SuperBall,
		pmt_Pillow,
		pmt_Static,
	};

	struct PhysicsMaterial
	{
		PhysicsMaterialType type;
		float restitution;
		float density;
	};

	const PhysicsMaterial substanceList[] = {
	{ pmt_Custom,	  0.3f, 0.0f },
	{ pmt_Rock,       0.6f, 0.1f },  // Rock       Density : 0.6  Restitution : 0.1
	{ pmt_Wood,       0.3f, 0.2f },  // Wood       Density : 0.3  Restitution : 0.2
	{ pmt_Metal,      1.2f, 0.05f }, // Metal      Density : 1.2  Restitution : 0.05
	{ pmt_Ice,        0.6f, 0.1f },  // Ice        Density : 1.2  Restitution : 0.05
	{ pmt_BouncyBall, 0.3f, 0.8f },  // BouncyBall Density : 0.3  Restitution : 0.8
	{ pmt_SuperBall,  0.3f, 0.95f }, // SuperBall  Density : 0.3  Restitution : 0.95
	{ pmt_Pillow,     0.1f, 0.2f },  // Pillow     Density : 0.1  Restitution : 0.2
	{ pmt_Static,     0.0f, 0.4f }  // Static     Density : 0.0  Restitution : 0.4
	};

	struct Kinematics
	{
		Kinematics(Body* _parent, PhysicsMaterialType _type, bool _autoMass = false) :
			parent(_parent), type(_type), restitution(substanceList[type].restitution),
			density(substanceList[type].density), autoCalcMass(_autoMass)
		{
			RecalculateMass();
		}

		void RecalculateMass()
		{
			if (!autoCalcMass) return;
			Transform* t = parent->transform;
			Vec3 size = t->scale;
			if (!isStatic)
			{
				// mass = density * volume
				float x = size.X();
				float y = size.Y();
				float z = size.Z();

				mass = density * x * y * z;
				inertia = mass * (x * x + y * y + z * z) / 12.f;
			}
			else
			{
				mass = 0.f;
				inertia = 0.f;
			}
		}

		void ApplyImpulse(const Vec3& force, const Vec3& contactVec = Vec3())
		{
			velocity += force * GetInverseMass();
			if (canRotate)
				angularVelocity += GetInverseInertia() * Cross(contactVec, force) * (180.f / 3.141593f);
		}

		void ApplyForce(const Vec3& force)
		{
			forceAccum += force;
		}

		void SetMaterialType(PhysicsMaterialType ntype)
		{
			type = ntype;
			restitution = substanceList[type].restitution;
			density = substanceList[type].density;
			RecalculateMass();
		}

		float GetInverseMass() const { return mass == 0.f ? 0.f : 1.f / mass; }
		float GetInverseInertia() const { return inertia == 0.f ? 0.f : 1.f / inertia; }

		Body* parent = nullptr;

		float gravityScale = 1.f;
		Vec3 velocity = {};
		Vec3 acceleration = {};
		Vec3 angularVelocity = {};
		Vec3 forceAccum = {};

		PhysicsMaterialType type = pmt_Custom;
		float restitution = 0.3f;
		float density = 0.f;
		float mass = 0.f;
		float inertia = 0.f;
		bool autoCalcMass = false;
		bool isStatic = false;
		bool canRotate = true;
	};
}