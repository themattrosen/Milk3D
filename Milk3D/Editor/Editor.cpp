#include "Editor.h"
#include "ImGui/imgui_impl_dx11.h"
#include "Graphics/Core/GraphicsDevice.h"
#include "Graphics/Core/Window.h"

// Implemented by imgui_impl_win32.cpp
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#include "ImGui/imgui_impl_win32.h"

namespace Milk3D
{
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