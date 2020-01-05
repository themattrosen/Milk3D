#pragma once

namespace Milk3D
{
	class GameObjectTypeIDBase
	{
	public:
		virtual unsigned GetTypeID() const = 0;
	protected:
		static unsigned s_typeIDBase;
	};

	template<typename T>
	class GameObjectTypeID : public GameObjectTypeIDBase
	{
	public:
		unsigned GetTypeID() const { return s_typeID; }
		static const unsigned GetGameObjectTypeID() { return s_typeID; }

	private:
		const static unsigned s_typeID;
	};

	template<typename T>
	const unsigned GameObjectTypeID<T>::s_typeID = GameObjectTypeIDBase::s_typeIDBase++;

	const constexpr unsigned INVALID_GAMEOBJECT_TYPEID = (unsigned)(-1);
}
