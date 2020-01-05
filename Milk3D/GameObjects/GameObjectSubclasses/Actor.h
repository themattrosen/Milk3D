#pragma once
#include <GameObjects\Reflection\GameObjectTypeID.h>
#include <GameObjects\GameObject.h>
#include "Graphics/Core/Mesh.h"
#include "Graphics/Core/Material.h"
#include "Graphics/Core/Shader.h"
#include "Physics\Body.h"

namespace Milk3D
{
	class Actor : 
		public GameObject,
		public GameObjectTypeID<Actor>
	{
	public:
		virtual void OnUpdate(float dt) override;
		void OnActivate() override;
		void OnDeactivate() override;
		void OnRemoveChild(GameObjectID child, int index) override;
		void OnAddChild(GameObjectID child) override;

		void Serialize(Serializer& s) override;

		void SetMesh(Mesh * mesh) { m_mesh = mesh; }
		void SetMaterial(Material const & material) { m_material = material; }
		void SetShader(Shader * shader) { m_shader = shader; }

		Mesh * GetMesh() { return m_mesh; }
		Material & GetMaterial() { return m_material; }
		Shader * GetShader() { return m_shader; }
		Body* GetBody() { return m_body; }

	private:
		Mesh * m_mesh = nullptr;
		Material m_material;
		Shader * m_shader = nullptr;
		Body* m_body = nullptr;
	};
}