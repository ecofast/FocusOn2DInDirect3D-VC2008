/*************************************************************
程序描述: Direct3D 中的 2D 编程, 范例 1.3 之 VC++ 2008 版
程序作者: Ernest Pazera
程序功能: 简单演示如何枚举显示适配器并输出其相关信息
附加依赖: d3d9.lib
创建日期: 2012/12/27
**************************************************************/

#include <windows.h>      // include windows stuff
#include <stdio.h>        // standard input/output
#include "d3d9.h"         // include direct3d9 stuff
#include "d3dfmtutils.h"  // include format utility functions

// constants
// window class name
const wchar_t* WINDOW_CLASS = L"Direct3D for 2D Graphic/Game Programming";
// window title
const wchar_t* WINDOW_TITLE = L"Example 1.3 (TGO-01-F): Enumerating Adapter Display Modes";

// globals
// instance handle
HINSTANCE g_hInstance = NULL;
// window handle
HWND g_hWnd = NULL;
// IDirect3D9 pointer
IDirect3D9* g_pd3d = NULL;

// function prototypes
// winmain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
// window procedure
LRESULT CALLBACK TheWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
// initialization
void Setup();
// clean up
void Cleanup();

// window procedure
LRESULT CALLBACK TheWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// which message did we get?
	switch (uMsg)
	{
	case WM_DESTROY:  // window being destroyed
		{
			// quit
			PostQuitMessage(0);
			// message handled, return 0
			return 0;
		}
		break;
	case WM_KEYDOWN:
		{
			if (wParam == VK_ESCAPE)
				DestroyWindow(hWnd);
			return 0;
		}
		break;
	default:  // all other messages, send to default handler
		return (DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
}

// winmain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// grab instance handle
	g_hInstance = hInstance;

	// redirect stderr and stdout output
	freopen("stderr.txt", "w", stderr);
	freopen("stdout.txt", "w", stdout);

	// fill in window class
	WNDCLASSEX wc;
	wc.cbClsExtra = 0;                                           // no extra class information
	wc.cbSize = sizeof(WNDCLASSEX);                              // size of structure
	wc.cbWndExtra = 0;                                           // no extra window information
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);      // black brush
	wc.hCursor = NULL;                                           // no cursor
	wc.hIcon = NULL;                                             // no icon
	wc.hIconSm = NULL;                                           // no small icon
	wc.hInstance = g_hInstance;                                  // instance handle
	wc.lpfnWndProc = TheWindowProc;                              // window procedure
	wc.lpszClassName = WINDOW_CLASS;                             // name of class
	wc.lpszMenuName = NULL;                                      // no menu
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;  // class style

	// register window class
	RegisterClassEx(&wc);

	// create window
	g_hWnd = CreateWindowEx(0, WINDOW_CLASS, WINDOW_TITLE, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, NULL, g_hInstance, NULL);

	// show the window
	ShowWindow(g_hWnd, nShowCmd);

	// initialization
	Setup();

	MSG msg;
	// message pump
	for ( ; ; )
	{
		// check for a message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// message exists

			// check for quit message
			if (msg.message == WM_QUIT) 
				break;

			// translate the message
			TranslateMessage(&msg);

			// dispatch the message
			DispatchMessage(&msg);
		}
	}

	// clean up
	Cleanup();

	// exit
	return (msg.wParam);
}

// initialization
void Setup()
{
	// create the IDirect3D9 object
	g_pd3d = Direct3DCreate9(D3D_SDK_VERSION);

	// error check
	if (g_pd3d)
	{
		// success
		fprintf(stdout, "IDirect3D9 object created successfully.\n");
	} 
	else
	{
		// failure
		fprintf(stderr, "IDirect3D9 object creation failed.\n");

		// cannot proceed, so return
		return;
	}

	// enumerate display adapters

	// get the number of display adapters for the system
	UINT nAdapterCount = g_pd3d->GetAdapterCount();

	// structure for retrieving data
	D3DADAPTER_IDENTIFIER9 ident;

	// loop through display adapters
	for (UINT nAdapter = 0; nAdapter < nAdapterCount; nAdapter++)
	{
		// grab adapter identifier
		g_pd3d->GetAdapterIdentifier(nAdapter, 0, &ident);

		// report identifier information
		// ordinal
		fprintf(stdout, "Adapter#%d\n", nAdapter);
		// driver name
		fprintf(stdout, "\tDriver: %s\n", ident.Driver);
		// description
		fprintf(stdout, "\tDescription: %s\n\n", ident.Description);

		// enumerate adapter display modes
		D3DDISPLAYMODE dm;
		g_pd3d->GetAdapterDisplayMode(nAdapter, &dm);
		D3DFORMAT fmt = dm.Format;
		// get number of modes
		UINT nModeCount = g_pd3d->GetAdapterModeCount(nAdapter, fmt);
		// structure for receiving data
		D3DDISPLAYMODE mode;
		// temporary string for holding format name
		char temp[40];
		// loop through modes
		for (UINT nMode = 0; nMode < nModeCount; nMode++)
		{
			// grab display mode
			g_pd3d->EnumAdapterModes(nAdapter, fmt, nMode, &mode);

			// report information about mode:
			// # of mode
			fprintf(stdout, "\tMode #%d\n", nMode);
			// width
			fprintf(stdout, "\t\tWidth: %d\n", mode.Width);
			// height
			fprintf(stdout, "\t\tHeight: %d\n", mode.Height);
			// grab name of format
			GetFormatName(mode.Format, temp);
			// format
			fprintf(stdout, "\t\tFormat: %s\n\n", temp);
		}
	}
}

// clean up
void Cleanup()
{
	// safe release of IDirect3D9 object
	if (g_pd3d)
	{
		// release
		g_pd3d->Release();

		// set to NULL
		g_pd3d = NULL;

		// report action
		fprintf(stdout, "IDirect3D9 object released.\n");
	}	
}