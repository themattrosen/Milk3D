#include "Actor.h"

namespace Milk3D
{
	void Actor::OnUpdate(float dt)
	{
	}

	void Actor::OnActivate()
	{
		AddToPhysicsEvent add;
		add.parent = this;
		SendEvent(&add);
		m_body = add.setBody;
	}

	void Actor::OnDeactivate()
	{
		RemoveFromPhysicsEvent remove;
		remove.parent = this;
		SendEvent(&remove);
	}

	void Actor::OnRemoveChild(GameObjectID child, int index)
	{
	}

	void Actor::OnAddChild(GameObjectID child)
	{
	}

	void Actor::Serialize(Serializer & s)
	{
	}
}
