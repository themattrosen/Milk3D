#include "PhysicsUtil.h"
#include "Collider.h"
#include "Body.h"

namespace Milk3D
{

	bool NoneNone(Manifold & m, const Collider * a, const Collider * b)
	{
		return false;
	}
	bool NoneBox(Manifold & m, const Collider * a, const Collider * b)
	{
		return false;
	}
	bool NoneSphere(Manifold & m, const Collider * a, const Collider * b)
	{
		return false;
	}
	bool BoxNone(Manifold & m, const Collider * a, const Collider * b)
	{
		return false;
	}
	bool SphereNone(Manifold & m, const Collider * a, const Collider * b)
	{
		return false;
	}

	bool SphereSphere(Manifold & m, const Collider * a, const Collider * b)
	{
		const SphereCollider* sa = reinterpret_cast<const SphereCollider*>(a);
		const SphereCollider* sb = reinterpret_cast<const SphereCollider*>(b);
		float r = sa->radius + sb->radius;
		r *= r;
		Vec3 bpos = sb->parent->transform->position + sb->offset;
		Vec3 apos = sa->parent->transform->position + sa->offset;
		Vec3 n = bpos - apos;

		if ((n.X() * n.X() + n.Y() * n.Y() + n.Z() * n.Z()) > r) return false;

		float d = n.Length();
		// might need this
		//r /= r;
		if (d != 0)
		{
			m.penetration = r - d;
			n.Normalize();
			m.normal = n;
		}
		else
		{
			m.penetration = sa->radius;
			m.normal = Vec3(1, 0, 0);
		}

		return true;
	}

	bool BoxBox(Manifold & m, const Collider * a, const Collider * b)
	{
		/*
		const BoxCollider* sa = reinterpret_cast<const BoxCollider*>(a);
		const BoxCollider* sb = reinterpret_cast<const BoxCollider*>(b);

		Vec3 bpos = sb->parent->transform->position + sb->offset;
		Vec3 apos = sa->parent->transform->position + sa->offset;
		Vec3 n = bpos - apos;

		float a_extent = (sa->max.X() - sa->min.X()) / 2.f;
		float b_extent = (sb->max.X() - sb->min.X()) / 2.f;

		float xoverlap = a_extent + b_extent - fabs(n.X());

		if (xoverlap > 0)
		{
			float ayextent = (sa->max.Y() - sa->min.Y()) / 2.f;
			float byextent = (sb->max.Y() - sb->min.Y()) / 2.f;

			float yoverlap = ayextent + byextent - fabs(n.Y());
			if (yoverlap > 0)
			{
				float azextent = (sa->max.Z() - sa->min.Z()) / 2.f;
				float bzextent = (sb->max.Z() - sb->min.Z()) / 2.f;
				float zoverlap = azextent + bzextent - abs(n.Z());

				if (zoverlap > 0)
				{

				}
			}
		}
		*/
		return false;
	}

	bool SphereBox(Manifold & m, const Collider * a, const Collider * b)
	{
		return false;
	}

	bool BoxSphere(Manifold & m, const Collider * a, const Collider * b)
	{
		return SphereBox(m, b, a);
	}


}