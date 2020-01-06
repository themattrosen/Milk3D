#include "Editor.h"
#include "ImGui/imgui_impl_dx11.h"
#include "Graphics/Core/GraphicsDevice.h"
#include "Graphics/Core/Window.h"
#include "Core\Events\EventCommon.h"
#include "GameObjects\GameObjectSubclasses\SphereObject.h"
#include "GameObjects\Components\ComponentInclude.h"

#include <random>

// Implemented by imgui_impl_win32.cpp
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#include "ImGui/imgui_impl_win32.h"

namespace Milk3D
{
	static std::default_random_engine generator;
	static std::uniform_int_distribution<int> posGen(-10, 10);
	static std::uniform_real_distribution<float> sizeGen(0.5f, 3.f);
	//static int dice_roll = distribution(generator);  // generates number in the range 1..6


	LRESULT Editor::CheckWndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
	}
	void Editor::Initialize()
	{
		auto mainWindow = Window::GetMainWindow();
		if (!mainWindow) return;

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO();

		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(mainWindow->GetHandle());

		GraphicsDevice & gd = GraphicsDevice::GetInstance();
		ImGui_ImplDX11_Init(gd.GetDevice(), gd.GetDeviceContext());
	}
	void Editor::Start()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void Editor::Update()
	{
		ImGui::Begin("Test");

		ImGui::Text("Hello world!");
		if (ImGui::Button("Create Sphere"))
		{
			CreateGameObjectEvent create;
			create.parentID = INVALID_GAMEOBJECTID;
			GameObjectID newID;
			create.obj = new SphereObject;
			create.setID = &newID;

			float x = posGen(generator);
			float y = posGen(generator);
			float z = posGen(generator);
			create.obj->GetTransform().position = Vec3(x, y, z);
			x = sizeGen(generator);
			y = sizeGen(generator);
			z = sizeGen(generator);
			create.obj->GetTransform().scale = Vec3(x);

			SendEvent(&create);

			GetGameObjectEvent get;
			get.id = *create.setID;
			SendEvent(&get);

			auto* gcomponent = get.setObj->GetComponent<GraphicsComponent>();
			gcomponent->SetMesh("Sphere.obj");
			gcomponent->SetTexture("Image2.png");
			gcomponent->SetShader("Phong.hlsl");
			AddToSceneEvent add;
			add.object = get.setObj;
			SendEvent(&add);
		}

		ImGui::End();
	}

	void Editor::Render()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	void Editor::Shutdown()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

}