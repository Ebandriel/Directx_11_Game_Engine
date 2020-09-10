#pragma once
#include "WindowContainer.h"
class Engine : WindowContainer
{
public:
	bool Initialise(HINSTANCE hInstance, std::string window_title, std::string window_class, float width, float height);
	bool ProcessMessages();
	void Update();
	void RenderFrame();

};
