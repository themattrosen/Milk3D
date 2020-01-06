#pragma once
#include "Core\System\System.h"
#include <string>
#include <unordered_map>

namespace Milk3D
{
	class AssetManager : public System
	{
	public:

		void OnEvent(SystemInitEvent* e) override;
		void OnEvent(SystemExitEvent* e) override;
		void OnEvent(SystemUpdateEvent* e) override;
		void OnEvent(RefreshAssetManagerEvent* e);
		void OnEvent(GetAssetEvent* e);

		template<typename AssetDerived>
		AssetDerived* GetAsset(const std::string& name)
		{
			auto iter = m_assets.find(name);
			if (iter == m_assets.end())
				return nullptr;

			return static_cast<AssetDerived*>(iter->second);
		}
	private:
		// stored by filename + extension
		std::unordered_map<std::string, class Asset*> m_assets;
		std::string m_rootDir = "Assets\\";
	};
}