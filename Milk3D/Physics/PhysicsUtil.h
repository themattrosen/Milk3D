#pragma once
#include <Core\Math\Math.h>
#include "Collider.h"

namespace Milk3D
{
	using Point = Vec3;
	
	struct Manifold
	{
		struct Body* A;
		struct Body* B;
		float penetration;
		Vec3 normal;
		Vec3 contactPoint;

	};

	bool NoneNone(Manifold&m, const Collider* a, const Collider* b);
	bool NoneBox(Manifold&m, const Collider* a, const Collider* b);
	bool NoneSphere(Manifold&m, const Collider* a, const Collider* b);
	bool BoxNone(Manifold&m, const Collider* a, const Collider* b);
	bool SphereNone(Manifold&m, const Collider* a, const Collider* b);

	bool SphereSphere(Manifold&m, const Collider* a, const Collider* b);
	bool BoxBox(Manifold&m, const Collider* a, const Collider* b);
	bool SphereBox(Manifold&m, const Collider* a, const Collider* b);
	bool BoxSphere(Manifold&m, const Collider* a, const Collider* b);
	

	using CollisionTestFunction = bool(*)(Manifold&m, const Collider* a, const Collider* b);
	const CollisionTestFunction collisionTable[st_Count][st_Count] = 
	{
		{ NoneNone, NoneSphere, NoneBox, },
		{ SphereNone, SphereSphere, SphereBox, },
		{ BoxNone, BoxSphere, BoxBox, }, 
	};
}