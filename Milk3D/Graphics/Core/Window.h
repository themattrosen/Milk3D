#pragma once

#include <Windows.h>
#include "Graphics/Headers/Common.h"
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
		Window(const char * title, unsigned width, unsigned height, unsigned posX, unsigned posY, bool vsync, bool fullscreen);
		~Window();

		static Window * GetCurrentWindow();
		static Window * GetMainWindow();
		static Window * GetWindow(HWND handle);
		static Window * GetWindow(LONG ID);
		static bool Update();
		static void Render();

		void StartFrame();
		void EndFrame();

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

		bool IsFullscreen() const { return fullscreen; }
		unsigned GetWidth() const { return width; }
		unsigned GetHeight() const { return height; }
		int GetPosX() const { return posX; }
		int GetPosY() const { return posY; }
		HWND GetHandle() { return handle; }
		DirectX::XMMATRIX const & GetProjection() const { return projectionMatrix; }
		DirectX::XMMATRIX const & GetOrtho() const { return orthoMatrix; }

		float GetFarPlane() const { return farPlane; }
		float GetNearPlane() const { return nearPlane; }
		float GetFOV() const { return fieldOfView; }

		// Don't allow copying of a window
		DELETE_COPY(Window);

	private:
		void Initialize();
		unsigned width = 0;
		unsigned height = 0;
		bool fullscreen = false;
		bool minimized = false;

		//! Projection matrix used in 3D rendering
		DirectX::XMMATRIX projectionMatrix;

		//! Orthographic matrix used in 2D rendering
		DirectX::XMMATRIX orthoMatrix;

		LPCSTR windowName = nullptr;
		HINSTANCE instance = nullptr;
		HWND handle = nullptr;

		//GraphicsDevice * graphicsDevice = nullptr;

		float fieldOfView = DirectX::XM_PIDIV4;
		float nearPlane = 0.1f;
		float farPlane = 1000.0f;

		int posX = 0;
		int posY = 0;

		static HANDLE CreateConsole();
		static HANDLE console;
		static std::unordered_map<LONG, Window*> windows;
		static Window * currentWindow;
		static Window * mainWindow;
	};
}