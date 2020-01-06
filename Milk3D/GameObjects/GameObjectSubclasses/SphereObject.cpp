#include "SphereObject.h"
#include "GameObjects\Components\ComponentInclude.h"

namespace Milk3D
{
	void SphereObject::OnCreate()
	{
		auto* gc = AddComponent<GraphicsComponent>();
		auto* pc = AddComponent<PhysicsComponent>();
		auto* body = pc->GetBody();
		body->movement->SetMaterialType(pmt_BouncyBall);
		body->movement->isStatic = false;
		body->collider->isTrigger = false;
	}

	void SphereObject::OnDelete()
	{
	}

}