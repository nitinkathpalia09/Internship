#include <sdkddkver.h>

// Macro used to reduce namespace pollution
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Reference for various Win32 API functions and
// structure declarations.
#include <Windows.h>

// Header needed for unicode adjustment support
#include <tchar.h>

// Include required for resources
#include "resource.h"

//
//
// WndProc - Window procedure
//
//
HINSTANCE hInstance;
void AddMenu(HWND);
HWND hwnd;
LRESULT
CALLBACK
WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	HWND hFind, hReplace, hItems;
	switch (uMsg)
	{
	case WM_PAINT:
		
		hdc = BeginPaint(hwnd, &ps);
		TextOut(hdc, 0, 0, L"Hello, Windows!", 15);
		EndPaint(hwnd, &ps);
		return 0L;
	HWND hedit;
	case WM_CREATE:
		hedit = CreateWindow(
			TEXT("EDIT"), TEXT(""),
			WS_CHILD | WS_VISIBLE | ES_LEFT | ES_WANTRETURN |
			ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_MULTILINE |
			WS_VSCROLL | WS_HSCROLL,
			0, 0, 0, 0, hWnd,  // use WM_SIZE and MoveWindow() to size
			NULL, GetModuleHandle(NULL), NULL);

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_FILE_NEW:
			CreateFile(L"Hello.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_SYSTEM, NULL);
			break;
		case ID_FILE_OPEN:
			OFSTRUCT csad;
			OpenFile("Hello.txt", &csad, OF_READWRITE);
			break;
		case ID_FILE_SAVE:
			break;
		case ID_FILE_CLOSE:
			PostQuitMessage(0);
			DestroyWindow(hWnd);
			break;
		case ID_EDIT_UNDO:
			SendMessage(hWnd, EM_UNDO, wParam, lParam);
		case ID_EDIT_FIND:
		
			
			hFind = CreateWindow(L"Find", L"Find", WS_CHILD| WS_VISIBLE, 0,0,100,100, hWnd, NULL, hInstance, NULL);
			break;
		case ID_EDIT_REPLACE:
		
			hReplace = CreateWindow(L"Replace", L"Replace", WS_CHILD|WS_VISIBLE, 0, 0, 100, 100, hWnd, NULL, hInstance, NULL);
			break;
		case ID_VIEW_ITEMS:
			
			hItems=CreateWindow(L"Items", L"Items", WS_CHILD | WS_VISIBLE, 0, 0, 100, 100, hWnd, NULL, hInstance, NULL);
			break;
		case ID_VIEW_VALUE:
			PAINTSTRUCT ps;
			HDC hdc;

			hdc = BeginPaint(hWnd, &ps);
			TextOut(hdc, 0, 0, L"Hello, Windows!", 15);
			EndPaint(hWnd, &ps);
			return 1;
		case ID_FORMAT_BACKGROUND:
			SendMessage(hWnd, WM_ERASEBKGND, wParam, lParam); 
			break;
		case ID_FORMAT_FONT:
			 HFONT hFont = CreateFont(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
				OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));
			 SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
			 break;
		}
		
			
		
	default:
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

