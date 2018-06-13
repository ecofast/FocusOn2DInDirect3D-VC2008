/************************************************************
程序描述: Direct3D 中的 2D 编程, 范例 5.3 之 VC++ 2008 版
程序作者: Ernest Pazera
程序功能: 以 UpdateSurface 替代 Clear 重写点灯游戏
附加依赖: d3d9.lib
创建日期: 2013/01/06
*************************************************************/

#include <windows.h>  //include windows stuff
#include <stdio.h>    //standard input/output
#include <math.h>
#include "D3D9.h"     //include direct3d9 stuff

#include "d3dfmtutils.h"   // include format utility functions
#include "d3dimageload.h"  // include image loading function

#pragma comment(lib, "d3d9.lib")

// constants
// window class name
const wchar_t* WINDOW_CLASS = L"Direct3D for 2D Graphic/Game Programming";
// window title
const wchar_t* WINDOW_TITLE = L"Example 5.3";
// screen width and height
const int SCREEN_WIDTH  = 800;
const int SCREEN_HEIGHT = 600;
// cells
const int CELL_COLUMNS = 10;
const int CELL_ROWS    = 7;
// image dimensions
const int IMAGE_WIDTH  = 64;
const int IMAGE_HEIGHT = 64;

// globals
// instance handle
HINSTANCE g_hInstance = NULL;
// window handle
HWND g_hWnd = NULL;
// IDirect3D9 pointer
IDirect3D9* g_pd3d = NULL;
// device type in use
D3DDEVTYPE g_devtype;
// format of backbuffer
D3DFORMAT g_format;
// device pointer
IDirect3DDevice9* g_pd3ddev = NULL;
// main viewport
D3DVIEWPORT9 g_vpmain;
// image surfaces
IDirect3DSurface9* g_pImageSurfaces[4];
// board
bool g_bBoard[CELL_COLUMNS][CELL_ROWS];
// source rectangle
RECT g_rcSrc;
// destination points
POINT g_ptDst[CELL_COLUMNS][CELL_ROWS];
// hovering over point
POINT g_ptHover;

// function prototypes
// winmain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
// window procedure
LRESULT CALLBACK TheWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
// initialization
void Setup();
// clean up
void Cleanup();
// redraw frame
void RedrawFrame();
// clear the board
void ClearBoard();
// make a move
void MakeMove(int x, int y);

