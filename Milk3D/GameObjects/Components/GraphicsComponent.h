#pragma once

#include "Component.h"
#include "Graphics\Core\Material.h"
#include <string>

namespace Milk3D
{
	class TextureAsset;
	class MeshAsset;
	class ShaderAsset;

	class GraphicsComponent : public ComponentType<GraphicsComponent>
	{
	public:
		GraphicsComponent(GameObject* parent) :
			ComponentType<GraphicsComponent>(parent)
		{}

		void OnActivate() override;
		void OnDeactivate() override;

		void Serialize(Serializer& s) override;

		TextureAsset* GetTexture() { return m_texture; }
		MeshAsset* GetMesh() { return m_mesh; }

		ShaderAsset* GetShader() { return m_shader; }

		Material& GetMaterial() { return m_material; }

		void SetTexture(const std::string& name);
		void SetMesh(const std::string& name);
		void SetShader(const std::string& name);

		bool IsCamera() const { return m_isCamera; }
		void SetIsCamera(bool cam) { m_isCamera = cam; }

	private:
		std::string m_textureName = "";
		std::string m_meshName = "";
		std::string m_shaderName = "";
		TextureAsset* m_texture = nullptr;
		MeshAsset* m_mesh = nullptr;
		ShaderAsset* m_shader = nullptr;
		Material m_material;
		bool m_isCamera = false;
	};
}