#pragma once

#include <Windows.h>
#include "Core/Math/Math.h"
#include "Graphics/Headers/GraphicsCommon.h"
#include "Graphics/Headers/DXMath.h"
#include <unordered_map>

namespace Milk3D
{
	enum IconType
	{
		iconSmall,
		iconBig,
		both
	};

	class Window
	{
	public:
		Window(const char * title, unsigned width, unsigned height, unsigned posX, unsigned posY, bool fullscreen);
		~Window();

		static Window * GetCurrentWindow();
		static Window * GetMainWindow();
		static Window * GetWindow(HWND handle);
		static Window * GetWindow(LONG ID);
		static bool Update();

		void SetFullscreen();
		void SetTitle(const char * newTitle);
		void SetWindowed(unsigned width, unsigned height);
		void SetCursorDisplay(bool displayCursor);
		void SetIcon(const char * file, IconType iconType);
		void SetDimensions(unsigned width, unsigned height, bool resize = true);
		void SetMatrices(float width, float height, float fov = DirectX::XM_PIDIV4, float nearDistance = 0.1f, float farDistance = 1000.0f);
		void GetDimensions(unsigned & width, unsigned & height);
		void ShowMessageBox(const char * name, const char * message);
		void SetMinimized(bool minimized);

		void SetWindowPosition(int windowX, int windowY, int flags = -1);

		void SetPosX(int posX, bool moveWindow = true);
		void SetPosY(int posY, bool moveWindow = true);

		//GraphicsDevice * GetGraphicsDevice();

		bool IsFullscreen() const { return m_fullscreen; }
		unsigned GetWidth() const { return m_width; }
		unsigned GetHeight() const { return m_height; }
		int GetPosX() const { return m_posX; }
		int GetPosY() const { return m_posY; }
		HWND GetHandle() { return m_handle; }
		Matrix const & GetProjection() const { return m_projectionMatrix; }
		Matrix const & GetOrtho() const { return m_orthoMatrix; }

		float GetFarPlane() const { return m_farPlane; }
		float GetNearPlane() const { return m_nearPlane; }
		float GetFOV() const { return m_fieldOfView; }

		// Don't allow copying of a window
		DELETE_COPY(Window);

	private:
		void Initialize();
		unsigned m_width = 0;
		unsigned m_height = 0;
		bool m_fullscreen = false;
		bool m_minimized = false;

		//! Projection matrix used in 3D rendering
		Matrix m_projectionMatrix;

		//! Orthographic matrix used in 2D rendering
		Matrix m_orthoMatrix;

		LPCSTR m_windowName = nullptr;
		HINSTANCE m_instance = nullptr;
		HWND m_handle = nullptr;

		//GraphicsDevice * graphicsDevice = nullptr;

		float m_fieldOfView = DirectX::XM_PIDIV4;
		float m_nearPlane = 0.1f;
		float m_farPlane = 1000.0f;

		int m_posX = 0;
		int m_posY = 0;

		static HANDLE CreateConsole();
		static HANDLE s_console;
		static std::unordered_map<LONG, Window*> s_windows;
		static Window * s_currentWindow;
		static Window * s_mainWindow;
	};
}