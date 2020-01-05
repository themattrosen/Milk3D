#pragma once

#include "Graphics/Core/Camera.h"
#include "Graphics/Core/Light.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/GameObjectSubclasses/Actor.h"
#include <vector>

namespace Milk3D
{
	class Scene
	{
		public:

			Camera & GetCamera() { return m_camera; }
			void AddLight(Light const & light) { m_lights.push_back(light); }
			std::vector<GameObject*> const & GetObjects() { return m_objects; }
			std::vector<Light> & GetLights() { return m_lights; }
			// TOOD: std::move?
			void SetObjects(std::vector<GameObject*> const & objects) 
			{ 
				m_objects = std::move(objects);
			}
		private:
			Camera m_camera;
			std::vector<GameObject*> m_objects;
			std::vector<Light> m_lights;
	};
}