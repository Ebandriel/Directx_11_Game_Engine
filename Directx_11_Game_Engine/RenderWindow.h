#pragma once
#include "ErrorLogger.h"

class WindowContainer;

class RenderWindow
{
public:
	bool Initialise(WindowContainer* pWindowContainer, HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessages();
	HWND GetHWND() const;
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
LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);