// window procedure
LRESULT CALLBACK TheWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// which message did we get?
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:  // left button has been pressed
		{
			// make a move
			if (g_ptHover.x >= 0 && g_ptHover.y >= 0 && g_ptHover.x < CELL_COLUMNS && g_ptHover.y < CELL_ROWS)
				MakeMove(g_ptHover.x, g_ptHover.y);

			// redraw frame
			RedrawFrame();

			// return 0
			return(0);
		}
		break;
	case WM_MOUSEMOVE:  // mouse has been moved
		{
			// grab mouse coordinates
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			// set hover to -1, -1
			g_ptHover.x = -1;
			g_ptHover.y = -1;

			// check if in vertical range
			if ( y >= g_ptDst[0][0].y && x >= g_ptDst[0][0].x)
			{
				// adjust x and y
				y -= g_ptDst[0][0].y;
				x -= g_ptDst[0][0].x;

				// divide by image width and height
				y /= IMAGE_HEIGHT;
				x /= IMAGE_WIDTH;

				// set hovering point
				g_ptHover.x = x;
				g_ptHover.y = y;

			}

			// redraw the frame
			RedrawFrame();

			// handled, so return 0
			return(0);
		}
		break;
	case WM_KEYDOWN:  // a key has been pressed
		{
			// if escape pressed, destroy window
			if (wParam == VK_ESCAPE) 
				DestroyWindow(hWnd);

			// return 0
			return (0);
		} 
		break;
	case WM_DESTROY:  // window being destroyed
		{
			// quit
			PostQuitMessage(0);

			// message handled, return 0
			return (0);
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
	freopen("stdout.txt", "w", stdout);
	freopen("stderr.txt", "w", stderr);

	// fill in window class
	WNDCLASSEX wc;
	wc.cbClsExtra = 0;                                           // no extra class information
	wc.cbSize = sizeof(WNDCLASSEX);                              // size of structure
	wc.cbWndExtra = 0;                                           // no extra window information
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);      // black brush
	wc.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));   // arrow cursor
	wc.hIcon = NULL;                                             // no icon
	wc.hIconSm = NULL ;	                                         // no small icon
	wc.hInstance = g_hInstance;	                                 // instance handle
	wc.lpfnWndProc = TheWindowProc;                              // window procedure
	wc.lpszClassName = WINDOW_CLASS;                             // name of class
	wc.lpszMenuName = NULL;                                      // no menu
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;  // class styles

	// register window class
	RegisterClassEx(&wc);

	// create window
	g_hWnd = CreateWindowEx(0, WINDOW_CLASS, WINDOW_TITLE, WS_POPUP, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, g_hInstance, NULL);

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
		else
		{
			// no message, update frame
			RedrawFrame();
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
	// seed the random number generator
	srand(GetTickCount());	

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

	// grab the current display mode of the default adapter	
	D3DDISPLAYMODE mode ;
	g_pd3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);
	UINT nDisplayModeCnt = g_pd3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, mode.Format);

	// loop through display modes
	for (UINT nDisplayMode = 0; nDisplayMode < nDisplayModeCnt; nDisplayMode++)
	{
		// check next display mode
		g_pd3d->EnumAdapterModes(D3DADAPTER_DEFAULT, mode.Format, nDisplayMode, &mode);

		// if it matches desired screen width, break out of loop
		if (mode.Width == SCREEN_WIDTH && mode.Height == SCREEN_HEIGHT) 
			break;
	}

	// check for proper sized mode
	if (mode.Width != SCREEN_WIDTH || mode.Height != SCREEN_HEIGHT) 
	{
		// did not find mode
		// post quit message
		PostQuitMessage(0);

		// report
		fprintf(stdout, "Did not find display mode!\n");

		// return
		return;
	}
	else
	{
		// found mode
		// report
		fprintf(stdout, "Found display mode.\n");
	}

	// store the format in a global
	g_format = mode.Format;

	// setup presentation parameters
	D3DPRESENT_PARAMETERS parms;
	memset(&parms, 0, sizeof(parms));

	// back buffer information
	parms.BackBufferWidth = mode.Width;    // use mode width
	parms.BackBufferHeight = mode.Height;  // use mode height
	parms.BackBufferFormat = mode.Format;  // use format of mode
	parms.BackBufferCount = 1;             // make one back buffer

	// multisampling
	parms.MultiSampleType = D3DMULTISAMPLE_NONE;

	// swap effect
	parms.SwapEffect = D3DSWAPEFFECT_COPY;  // we want to copy from back buffer to screen

	// destination window
	parms.hDeviceWindow = g_hWnd;
	parms.Windowed = FALSE;

	// depth buffer information
	parms.EnableAutoDepthStencil = FALSE;
	parms.AutoDepthStencilFormat = D3DFMT_UNKNOWN;

	// flags
	parms.Flags = 0;

	// refresh rate and presentation interval
	parms.FullScreen_RefreshRateInHz = mode.RefreshRate;  // use mode's refresh rate
	parms.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	// attempt to create a HAL device
	HRESULT hr;
	hr = g_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &parms, &g_pd3ddev);
	// error check
	if (FAILED(hr))
	{
		// could not create HAL device
		fprintf(stdout, "Could not create HAL device!\n");

		// attempt to make REF device
		hr = g_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, g_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &parms, &g_pd3ddev);
		if (FAILED(hr))
		{
			// could not create REF device
			fprintf(stdout, "Could not create REF device!\n");

			// post a quit message
			PostQuitMessage(0);
		} 
		else
		{
			// successfully made REF device, store this value in a global
			g_devtype = D3DDEVTYPE_REF;

			// report
			fprintf(stdout, "Successfully created REF device.\n");
		}
	} 
	else
	{
		// successfully made a HAL device, store this value in a global
		g_devtype = D3DDEVTYPE_HAL;

		// report
		fprintf(stdout, "Successfully created HAL device.\n");
	}

	// set up viewports
	// main viewport
	g_vpmain.X = 0;
	g_vpmain.Y = 0;
	g_vpmain.Width = SCREEN_WIDTH;
	g_vpmain.Height = SCREEN_HEIGHT;
	g_vpmain.MinZ = 0.0f;
	g_vpmain.MaxZ = 1.0f;

	// set the viewport
	g_pd3ddev->SetViewport(&g_vpmain);

	// load the image surfaces
	g_pImageSurfaces[0] = ImageSurfaceFromBMP(g_pd3ddev, L"tails.bmp", g_format);
	g_pImageSurfaces[1] = ImageSurfaceFromBMP(g_pd3ddev, L"heads.bmp", g_format);
	g_pImageSurfaces[2] = ImageSurfaceFromBMP(g_pd3ddev, L"tailsnegative.bmp", g_format);
	g_pImageSurfaces[3] = ImageSurfaceFromBMP(g_pd3ddev, L"headsnegative.bmp", g_format);

	// set up points
	for (int x = 0; x < CELL_COLUMNS; x++)
	{
		for (int y = 0; y < CELL_ROWS; y++)
		{
			g_ptDst[x][y].x = IMAGE_WIDTH * x + mode.Width / 2 - CELL_COLUMNS * IMAGE_WIDTH / 2;
			g_ptDst[x][y].y = IMAGE_HEIGHT * y + mode.Height / 2 - CELL_ROWS * IMAGE_HEIGHT / 2;
		}
	}

	// set up source rectangle
	SetRect(&g_rcSrc, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);

	// clear the board
	ClearBoard();

	// make 10 random moves
	for (int i = 0; i < 10; i++)
	{
		MakeMove(rand() % CELL_COLUMNS , rand() % CELL_ROWS);
	}

	// redraw the frame
	RedrawFrame();
}

