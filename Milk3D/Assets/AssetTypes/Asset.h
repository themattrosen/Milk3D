#pragma once
#include <string>

namespace Milk3D
{
#define REGISTER_ASSET(type, ...) at_##type , 
	enum AssetType
	{
#include "AssetTypeRegistry.h"
		at_Count
	};
#undef REGISTER_ASSET
#define REGISTER_ASSET(type, ...) 

	class Asset
	{
	public:
		Asset(const std::string& dir, const std::string& filename, AssetType type) :
			m_dir(dir),
			m_name(filename),
			m_type(type),
			m_isLoaded(false)
		{}

		virtual ~Asset() {}

		const std::string& GetName() const { return m_name; }
		const std::string& GetDirectory() const { return m_dir; }
		AssetType GetType() const { return m_type; }

		bool IsLoaded() const { return m_isLoaded; }

		void Load()
		{
			if (!IsLoaded())
			{
				LoadInternal();
				m_isLoaded = true;
			}
		}
		void Unload()
		{
			if (IsLoaded())
			{
				UnloadInternal();
				m_isLoaded = false;
			}
		}

		virtual void LoadInternal() = 0;
		virtual void UnloadInternal() = 0;

	private:
		std::string m_dir;
		std::string m_name;
		AssetType m_type;
		bool m_isLoaded;

		friend class Serializer;
	};
}