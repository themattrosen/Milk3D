#pragma once

#include "Scene.h"

namespace Milk3D
{
	#define MAX_LIGHTS 8

	class ForwardRenderer
	{
		public:
			ForwardRenderer() = default;

			static void Initialize();

			static Scene * AddScene();
			static void Render(Scene * scene);
			static void RenderAll();

			static void Shutdown();

		private:
			static std::vector<Scene> s_scenes;
			struct LightBuffer
			{
				Light lights[MAX_LIGHTS];
				unsigned int activeLights = 0;
				DirectX::XMFLOAT3 padding;
			};
			static LightBuffer m_lightBuffer;

			struct SceneBuffer
			{
				DirectX::XMFLOAT3 cameraPosition;
				float padding = 0.0f;
			};
			static SceneBuffer m_sceneBuffer;
	};
}