// clean up
void Cleanup()
{
	// release image surfaces
	for (int nSurface = 0; nSurface < 4; nSurface++)
	{
		if (g_pImageSurfaces[nSurface])
		{
			// release
			g_pImageSurfaces[nSurface]->Release();

			// set to null
			g_pImageSurfaces[nSurface] = NULL;

			// report
			fprintf(stdout, "Released Image Surface#%d\n.", nSurface);
		}
	}

	// safe release of device
	if (g_pd3ddev)
	{
		// release
		g_pd3ddev->Release();

		// set to null
		g_pd3ddev = NULL;

		// report
		fprintf(stdout, "IDirect3DDevice9 object released.\n");
	}

	// safe release of IDirect3D9 object
	if (g_pd3d)
	{
		// release
		g_pd3d->Release();

		// set to null
		g_pd3d = NULL;

		// report action
		fprintf(stdout, "IDirect3D9 object released.\n");
	}
}

// redraw frame
void RedrawFrame()
{
	// clear the screen
	g_pd3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 0.0f, 0);	

	// back buffer surface
	IDirect3DSurface9* pBackBuffer;

	// grab back buffer
	g_pd3ddev->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

	// copy rectangle
	/*
	*	Note: 
	*	The source surface must have been created with D3DPOOL_SYSTEMMEM
	*	The destination surface must have been created with D3DPOOL_DEFAULT
	*/
	// redraw the board
	int nImage;
	for (int y = 0; y < CELL_ROWS; y++)
	{
		for (int x = 0; x < CELL_COLUMNS; x++)
		{
			// by default image 0
			nImage = 0;

			// check board
			if (g_bBoard[x][y])
				nImage = 1;

			// check for hovering
			if (g_ptHover.x == x && g_ptHover.y == y)
				nImage += 2;

			// copy the rectangle
			g_pd3ddev->UpdateSurface(g_pImageSurfaces[nImage], &g_rcSrc, pBackBuffer, &g_ptDst[x][y]);
		}
	}

	// release the back buffer surface
	pBackBuffer->Release();

	// present the scene
	g_pd3ddev->Present(NULL, NULL, NULL, NULL);
}

// clear the board
void ClearBoard()
{
	// loop through columns
	for (int x = 0; x < CELL_COLUMNS; x++)
	{
		// loop through rows
		for (int y = 0; y < CELL_ROWS; y++)
		{
			// set board square to false
			g_bBoard[x][y] = false;
		}
	}
}

// make a move
void MakeMove(int x, int y)
{
	// change square x, y
	g_bBoard[x][y] = !g_bBoard[x][y];

	// change square x + 1, y
	if (x + 1 < CELL_COLUMNS)
		g_bBoard[x + 1][y] = !g_bBoard[x + 1][y];

	// change square x - 1, y
	if (x - 1 >= 0)
		g_bBoard[x - 1][y] = !g_bBoard[x - 1][y];

	// change square x, y + 1
	if (y + 1 < CELL_ROWS)
		g_bBoard[x][y + 1] = !g_bBoard[x][y + 1];

	// change square x, y - 1
	if (y - 1 >= 0)
		g_bBoard [x][y - 1] = !g_bBoard[x][y - 1];
}