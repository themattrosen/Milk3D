#include "MeshAsset.h"
#include "Graphics\Core\Model.h"

namespace Milk3D
{
	void MeshAsset::LoadInternal()
	{
		m_mesh = new Model;
		std::string path = GetDirectory() + "/" + GetName();
		m_mesh->Initialize(path.c_str());
	}

	void MeshAsset::UnloadInternal()
	{
		delete m_mesh;
	}
}
