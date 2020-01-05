#include "Input.h"
#include "Graphics/Core/Window.h"

namespace Milk3D
{
	void Input::KeyUp(UINT8 key)
	{
		InputManager & input = InputManager::GetInstance();
		input.pressed[key] = false;
		input.released[key] = true;
		input.alreadyPressed[key] = false;
	}
	void Input::KeyDown(UINT8 key)
	{
		InputManager & input = InputManager::GetInstance();
		input.pressed[key] = true;
		input.released[key] = false;
	}

	bool Input::Pressed(UINT8 key)
	{
		return InputManager::GetInstance().pressed[key];
	}
	bool Input::Triggered(UINT8 key)
	{
		InputManager & input = InputManager::GetInstance();
		bool & alreadyPressed = input.alreadyPressed[key];
		if (Pressed(key) && !alreadyPressed)
		{
			alreadyPressed = true;
			return true;
		}
		return false;
	}
	bool Input::Released(UINT8 key)
	{
		bool & released = InputManager::GetInstance().released[key];
		if (released)
			released = false;
		return released;
	}

	bool Input::LeftMouseDown()
	{
		return Pressed(VK_MOUSE_LEFT);
	}
	bool Input::RightMouseDown()
	{
		return Pressed(VK_MOUSE_RIGHT);
	}

	Vec2 Input::GetMousePosition()
	{
		Vec2 mousePos;
		POINT cursorPos;
		if (GetCursorPos(&cursorPos))
		{
			Window * window = Window::GetMainWindow();
			mousePos.Set(static_cast<float>(cursorPos.x - window->GetPosX()), static_cast<float>(cursorPos.x - window->GetPosY()));
		}
		return mousePos;
	}
}