#pragma once

#include "Asset.h"

namespace Milk3D
{
	class Shader;

	class ShaderAsset : public Asset
	{
	public:
		ShaderAsset(const std::string& dir, const std::string& filename) :
			Asset(dir, filename, at_ShaderAsset)
		{}

		void LoadInternal() override;
		void UnloadInternal() override;

		Shader* GetShader() { return m_shader; }
	private:
		Shader* m_shader;
	};
}