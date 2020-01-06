#include "ShaderAsset.h"
#include "Graphics\Core\Shader.h"
namespace Milk3D
{
	void ShaderAsset::LoadInternal()
	{
		m_shader = new Shader;
		std::string path = GetDirectory() + "/" + GetName();
		m_shader->Initialize(path.c_str(), ShaderType::Pixel | ShaderType::Vertex);
	}

	void ShaderAsset::UnloadInternal()
	{
		delete m_shader;
	}

}