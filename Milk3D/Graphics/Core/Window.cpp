#include "Window.h"
#include <Windows.h>
#include <iostream>

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
static LONG GetWindowID(HWND hwnd);

namespace Milk3D
{
	HANDLE Window::console = nullptr;
	std::unordered_map<LONG, Window*> Window::windows;
	Window * Window::currentWindow = nullptr;
	Window * Window::mainWindow = nullptr;

	Window::Window(const char * title_, unsigned width_, unsigned height_, unsigned posX_, unsigned posY_, bool vsync_, bool fullscreen_) :
		windowName(title_), width(width_), height(height_), fullscreen(fullscreen_), posX(posX_), posY(posY_)
	{
		Initialize();
		if (windows.empty())
			mainWindow = this;
		windows.emplace(GetWindowID(handle), this);
		//graphicsDevice = GraphicsDevice::CreateGraphicsDevice(graphicsAPI_, handle, width_, height_, vsync_, fullscreen_, bufferCount, MSAA);
		SetMatrices(static_cast<float>(width_), static_cast<float>(height_));
	}

	Window::~Window()
	{
		//delete graphicsDevice;
		//graphicsDevice = nullptr;

		// Show the mouse cursor
		ShowCursor(true);

		// Fix the display settings if leaving full screen mode
		if (fullscreen)
		{
			ChangeDisplaySettings(NULL, 0);
		}

		// Remove the window.
		DestroyWindow(handle);
		handle = nullptr;

		// Remove the application instance.
		UnregisterClass(windowName, instance);
		instance = nullptr;

		FreeConsole();
		console = nullptr;
	}

	Window * Window::GetCurrentWindow()
	{
		return currentWindow;
	}

	Window * Window::GetMainWindow()
	{
		return mainWindow;
	}

	Window * Window::GetWindow(HWND hwnd)
	{
		LONG id = GetWindowID(hwnd);
		auto window = windows.find(id);
		if (window != windows.end())
			return window->second;
		return nullptr;
	}

	Window * Window::GetWindow(LONG ID)
	{
		auto window = windows.find(ID);
		if (window != windows.end())
			return window->second;
		return nullptr;
	}

	bool Window::Update()
	{
		MSG msg = { 0 };

		// Handle the windows messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application, exit
		if (msg.message == WM_QUIT)
		{
			return false;
		}

		return true;
	}

	void Window::SetFullscreen()
	{
		// Determine the resolution of the clients desktop screen
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);

		DEVMODE fullscreenSettings;

		EnumDisplaySettings(NULL, 0, &fullscreenSettings);
		fullscreenSettings.dmPelsWidth = width;
		fullscreenSettings.dmPelsHeight = height;
		fullscreenSettings.dmBitsPerPel = DM_BITSPERPEL;
		fullscreenSettings.dmDisplayFrequency = DM_DISPLAYFREQUENCY;
		fullscreenSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;

