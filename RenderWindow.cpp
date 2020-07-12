#include "WindowContainer.h"

bool RenderWindow::Initialize(WindowContainer* pWindowContainer, HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	this->hInstance = hInstance;
	this->width = width;
	this->height = height;
	this->window_title = window_title;
	this->window_title_wide = StringConverter::StringToWide(this->window_title);
	this->window_class = window_class;
	this->window_class_wide = StringConverter::StringToWide(this->window_class);

	this->RegisterWindowClass();

	this->handle = CreateWindowEx(0, // standard options
		this->window_class_wide.c_str(), // class name
		this->window_title_wide.c_str(), // window title
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, // window style
		0, // window x position
		0, // window y position
		this->width, // window width
		this->height, // window height
		NULL, // handle to window parent 
		NULL, // handle to menu or child window
		this->hInstance, // handle to window instance
		pWindowContainer);// window creation parameters
	// test to see if window was made
	if (this->handle == NULL)
	{
		ErrorLogger::Log(GetLastError(), "CreateWindowEX Failed for window: " + this->window_title);
		return false;
	}

	// bring window on screen and set it as focus
	ShowWindow(this->handle, SW_SHOW);
	SetForegroundWindow(this->handle);
	SetFocus(this->handle);

	return true;
}

bool RenderWindow::ProcessMessages()
{
	// window message handling
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG)); // init message structure
	if (PeekMessage(&msg, // where to store message
		this->handle, // handle to window we are getting messages from
		0, // min filter
		0, // max filter
		PM_REMOVE))// remove message once captured
	{
		TranslateMessage(&msg); // translate from virtual key to character messages
		DispatchMessage(&msg); // dispatch messages to window proc
	}

	// check if window closed

	if (msg.message == WM_NULL)
	{
		if (!IsWindow(this->handle))
		{
			this->handle = NULL; // message loop destroys window
			UnregisterClass(this->window_class_wide.c_str(), this->hInstance);
			return false;
		}
	}
	return true;
}

RenderWindow::~RenderWindow()
{
	if (this->handle != NULL)
	{
		UnregisterClass(this->window_class_wide.c_str(), this->hInstance);
		DestroyWindow(handle);
	}

}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		WindowContainer* pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);
		if (pWindow == nullptr) // pointer protection
		{
			ErrorLogger::Log("Critical Error: Pointer to window container is null during WM_NCCREATE.");
			exit(-1);
		}
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
		
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);

	}
}

LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		// all other messages
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;
	default:
	{
		//get pointer for windows class
		WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		// forward message to class handler
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
}

void RenderWindow::RegisterWindowClass()
{
	WNDCLASSEX wc; //window class
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // redraw flags vert/horiz from resize
	wc.lpfnWndProc = HandleMessageSetup; // pointer to window proc function for message handling
	wc.cbClsExtra = 0; //no of extra bytes allocated for window-class
	wc.cbWndExtra = 0; //no of extra bytes allocated for window instance
	wc.hInstance = this->hInstance; // passing instance handle
	wc.hIcon = NULL; // handle to class icon resource currently not used
	wc.hIconSm = NULL; // handle to small icon for class currently not used
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // default window cursor if left null will have to explicitly set
	wc.hbrBackground = NULL; //handle to background brush for background colour
	wc.lpszMenuName = NULL; // pointer to null terminated character string for menu
	wc.lpszClassName = this->window_class_wide.c_str(); //pointer to null terminated string for class name
	wc.cbSize = sizeof(WNDCLASSEX); // size of struct
	RegisterClassEx(&wc);

}
