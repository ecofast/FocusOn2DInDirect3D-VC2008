/********************************************************************************
��������: Direct3D �е� 2D ���, ���� 2.3 ֮ VC++ 2008 ��
��������: Ernest Pazera
������: ��ʾ IDirect3DDevice �� Clear��SetViewport �� Present �ȳ�Ա���÷�
��������: d3d9.lib
��������: 2012/12/25
********************************************************************************/

#include <windows.h>  // include windows stuff
#include <stdio.h>    // standard input/output
#include "d3d9.h"     // include direct3d9 stuff

// constants
// window class name
const wchar_t* WINDOW_CLASS = L"Direct3D for 2D Graphic/Game Programming";
// window title
const wchar_t* WINDOW_TITLE = L"Example 2.3 (TGO-02-B, C, D): Viewports and Presenting";
// screen width and height
const int SCREEN_WIDTH  = 800;
const int SCREEN_HEIGHT = 600;
// cell columns and rows
const int CELL_COLUMNS = 10;
const int CELL_ROWS    = 10;
// cell width
const int CELL_WIDTH  = SCREEN_WIDTH / CELL_COLUMNS;
// cell height
const int CELL_HEIGHT = SCREEN_HEIGHT / CELL_ROWS;

// globals
// instance handle
HINSTANCE g_hInstance = NULL;
// window handle
HWND g_hWnd = NULL;
// IDirect3D9 pointer
IDirect3D9* g_pd3d = NULL;
// device type in use
D3DDEVTYPE g_devtype;
// device pointer
IDirect3DDevice9* g_pd3ddev = NULL;
// main viewport
D3DVIEWPORT9 g_vpmain;
// cell viewports
D3DRECT g_rccells[CELL_COLUMNS][CELL_ROWS];
// game map
bool g_map[CELL_COLUMNS][CELL_ROWS];

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
// clear the map
void ClearMap();
// make a move
void MakeMove(int x, int y);

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
	case WM_LBUTTONDOWN:  // left mouse button press
		{
			// grab x and y coordinates
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			// calculate cell
			x /= CELL_WIDTH;
			y /= CELL_HEIGHT;

			// make a move
			MakeMove(x, y);

			// refresh frame
			RedrawFrame();

			// return 0
			return(0);
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
	wc.hCursor = LoadCursor(0, IDC_ARROW);                       // standard cursor
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
	g_hWnd = CreateWindowEx(0, WINDOW_CLASS, WINDOW_TITLE, WS_POPUP, 0, 0, 800, 600, NULL, NULL, g_hInstance, NULL);

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
	// seed the random generator
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

	// find display mode
	D3DDISPLAYMODE mode;
	g_pd3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);	
	UINT nDisplayModeCount = g_pd3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, mode.Format);

	// loop through display modes
	for (UINT nDisplayMode = 0; nDisplayMode < nDisplayModeCount; nDisplayMode++)
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

	// setup presentation parameters
	D3DPRESENT_PARAMETERS parms;
	memset(&parms, 0, sizeof(parms));

	// back buffer information
	parms.BackBufferWidth = mode.Width;    // use mode width
	parms.BackBufferHeight = mode.Height;  // use mode height
	parms.BackBufferFormat = mode.Format;  // uses format of mode
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
	g_vpmain.X = 0 ;
	g_vpmain.Y = 0 ;
	g_vpmain.Width = SCREEN_WIDTH ;
	g_vpmain.Height = SCREEN_HEIGHT ;
	g_vpmain.MinZ = 0.0f ;
	g_vpmain.MaxZ = 1.0f ;

	// set the viewport
	g_pd3ddev->SetViewport(&g_vpmain);

	// cell RECTs
	// loop through columns
	for (int x = 0; x < CELL_COLUMNS; x++)
	{
		// loop through rows
		for (int y = 0; y < CELL_ROWS; y++)
		{
			// set up viewport
			g_rccells[x][y].x1 = CELL_WIDTH * x + 1;
			g_rccells[x][y].y1 = CELL_HEIGHT * y + 1;
			g_rccells[x][y].x2 = g_rccells[x][y].x1 + CELL_WIDTH - 2;
			g_rccells[x][y].y2 = g_rccells[x][y].y1 + CELL_HEIGHT - 2;
		}
	}

	// set up the map
	ClearMap();  // clear out the map

	for (int i = 0; i < 10; i++)  // make 10 random moves
	{
		MakeMove(rand() % CELL_COLUMNS, rand() % CELL_ROWS);
	}

	// redraw the frame
	RedrawFrame();
}

// clean up
void Cleanup()
{
	// safe release of device
	if (g_pd3ddev)
	{
		// release
		g_pd3ddev->Release();

		// set to NULL
		g_pd3ddev = NULL;

		// report
		fprintf(stdout, "IDirect3DDevice9 object released.\n");
	}

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

//  redraw frame
void RedrawFrame()
{
	// clear the screen
	g_pd3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(128, 128, 128), 0.0f, 0);

	D3DCOLOR col;

	// clear the cells
	for (int x = 0; x < CELL_COLUMNS; x++)
	{
		for (int y = 0; y < CELL_COLUMNS; y++)
		{
			// set the cell viewport
			// g_pd3ddev->SetViewport(&g_vpmain);

			if (g_map[x][y]) 
				col = D3DCOLOR_XRGB(255, 255, 0 );  // lit color
			else
				col = D3DCOLOR_XRGB(0, 0, 255);     // unlit color

			// clear the viewport
			g_pd3ddev->Clear(1, &g_rccells[x][y], D3DCLEAR_TARGET, col, 0.0f, 0);
		}
	}

	// present the scene
	g_pd3ddev->Present( NULL, NULL, NULL, NULL);
}

// clear the map
void ClearMap()
{
	// clear out map cells to false
	for (int x = 0; x < CELL_COLUMNS; x++)
	{
		for (int y = 0; y < CELL_ROWS; y++)
		{
			g_map[x][y] = false;
		}
	}
}

// make a move
void MakeMove(int x, int y)
{
	// toggle center cell
	g_map[x][y] = !g_map[x][y];

	// toggle cell to left
	if (x > 0) 
		g_map[x - 1][y] = !g_map[x - 1][y];

	// toggle cell to right
	if (x < CELL_COLUMNS - 1) 
		g_map[x + 1][y] = !g_map [x + 1][y];

	// toggle cell above
	if (y > 0) 
		g_map[x][y - 1] = !g_map[x][y - 1];

	// toggle cell below
	if (y < CELL_ROWS - 1) 
		g_map[x][y + 1] = !g_map[x][y + 1];
}