		SetWindowLongPtr(handle, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
		SetWindowLongPtr(handle, GWL_STYLE, WS_POPUP | WS_VISIBLE);
		SetWindowPos(handle, HWND_TOPMOST, 0, 0, width, height, SWP_SHOWWINDOW);
		bool failed = ChangeDisplaySettings(&fullscreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_FAILED;
		ShowWindow(handle, SW_MAXIMIZE);

		//graphicsDevice->Resize(width, height);

		if (failed)
		{
			std::cout << "Failed to set window to fullscreen." << std::endl;
			return;
		}
		//graphicsDevice->SetFullscreen(true);
		fullscreen = true;
	}

	void Window::SetWindowed(unsigned width_, unsigned height_)
	{
		width = width_;
		height = height_;

		LONG posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		LONG posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = width_;
		rect.bottom = height_;

		SetWindowLongPtr(handle, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
		MoveWindow(handle, posX, posY, width_, height_, TRUE);
		fullscreen = false;
		//graphicsDevice->SetFullscreen(false);
		//graphicsDevice->Resize(width_, height_);
	}

	void Window::SetWindowPosition(int windowX, int windowY, int flags)
	{
		SetWindowPos(handle, nullptr, windowX, windowY, 0, 0, flags);
	}

	void Window::SetTitle(const char * newTitle)
	{
		SetWindowText(handle, newTitle);
	}

	void Window::SetCursorDisplay(bool displayCursor)
	{
		ShowCursor(displayCursor);
	}

	void Window::SetIcon(const char * file, IconType iconType)
	{
		HANDLE icon = LoadImage(instance, file, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
		if (!icon)
			return;

		LPARAM param = reinterpret_cast<LPARAM>(icon);

		if (iconType == both)
		{
			SendMessage(handle, WM_SETICON, ICON_SMALL, param);
			SendMessage(handle, WM_SETICON, ICON_BIG, param);
		}
		else
			SendMessage(handle, WM_SETICON, iconType == iconBig ? ICON_BIG : ICON_SMALL, param);
	}

	void Window::SetMatrices(float width_, float height_, float fov, float nearDistance, float farDistance)
	{
		if (width_ == 0 || height_ == 0)
			return;

		fieldOfView = fov;
		nearPlane = nearDistance;
		farPlane = farDistance;

		float screenAspect = width_ / height_;

		// Create the projection matrix for 3D rendering
		projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fov, screenAspect, nearDistance, farDistance);

		// Create an orthographic projection matrix for 2D rendering
		orthoMatrix = DirectX::XMMatrixOrthographicLH(width_, height_, nearDistance, farDistance);
	}
	void Window::SetDimensions(unsigned width_, unsigned height_, bool resize)
	{
		width = width_;
		height = height_;

		SetMatrices(static_cast<float>(width_), static_cast<float>(height_), fieldOfView, nearPlane, farPlane);

		if (resize)
		{
			RECT rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = width;
			rect.bottom = height;
			SetWindowLongPtr(handle, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
			AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
			//graphicsDevice->Resize(width_, height_);
		}
	}
	void Window::GetDimensions(unsigned & width_, unsigned & height_)
	{
		width_ = width;
		height_ = height;
	}
	void Window::ShowMessageBox(const char * name, const char * message)
	{
		MessageBox(handle, name, message, MB_OK);
	}

	void Window::SetMinimized(bool minimized_)
	{
		minimized = minimized_;
	}

	//GraphicsDevice * Window::GetGraphicsDevice()
	//{
	//	return graphicsDevice;
	//}

	void Window::SetPosX(int posX_, bool moveWindow)
	{
		posX = posX_;
		if (moveWindow)
			SetWindowPosition(posX, posY);
	}
	void Window::SetPosY(int posY_, bool moveWindow)
	{
		posY = posY_;
		if (moveWindow)
			SetWindowPosition(posX, posY);
	}

	void Window::Render()
	{
		// Render here
		for (auto & window : windows)
		{
			currentWindow = window.second;
			//currentWindow->graphicsDevice->StartFrame();

			// Render here

			//currentWindow->graphicsDevice->EndFrame();
		}

		currentWindow = nullptr;
	}

	void Window::StartFrame()
	{
		currentWindow = this;
		//graphicsDevice->StartFrame();
	}
	void Window::EndFrame()
	{
		//graphicsDevice->EndFrame();
		currentWindow = nullptr;
	}

	//------------------------------------------------------------------------------
	// Private:
	//------------------------------------------------------------------------------

	void Window::Initialize()
	{
		// Get the instance of this application
		instance = GetModuleHandle(NULL);

		WNDCLASSEX wc;

		// Setup the windows class with default settings
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = instance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		wc.lpszMenuName = NULL;
		wc.lpszClassName = windowName;
		wc.cbSize = sizeof(WNDCLASSEX);

		// Register the window class.
		RegisterClassEx(&wc);

		// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
		if (fullscreen)
		{
			DEVMODE dmScreenSettings;

			// Determine the resolution of the clients desktop screen.
			width = GetSystemMetrics(SM_CXSCREEN);
			height = GetSystemMetrics(SM_CYSCREEN);

			// If full screen set the screen to maximum size of the users desktop and 32bit
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = width;
			dmScreenSettings.dmPelsHeight = height;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			// Change the display settings to full screen
			ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

			// Set the position of the window to the top left corner
			posX = 0;
			posY = 0;

			handle = CreateWindowEx(WS_EX_APPWINDOW, windowName, windowName,
				WS_CLIPSIBLINGS | WS_POPUP, posX, posY, width, height,
				NULL, NULL, instance, NULL);

		}
		else
		{
			handle = CreateWindowEx(WS_EX_APPWINDOW, windowName, windowName,
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_TILEDWINDOW | WS_POPUP,
				posX, posY, width, height, NULL, NULL, instance, NULL);
		}


		assert(handle);

		// Create console
		if (!console)
			console = CreateConsole();

		// Bring the window up on the screen and set it as main focus
		ShowWindow(handle, SW_SHOW);
		SetForegroundWindow(handle);
		SetFocus(handle);
		ShowCursor(true);
	}

	HANDLE Window::CreateConsole()
	{
#if 0
		if (!AllocConsole())
		{
			//std::cerr << "Failed to create console." << std::endl;
			return nullptr;
		}
		AttachConsole(GetCurrentProcessId());

		// Redirect cout and cerr
		freopen("CONOUT$", "wt", stdout);
		freopen("CONOUT$", "wt", stderr);
		freopen("CONIN$", "rt", stdin);
		SetConsoleTitle("Debug Console");
		std::ios::sync_with_stdio(1);
#endif

		return GetStdHandle(STD_OUTPUT_HANDLE);
	}
}

LONG GetWindowID(HWND hwnd)
{
	return GetWindowLong(hwnd, GWL_ID);
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}

LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	// TODO:
	//if (DX11::Editor::CheckWndProcHandler(hwnd, umsg, wparam, lparam))
	//	return true;

	switch (umsg)
	{
		// Check if a key has been pressed on the keyboard.
	case WM_KEYDOWN:
	{
		// If a key is pressed send it to the input object so it can record that state.
		if (static_cast<UINT8>(wparam) == VK_ESCAPE)
		{
			
		}

		return 0;
	}

	// Check if a key has been released on the keyboard.
	case WM_KEYUP:
	{
		// DX11::Input::KeyUp(static_cast<int>(wparam));
		return 0;
	}

	case WM_LBUTTONDOWN:
	{
		//DX11::Input::MouseLeftDown();
		return 0;
	}

	case WM_LBUTTONUP:
	{
		//DX11::Input::MouseLeftUp();
		return 0;
	}

	// Check if the window has resized
	case WM_SIZE:
	{
		if (wparam == SIZE_MINIMIZED)
		{
			// TODO: Handle minimized
			break;
		}

		unsigned newWidth = (LOWORD(lparam)), newHeight = HIWORD(lparam);

		// DX11::Window * window = DX11::Window::GetWindow(hwnd);

		//if (!window)
		//	break;

		if (wparam == SIZE_MAXIMIZED)
		{
			//window->SetDimensions(newWidth, newHeight, false);
			//if (!window->IsFullscreen())
			//	window->SetFullscreen();
		}
		else
		{
			//window->SetDimensions(newWidth, newHeight, true);
		}

		break;
	}

	case WM_MOVING:
	{
		// TODO: Pause while window moves
		//
		//DX11::Window * window = DX11::Window::GetWindow(hwnd);
		//if (!window) break;
		//RECT * r = reinterpret_cast<LPRECT>(lparam);
		//window->SetPosX(r->left, false);
		//window->SetPosY(r->top, false);
		break;
	}

	case WM_KILLFOCUS:
	{
		// Pause
		break;
	}
	case WM_SETFOCUS:
	{
		// Unpause
		break;
	}
	}
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}