//
//
// WinMain - Win32 application entry point.
//
//
int
APIENTRY
_tWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR lpCmdLine,
	_In_ int nShowCmd)
{
	const size_t MAX_LOADSTRING = 100;
	
	HCURSOR cursor;
	HICON icon;
	
	


	cursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_MYAPP_POINTER));
	icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYAPP_ICON));


	// Setup window class attributes.
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));

	wcex.cbSize = sizeof(wcex);	// WNDCLASSEX size in bytes
	wcex.style = CS_HREDRAW | CS_VREDRAW;		// Window class styles
	wcex.lpszClassName = L"Window Class";// Window class name
	wcex.hbrBackground = CreateSolidBrush(RGB(180, 180, 180));	// Window background brush color.
	wcex.hCursor = cursor;		// Window cursor
	wcex.lpfnWndProc = WndProc;		// Window procedure associated to this window class.
	wcex.hInstance = hInstance;	// The application instance.
	wcex.hIcon = icon;			// Application icon.
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MYAPP_MENU);
	// Register window and ensure registration success.
	if (!RegisterClassEx(&wcex))
		return 1;
	// Setup window initialization attributes.
	CREATESTRUCT cs;
	ZeroMemory(&cs, sizeof(cs));

	cs.x = 0;	// Window X position
	cs.y = 0;	// Window Y position
	cs.cx = 640;	// Window width
	cs.cy = 480;	// Window height
	cs.hInstance = hInstance; // Window instance.
	cs.lpszClass = wcex.lpszClassName;		// Window class name
	cs.lpszName = wcex.lpszClassName;	// Window title
	cs.style = WS_OVERLAPPEDWINDOW;		// Window style

	// Create the window.
	HWND hWnd = ::CreateWindowEx(
		cs.dwExStyle,
		cs.lpszClass,
		cs.lpszName,
		cs.style,
		cs.x,
		cs.y,
		cs.cx,
		cs.cy,
		cs.hwndParent,
		cs.hMenu,
		cs.hInstance,
		cs.lpCreateParams);

	// Validate window.
	if (!hWnd)
		return 1;

	// Display the window.
	::ShowWindow(hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(hWnd);

	// Main message loop.
	MSG msg;
	while (::GetMessage(&msg, hWnd, 0, 0) > 0)
		::DispatchMessage(&msg);

	// Unregister window class, freeing the memory that was
	// previously allocated for this window.
	::UnregisterClass(wcex.lpszClassName, hInstance);

	return (int)msg.wParam;
}

void AddMenu(HWND hwnd) {
	HMENU  SubMenu, MenuFile, MenuEdit, MenuView, MenuFormat;
	
	SubMenu = CreateMenu();
	MenuFile = CreateMenu();
	MenuEdit = CreateMenu();
	MenuView = CreateMenu();
	MenuFormat = CreateMenu();
	AppendMenuW(SubMenu, MF_STRING | MF_POPUP, (UINT_PTR)MenuFile, L"&File");
	AppendMenu(MenuFile, MF_STRING, ID_FILE_NEW, L"&New");
	AppendMenu(MenuFile, MF_STRING, ID_FILE_OPEN, L"&Open");
	AppendMenu(MenuFile, MF_STRING, ID_FILE_SAVE, L"&Save");
	AppendMenuW(MenuFile, MF_SEPARATOR, 0, NULL);
	AppendMenu(MenuFile, MF_STRING, ID_FILE_CLOSE, L"&Close");
	AppendMenuW(SubMenu, MF_STRING | MF_POPUP, (UINT_PTR)MenuEdit, L"&Edit");
	AppendMenu(MenuEdit, MF_STRING, ID_EDIT_UNDO, L"&Undo");
	AppendMenuW(MenuFile, MF_SEPARATOR, 0, NULL);
	AppendMenu(MenuEdit, MF_STRING, ID_EDIT_FIND, L"&Find");
	AppendMenu(MenuEdit, MF_STRING, ID_EDIT_REPLACE, L"&Replace");
	AppendMenuW(SubMenu, MF_STRING | MF_POPUP, (UINT_PTR)MenuView, L"&View");
	AppendMenu(MenuView, MF_STRING, ID_VIEW_ITEMS, L"&Items");
	AppendMenu(MenuView, MF_STRING, ID_VIEW_VALUE, L"&Value");
	AppendMenuW(SubMenu, MF_STRING | MF_POPUP, (UINT_PTR)MenuFormat, L"&Format");
	AppendMenu(MenuFormat, MF_STRING, ID_FORMAT_BACKGROUND, L"&Background");
	AppendMenu(MenuFormat, MF_STRING, ID_FORMAT_FONT, L"&Font");

	SetMenu(hwnd, SubMenu);
}