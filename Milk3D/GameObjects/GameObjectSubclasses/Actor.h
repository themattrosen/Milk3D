#pragma once
#include <GameObjects\Reflection\GameObjectTypeID.h>
#include <GameObjects\GameObject.h>

namespace Milk3D
{
	class Actor : 
		public GameObject,
		public GameObjectTypeID<Actor>
	{
	public:
		virtual void OnUpdate(float dt) override;
		void OnRemoveChild(GameObjectID child, int index) override;
		void OnAddChild(GameObjectID child) override;

		void Serialize(Serializer& s) override;

	private:
		

	};
}