#pragma once

#include <Windows.h>
#include "ImGui/imgui.h"

namespace Milk3D
{
	struct Editor
	{
	public:
		static void Initialize();
		static void Start();
		static void Render();
		static void Shutdown();
		static LRESULT CheckWndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};
}