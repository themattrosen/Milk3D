#pragma once

#include "Asset.h"

namespace Milk3D
{
	class MeshAsset : public Asset
	{
	public:
		MeshAsset(const std::string& dir, const std::string& filename) :
			Asset(dir, filename, at_MeshAsset)
		{}

		void LoadInternal() override;
		void UnloadInternal() override;

		class Model* GetMesh() { return m_mesh; }

	private:
		class Model* m_mesh;
	};
}