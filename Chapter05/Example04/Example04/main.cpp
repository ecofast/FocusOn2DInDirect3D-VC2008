/************************************************************
程序描述: Direct3D 中的 2D 编程, 范例 5.4 之 VC++ 2008 版
程序作者: Ernest Pazera
程序功能: 简单示范 D3D 中光标的使用
附加依赖: d3d9.lib
创建日期: 2013/01/06
*************************************************************/

#include <windows.h>  //include windows stuff
#include <stdio.h>    //standard input/output
#include <math.h>
#include "D3D9.h"     //include direct3d9 stuff

#include "d3dfmtutils.h"   // include format utility functions
#include "d3dimageload.h"  // include image loading function
#include "d3dcursors.h"    // cursor functions

#pragma comment(lib, "d3d9.lib")

// constants
// window class name
const wchar_t* WINDOW_CLASS = L"Direct3D for 2D Graphic/Game Programming";
// window title
const wchar_t* WINDOW_TITLE = L"Example 5.4 (TGO-05-F): D3D Cursors";
// screen width and height
const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;
// tile size
const int TILE_WIDTH = 64;
const int TILE_HEIGHT = 48;
// tile rows and columns
const int TILE_COLUMNS = SCREEN_WIDTH / TILE_WIDTH;
const int TILE_ROWS = SCREEN_HEIGHT / TILE_HEIGHT;
// number of tiles
const int TILE_COUNT = TILE_COLUMNS * TILE_ROWS;
// direction enum
enum
{
	DIR_NORTH = 0,
	DIR_EAST  = 1,
	DIR_SOUTH = 2,
	DIR_WEST  = 3,
	DIR_COUNT = 4
};
// cursor enumeration
enum 
{
	CURSOR_NORTH = 0,
	CURSOR_EAST  = 1,
	CURSOR_SOUTH = 2,
	CURSOR_WEST  = 3,
	CURSOR_NONE  = 4, 
	CURSOR_COUNT = 5
};

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
IDirect3DSurface9* g_pImageSurface = NULL;
// tile source rects
RECT g_rcSrc[TILE_COUNT];
// tile destination points
POINT g_ptDst[TILE_COUNT];
// board
int g_iBoard[TILE_COUNT];
// hole position
POINT g_ptHole;
// cursors
D3DCursor* g_pCursors[CURSOR_COUNT];

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
// make a move
void MakeMove(int direction);
// scramble
void Scramble();

// window procedure
LRESULT CALLBACK TheWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// which message did we get?
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:  // left button has been pressed
		{
			// get mouse position
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			// calculate tile
			x /= TILE_WIDTH;
			y /= TILE_HEIGHT;

			// check for arrow cursors
			if (x == g_ptHole.x && y == g_ptHole.y - 1)
			{
				MakeMove(DIR_SOUTH);
				D3DCursor_Set(g_pd3ddev, g_pCursors[CURSOR_NONE]);
			}
			else if (x == g_ptHole.x && y == g_ptHole.y + 1)
			{
				MakeMove(DIR_NORTH);
				D3DCursor_Set(g_pd3ddev, g_pCursors[CURSOR_NONE]);
			}
			else if (x == g_ptHole.x - 1 && y == g_ptHole.y)
			{
				MakeMove(DIR_EAST);
				D3DCursor_Set(g_pd3ddev, g_pCursors[CURSOR_NONE]);
			}
			else if (x == g_ptHole.x + 1 && y == g_ptHole.y)
			{
				MakeMove(DIR_WEST);
				D3DCursor_Set(g_pd3ddev, g_pCursors[CURSOR_NONE]);
			}

			// redraw frame
			RedrawFrame();

			// handled, return 0
			return(0);
		}
		break;
	case WM_MOUSEMOVE:  // mouse has been moved
		{
			// get mouse position
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			// move the cursor
			g_pd3ddev->SetCursorPosition(x, y, 0);

			// calculate tile
			x /= TILE_WIDTH;
			y /= TILE_HEIGHT;			

			// check for arrow cursors
			if (x == g_ptHole.x && y == g_ptHole.y - 1)
			{
				D3DCursor_Set(g_pd3ddev, g_pCursors[CURSOR_SOUTH]);
			}
			else if (x == g_ptHole.x && y == g_ptHole.y + 1)
			{
				D3DCursor_Set(g_pd3ddev, g_pCursors[CURSOR_NORTH]);
			}
			else if (x == g_ptHole.x - 1 && y == g_ptHole.y)
			{
				D3DCursor_Set(g_pd3ddev, g_pCursors[CURSOR_EAST]);
			}
			else if (x == g_ptHole.x + 1 && y == g_ptHole.y)
			{
				D3DCursor_Set(g_pd3ddev, g_pCursors[CURSOR_WEST]);
			}			
			else  // set cursor to none
			{
				D3DCursor_Set(g_pd3ddev, g_pCursors[CURSOR_NONE]);
			}

			// redraw the frame
			RedrawFrame();

			// handled, so return 0
			return(0);
		}
		break;
	case WM_SETCURSOR:
		{
			// this code keeps windows from changing the cursor
			SetCursor(NULL);
			g_pd3ddev->ShowCursor(TRUE);
			return (TRUE);
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

	// load image surface
	g_pImageSurface = ImageSurfaceFromBMP(g_pd3ddev, L"tansdesk.bmp", g_format);

	// set up source rectangles and destination positions
	for (int x = 0; x < TILE_COLUMNS; x++)
	{
		for (int y = 0; y < TILE_ROWS; y++)
		{
			// set source rectangle
			SetRect(&g_rcSrc[y * TILE_COLUMNS + x], x * TILE_WIDTH, y * TILE_HEIGHT, x * TILE_WIDTH + TILE_WIDTH, y * TILE_HEIGHT + TILE_HEIGHT);

			// set destination points
			g_ptDst[y * TILE_COLUMNS + x].x = x * TILE_WIDTH;
			g_ptDst[y * TILE_COLUMNS + x].y = y * TILE_HEIGHT;

			// set the tile position
			g_iBoard[y * TILE_COLUMNS + x] = x + y * TILE_COLUMNS;
		}
	}

	// set bottom right tile to empty hole
	g_ptHole.x = TILE_COLUMNS - 1;
	g_ptHole.y = TILE_ROWS - 1;
	g_iBoard[TILE_COUNT - 1] = -1;

	// load cursors
	g_pCursors[CURSOR_NORTH] = D3DCursor_Load(g_pd3ddev, L"north.bmp");
	g_pCursors[CURSOR_EAST] = D3DCursor_Load(g_pd3ddev, L"east.bmp");
	g_pCursors[CURSOR_SOUTH] = D3DCursor_Load(g_pd3ddev, L"south.bmp");
	g_pCursors[CURSOR_WEST] = D3DCursor_Load(g_pd3ddev, L"west.bmp");
	g_pCursors[CURSOR_NONE] = D3DCursor_Load(g_pd3ddev, L"none.bmp");

	// set cursor to none
	D3DCursor_Set(g_pd3ddev, g_pCursors[CURSOR_NONE]);

	// scramble tiles
	Scramble();

	// redraw the frame
	RedrawFrame();
}

