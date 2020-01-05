#pragma once
#include <GameObjects\Reflection\GameObjectTypeID.h>
#include <GameObjects\GameObject.h>
#include "Graphics/Core/Mesh.h"
#include "Graphics/Core/Texture.h"
#include "Graphics/Core/Shader.h"

namespace Milk3D
{
	class Actor : 
		public GameObject,
		public GameObjectTypeID<Actor>
	{
	public:
		virtual void OnUpdate(float dt) override;
		void OnRemoveChild(GameObjectID child, int index) override;
		void OnAddChild(GameObjectID child) override;

		void Serialize(Serializer& s) override;

		void SetMesh(Mesh * mesh) { m_mesh = mesh; }
		void SetTexture(Texture * texture) { m_texture = texture; }
		void SetShader(Shader * shader) { m_shader = shader; }

		Mesh * GetMesh() { return m_mesh; }
		Texture * GetTexture() { return m_texture; }
		Shader * GetShader() { return m_shader; }

	private:
		Mesh * m_mesh = nullptr;
		Texture * m_texture = nullptr;
		Shader * m_shader = nullptr;
	};
}