#include "AssetManager.h"
#include "AssetTypes\AssetTypeInclude.h"
#include <filesystem>

namespace fs = std::experimental::filesystem;

namespace Milk3D
{
	static bool HasExtension(const std::string& filename, 
		const std::vector<std::string>& extensions)
	{
		std::string ext;
		auto pind = filename.find_last_of('.');
		if (pind == std::string::npos)
			ext = filename;
		else
			ext = filename.substr(pind);

		for (const auto& next : extensions)
		{
			if (next == ext)
				return true;
		}

		return false;
	}

	void AssetManager::OnEvent(SystemInitEvent * e)
	{
		for (const auto& entry : fs::recursive_directory_iterator(m_rootDir))
		{
			if (fs::is_regular_file(entry.path()))
			{
				std::string filename = entry.path().filename().string();
				std::string dir = entry.path().parent_path().string();

#undef REGISTER_ASSET
#define REGISTER_ASSET(type, ...)	\
				if(HasExtension(filename, {__VA_ARGS__}))	\
				{	\
					type* t = new type(dir, filename);	\
					t->Load();	\
					m_assets.insert_or_assign(filename, t);	\
					continue;	\
				}
#include "AssetTypes\AssetTypeRegistry.h"
#undef REGISTER_ASSET
#define REGISTER_ASSET(type, ...) 
			}
		}

		Subscribe(et_RefreshAssetManagerEvent);
		Subscribe(et_GetAssetEvent);
	}

	void AssetManager::OnEvent(SystemExitEvent * e)
	{
		for (auto& pair : m_assets)
		{
			pair.second->Unload();
			delete pair.second;
		}
		m_assets.clear();
	}

	void AssetManager::OnEvent(SystemUpdateEvent * e)
	{

	}

	void AssetManager::OnEvent(RefreshAssetManagerEvent * e)
	{
		for (const auto& entry : fs::recursive_directory_iterator(m_rootDir))
		{
			if (fs::is_regular_file(entry.path()))
			{
				std::string filename = entry.path().filename().string();
				std::string dir = entry.path().parent_path().string();
				auto iter = m_assets.find(filename);
				if (iter == m_assets.end())
				{
#undef REGISTER_ASSET
#define REGISTER_ASSET(type, ...)	\
				if(HasExtension(filename, {__VA_ARGS__}))	\
				{	\
					type* t = new type(dir, filename);	\
					m_assets.insert_or_assign(filename, t);	\
					continue;	\
				}
#include "AssetTypes\AssetTypeRegistry.h"
#undef REGISTER_ASSET
#define REGISTER_ASSET(type, ...) 
				}
				else
				{
					iter->second->Unload();
					iter->second->Load();
				}
			}
		}
	}

	void AssetManager::OnEvent(GetAssetEvent * e)
	{
		auto iter = m_assets.find(e->name);
		if (iter == m_assets.end())
			e->setAsset = nullptr;
		else
			e->setAsset = iter->second;
	}

}