// clean up
void Cleanup()
{
	// destroy cursors
	for (int nCursor = 0; nCursor < CURSOR_COUNT; nCursor++)
		D3DCursor_Destroy(g_pCursors[nCursor]);

	// safe release of image surface
	if (g_pImageSurface)
	{
		// release
		g_pImageSurface->Release();

		// set to null
		g_pImageSurface = NULL;

		// report
		fprintf(stdout, "Released Image Surface.\n");
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

	// source rectangle
	RECT rcSrc;
	SetRect(&rcSrc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);	

	// back buffer surface
	IDirect3DSurface9* pBackBuffer;

	// grab back buffer
	g_pd3ddev->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

	// render the tiles
	int nIndex;
	int nTile;

	for (nIndex = 0; nIndex < TILE_COUNT; nIndex++)
	{
		// grab tile from board
		nTile = g_iBoard[nIndex];

		// check for valid tile
		if (nTile != -1)
		{
			/*
			*	Note: 
			*	The source surface must have been created with D3DPOOL_SYSTEMMEM
			*	The destination surface must have been created with D3DPOOL_DEFAULT
			*/
			// render tile
			g_pd3ddev->UpdateSurface(g_pImageSurface, &g_rcSrc[nTile], pBackBuffer, &g_ptDst[nIndex]);
		}
	}	

	// release the back buffer surface
	pBackBuffer->Release();

	// present the scene
	g_pd3ddev->Present(NULL, NULL, NULL, NULL);
}

// make a move
void MakeMove(int direction)
{
	// new point to switch
	POINT ptNew = g_ptHole;

	// move new point
	switch (direction)
	{
	case DIR_NORTH:
		{
			ptNew.y++;
		}
		break;
	case DIR_EAST:
		{
			ptNew.x--;
		}
		break;
	case DIR_SOUTH:
		{
			ptNew.y--;
		}
		break;
	case DIR_WEST :
		{
			ptNew.x++;
		}
		break;
	}

	// check bounds
	if (ptNew.x < 0 || ptNew.y < 0 || ptNew.x >= TILE_COLUMNS || ptNew.y >= TILE_ROWS)
		return;

	// move tile
	g_iBoard[g_ptHole.y * TILE_COLUMNS + g_ptHole.x] = g_iBoard[ptNew.x + ptNew.y * TILE_COLUMNS];

	// make new hole
	g_iBoard[ptNew.x + ptNew.y * TILE_COLUMNS] = -1;

	// copy hole
	g_ptHole = ptNew;
}

// scramble
void Scramble()
{
	// assume not mixed
	bool bMixed = false;

	// until mixed
	while (!bMixed)
	{
		// assume mixed
		bMixed = true;

		// checked for mixed state
		for (int nIndex = 0; nIndex < TILE_COUNT; nIndex++)
		{
			if (g_iBoard[nIndex] == nIndex)
			{
				// not mixed
				bMixed = false;
			}
			// if not mixed, break out of loop
			if (!bMixed)
				break;
		}

		// make a move
		MakeMove(rand() % DIR_COUNT);
	}
}