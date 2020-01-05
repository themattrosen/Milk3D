#include "Window.h"
#include <Windows.h>
#include <iostream>

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
static LONG GetWindowID(HWND hwnd);

namespace Milk3D
{
	HANDLE Window::s_console = nullptr;
	std::unordered_map<LONG, Window*> Window::s_windows;
	Window * Window::s_currentWindow = nullptr;
	Window * Window::s_mainWindow = nullptr;

	Window::Window(const char * title_, unsigned width_, unsigned height_, unsigned posX_, unsigned posY_, bool fullscreen_) :
		m_windowName(title_), m_width(width_), m_height(height_), m_fullscreen(fullscreen_), m_posX(posX_), m_posY(posY_)
	{
		Initialize();
		if (s_windows.empty())
			s_mainWindow = this;
		s_windows.emplace(GetWindowID(m_handle), this);
		SetMatrices(static_cast<float>(width_), static_cast<float>(height_));
	}

	Window::~Window()
	{
		//delete graphicsDevice;
		//graphicsDevice = nullptr;

		// Show the mouse cursor
		ShowCursor(true);

		// Fix the display settings if leaving full screen mode
		if (m_fullscreen)
		{
			ChangeDisplaySettings(NULL, 0);
		}

		// Remove the window.
		DestroyWindow(m_handle);
		m_handle = nullptr;

		// Remove the application instance.
		UnregisterClass(m_windowName, m_instance);
		m_instance = nullptr;

		FreeConsole();
		s_console = nullptr;
	}

	Window * Window::GetCurrentWindow()
	{
		return s_currentWindow;
	}

	Window * Window::GetMainWindow()
	{
		return s_mainWindow;
	}

	Window * Window::GetWindow(HWND hwnd)
	{
		LONG id = GetWindowID(hwnd);
		auto window = s_windows.find(id);
		if (window != s_windows.end())
			return window->second;
		return nullptr;
	}

