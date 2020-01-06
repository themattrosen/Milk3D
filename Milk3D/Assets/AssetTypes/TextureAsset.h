#pragma once

#include "Asset.h"

namespace Milk3D
{
	class TextureAsset : public Asset
	{
	public:
		TextureAsset(const std::string& dir, const std::string& filename) :
			Asset(dir, filename, at_TextureAsset)
		{}

		void LoadInternal() override;
		void UnloadInternal() override;

		class Texture* GetTexture() { return m_texture; }
	private:
		Texture* m_texture;
	};
}