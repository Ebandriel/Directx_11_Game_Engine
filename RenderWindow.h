#pragma once
#include "ErrorLogger.h"

class RenderWindow
{
public:
	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessages();
	~RenderWindow();
private:
	void RegisterWindowClass();
	HWND handle = NULL; //window handle
	HINSTANCE hInstance = NULL; // app instance handle
	std::string window_title = "";
	std::wstring window_title_wide = L""; //wide string for windows title
	std::string window_class = "";
	std::wstring window_class_wide = L""; //wide string for window class
	int width = 0;
	int height = 0;

};