	Window * Window::GetWindow(LONG ID)
	{
		auto window = s_windows.find(ID);
		if (window != s_windows.end())
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
		m_width = GetSystemMetrics(SM_CXSCREEN);
		m_height = GetSystemMetrics(SM_CYSCREEN);

		DEVMODE fullscreenSettings;

		EnumDisplaySettings(NULL, 0, &fullscreenSettings);
		fullscreenSettings.dmPelsWidth = m_width;
		fullscreenSettings.dmPelsHeight = m_height;
		fullscreenSettings.dmBitsPerPel = DM_BITSPERPEL;
		fullscreenSettings.dmDisplayFrequency = DM_DISPLAYFREQUENCY;
		fullscreenSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;

		SetWindowLongPtr(m_handle, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
		SetWindowLongPtr(m_handle, GWL_STYLE, WS_POPUP | WS_VISIBLE);
		SetWindowPos(m_handle, HWND_TOPMOST, 0, 0, m_width, m_height, SWP_SHOWWINDOW);
		bool failed = ChangeDisplaySettings(&fullscreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_FAILED;
		ShowWindow(m_handle, SW_MAXIMIZE);

		//graphicsDevice->Resize(width, height);

		if (failed)
		{
			std::cout << "Failed to set window to fullscreen." << std::endl;
			return;
		}
		//graphicsDevice->SetFullscreen(true);
		m_fullscreen = true;
	}

	void Window::SetWindowed(unsigned width_, unsigned height_)
	{
		m_width = width_;
		m_height = height_;

		LONG posX = (GetSystemMetrics(SM_CXSCREEN) - m_width) / 2;
		LONG posY = (GetSystemMetrics(SM_CYSCREEN) - m_height) / 2;

		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = width_;
		rect.bottom = height_;

		SetWindowLongPtr(m_handle, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
		MoveWindow(m_handle, posX, posY, width_, height_, TRUE);
		m_fullscreen = false;
		//graphicsDevice->SetFullscreen(false);
		//graphicsDevice->Resize(width_, height_);
	}

	void Window::SetWindowPosition(int windowX, int windowY, int flags)
	{
		SetWindowPos(m_handle, nullptr, windowX, windowY, 0, 0, flags);
	}

	void Window::SetTitle(const char * newTitle)
	{
		SetWindowText(m_handle, newTitle);
	}

	void Window::SetCursorDisplay(bool displayCursor)
	{
		ShowCursor(displayCursor);
	}

	void Window::SetIcon(const char * file, IconType iconType)
	{
		HANDLE icon = LoadImage(m_instance, file, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
		if (!icon)
			return;

		LPARAM param = reinterpret_cast<LPARAM>(icon);

		if (iconType == both)
		{
			SendMessage(m_handle, WM_SETICON, ICON_SMALL, param);
			SendMessage(m_handle, WM_SETICON, ICON_BIG, param);
		}
		else
			SendMessage(m_handle, WM_SETICON, iconType == iconBig ? ICON_BIG : ICON_SMALL, param);
	}

	void Window::SetMatrices(float width_, float height_, float fov, float nearDistance, float farDistance)
	{
		if (width_ == 0 || height_ == 0)
			return;

		m_fieldOfView = fov;
		m_nearPlane = nearDistance;
		m_farPlane = farDistance;

		float screenAspect = width_ / height_;

		// Create the projection matrix for 3D rendering
		m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fov, screenAspect, nearDistance, farDistance);

		// Create an orthographic projection matrix for 2D rendering
		m_orthoMatrix = DirectX::XMMatrixOrthographicLH(width_, height_, nearDistance, farDistance);
	}
	void Window::SetDimensions(unsigned width_, unsigned height_, bool resize)
	{
		m_width = width_;
		m_height = height_;

		SetMatrices(static_cast<float>(width_), static_cast<float>(height_), m_fieldOfView, m_nearPlane, m_farPlane);

		if (resize)
		{
			RECT rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = m_width;
			rect.bottom = m_height;
			SetWindowLongPtr(m_handle, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
			AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
			//graphicsDevice->Resize(width_, height_);
		}
	}
	void Window::GetDimensions(unsigned & width_, unsigned & height_)
	{
		width_ = m_width;
		height_ = m_height;
	}
	void Window::ShowMessageBox(const char * name, const char * message)
	{
		MessageBox(m_handle, name, message, MB_OK);
	}

	void Window::SetMinimized(bool minimized_)
	{
		m_minimized = minimized_;
	}

	//GraphicsDevice * Window::GetGraphicsDevice()
	//{
	//	return graphicsDevice;
	//}

	void Window::SetPosX(int posX_, bool moveWindow)
	{
		m_posX = posX_;
		if (moveWindow)
			SetWindowPosition(m_posX, m_posY);
	}
	void Window::SetPosY(int posY_, bool moveWindow)
	{
		m_posY = posY_;
		if (moveWindow)
			SetWindowPosition(m_posX, m_posY);
	}

	//------------------------------------------------------------------------------
	// Private:
	//------------------------------------------------------------------------------

	void Window::Initialize()
	{
		// Get the instance of this application
		m_instance = GetModuleHandle(NULL);

		WNDCLASSEX wc;

		// Setup the windows class with default settings
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_instance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		wc.lpszMenuName = NULL;
		wc.lpszClassName = m_windowName;
		wc.cbSize = sizeof(WNDCLASSEX);

		// Register the window class.
		RegisterClassEx(&wc);

		// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
		if (m_fullscreen)
		{
			DEVMODE dmScreenSettings;

			// Determine the resolution of the clients desktop screen.
			m_width = GetSystemMetrics(SM_CXSCREEN);
			m_height = GetSystemMetrics(SM_CYSCREEN);

			// If full screen set the screen to maximum size of the users desktop and 32bit
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = m_width;
			dmScreenSettings.dmPelsHeight = m_height;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			// Change the display settings to full screen
			ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

			// Set the position of the window to the top left corner
			m_posX = 0;
			m_posY = 0;

			m_handle = CreateWindowEx(WS_EX_APPWINDOW, m_windowName, m_windowName,
				WS_CLIPSIBLINGS | WS_POPUP, m_posX, m_posY, m_width, m_height,
				NULL, NULL, m_instance, NULL);

		}
		else
		{
			m_handle = CreateWindowEx(WS_EX_APPWINDOW, m_windowName, m_windowName,
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_TILEDWINDOW | WS_POPUP,
				m_posX, m_posY, m_width, m_height, NULL, NULL, m_instance, NULL);
		}


		assert(m_handle);

		// Create console
		if (!s_console)
			s_console = CreateConsole();

		// Bring the window up on the screen and set it as main focus
		ShowWindow(m_handle, SW_SHOW);
		SetForegroundWindow(m_handle);
		SetFocus(m_handle);
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
