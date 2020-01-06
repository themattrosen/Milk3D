#pragma once

#include "Asset.h"

namespace Milk3D
{
	class MaterialAsset : public Asset
	{
	public:
		MaterialAsset(const std::string& dir, const std::string& filename) :
			Asset(dir, filename, at_MaterialAsset)
		{}

		void LoadInternal() override;
		void UnloadInternal() override;
	private:

	};
}