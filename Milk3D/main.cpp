#include <iostream>
#include "Graphics/Core/Window.h"

int main()
{
	Milk3D::Window window("Milk3D", 1600, 900, 100, 100, true, false);;
	while (window.Update())
	{
		window.StartFrame();

		window.EndFrame();
	}

	return 0;
}