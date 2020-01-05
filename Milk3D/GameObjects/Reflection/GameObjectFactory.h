#pragma once

namespace Milk3D
{
	class GameObjectFactory
	{
	public:
		static class GameObject* Create(unsigned typeID);
		static unsigned GetTypeID(const GameObject* obj);
	private:

	};
}
