#pragma once

#include "Asset.h"

namespace Milk3D
{
	class LevelAsset : public Asset
	{
	public:
		LevelAsset(const std::string& dir, const std::string& filename) :
			Asset(dir, filename, at_LevelAsset)
		{}

		void LoadInternal() override;
		void UnloadInternal() override;
	private:

	};
}