#include "TextureAsset.h"
#include "Graphics\Core\Texture.h"

namespace Milk3D
{
	void TextureAsset::LoadInternal()
	{
		std::string path = GetDirectory() + "/" + GetName();
		m_texture = new Texture;
		m_texture->Initialize(path.c_str());
	}
	void TextureAsset::UnloadInternal()
	{
		delete m_texture;
	}
}
