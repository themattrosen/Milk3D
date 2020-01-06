#include "GraphicsComponent.h"
#include "Core\Events\EventCommon.h"
#include "Assets\AssetTypes\Asset.h"

namespace Milk3D
{

	void GraphicsComponent::OnActivate()
	{
		GetAssetEvent get;
		get.name = m_textureName;
		SendEvent(&get);
		m_texture = reinterpret_cast<TextureAsset*>(get.setAsset);

		get.name = m_meshName;
		SendEvent(&get);
		m_mesh = reinterpret_cast<MeshAsset*>(get.setAsset);

		get.name = m_shaderName;
		SendEvent(&get);
		m_shader = reinterpret_cast<ShaderAsset*>(get.setAsset);

		AddToSceneEvent add;
		add.object = GetParent();
		SendEvent(&add);
	}

	void GraphicsComponent::OnDeactivate()
	{
		RemoveFromSceneEvent remove;
		remove.object = GetParent();
		SendEvent(&remove);

		m_texture = nullptr;
		m_mesh = nullptr;
		m_shader = nullptr;
	}

	void GraphicsComponent::Serialize(Serializer & s)
	{
		s % m_textureName % 
			m_meshName % 
			m_shaderName %
			m_isCamera % 
			Serializer::Endline();
	}

	void GraphicsComponent::SetTexture(const std::string & name)
	{
		m_textureName = name;
		GetAssetEvent get;
		get.name = m_textureName;
		SendEvent(&get);
		m_texture = reinterpret_cast<TextureAsset*>(get.setAsset);
	}

	void GraphicsComponent::SetMesh(const std::string & name)
	{
		m_meshName = name;
		GetAssetEvent get;
		get.name = m_meshName;
		SendEvent(&get);
		m_mesh = reinterpret_cast<MeshAsset*>(get.setAsset);
	}

	void GraphicsComponent::SetShader(const std::string & name)
	{
		m_shaderName = name;
		GetAssetEvent get;
		get.name = m_shaderName;
		SendEvent(&get);
		m_shader = reinterpret_cast<ShaderAsset*>(get.setAsset);
	}

}