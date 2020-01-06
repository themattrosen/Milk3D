#pragma once
#include "GameObjects\GameObject.h"
#include "GameObjects\Reflection\GameObjectTypeID.h"


namespace Milk3D
{
	class SphereObject :
		public GameObject,
		public GameObjectTypeID<SphereObject>
	{
	public:
		
		void OnCreate() override;
		void OnDelete() override;

	private:

	};
}
