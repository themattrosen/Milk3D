#include "GameObjectFactory.h"
#include "GameObjects\GameObject.h"
#include "GameObjectTypeInclude.h"

namespace Milk3D
{
	GameObject * Milk3D::GameObjectFactory::Create(unsigned typeID)
	{
#define REGISTER_GAMEOBJECT_TYPE(type) \
		if(type::GetGameObjectTypeID() == typeID)	\
			return new type;	\

#include "GameObjectTypeRegistry.h"

		else
			return new GameObject;

#undef REGISTER_GAMEOBJECT_TYPE
#define REGISTER_GAMEOBJECT_TYPE(type) 
	}

	unsigned GameObjectFactory::GetTypeID(const GameObject * obj)
	{
		const GameObjectTypeIDBase* b = 
			dynamic_cast<const GameObjectTypeIDBase*>(obj);
		if (!b) return INVALID_GAMEOBJECT_TYPEID;
		return b->GetTypeID();
	}

}
