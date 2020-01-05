#pragma once
#include <Core\Math\Math.h>

namespace Milk3D
{
	enum ShapeType
	{
		st_None = 0,
		st_Sphere,
		st_Box,

		st_Count
	};

	struct Collider
	{
		struct Body* parent = nullptr;
		bool isTrigger = false;
		bool justCollided = false;
		ShapeType type = st_None;
		Vec3 offset = {};

		Collider(Body* p, ShapeType st) :
			parent(p), type(st)
		{}
	};

	struct SphereCollider : public Collider
	{
		SphereCollider(Body* p, float r = 1.f) : 
			Collider(p, st_Sphere), radius(r) {}

		float radius;
	};

	struct BoxCollider : public Collider
	{
		BoxCollider(Body* p) :
			Collider(p, st_Box)
		{}

		Vec3 min, max;
	};
}