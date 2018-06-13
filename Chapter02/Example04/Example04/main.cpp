/*********************************************************************************
程序描述: Direct3D 中的 2D 编程, 范例 2.4 之 VC++ 2008 版
程序作者: Ernest Pazera
程序功能: 以 IDirect3DDevice9 之 Clear 成员为唯一渲染方法模拟实现"游戏人生"
附加依赖: d3d9.lib
创建日期: 2012/12/26
*********************************************************************************/

#include <windows.h>   // include windows stuff
#include <stdio.h>     // standard input/output
#include "d3d9.h"      // include direct3d9 stuff
#include "resource.h"  // include resource IDs

// constants
// window class name
const wchar_t* WINDOW_CLASS = L"Direct3D for 2D Graphic/Game Programming";
// window title
const wchar_t* WINDOW_TITLE = L"Example 2.4 (TGO-02-C, D): Conway's Game of Life";
// screen dimensions
const int SCREEN_WIDTH  = 800;
const int SCREEN_HEIGHT = 600;
// cell size
const int CELL_WIDTH  = 10;
const int CELL_HEIGHT = 10;
// cell columns and rows
const int CELL_COLUMNS = SCREEN_WIDTH / CELL_WIDTH;
const int CELL_ROWS    = SCREEN_HEIGHT / CELL_HEIGHT;

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
// maps
bool g_Alive[CELL_COLUMNS][CELL_ROWS];
bool g_Birth[CELL_COLUMNS][CELL_ROWS];
bool g_Death[CELL_COLUMNS][CELL_ROWS];
// cell rectangles
D3DRECT g_rccells[CELL_COLUMNS][CELL_ROWS];
// running flag
bool g_Running = false;

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
// update the map
void UpdateMap();

// window procedure
LRESULT CALLBACK TheWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// which message did we get?
	switch (uMsg)
	{
	case WM_DESTROY:  // window being destroyed
		{
			// destroy the timer
			KillTimer(hWnd, 0);

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
	case WM_COMMAND:
		{
			// which command?
			int nCommand = LOWORD(wParam);
			switch (nCommand)
			{
			case ID_GAME_GO:  // start running
				{
					// update menu items
					CheckMenuItem(GetMenu(hWnd), ID_GAME_GO, MF_CHECKED);
					EnableMenuItem(GetMenu(hWnd), ID_GAME_GO, MF_GRAYED);
					CheckMenuItem(GetMenu(hWnd), ID_GAME_STOP, MF_UNCHECKED);
					EnableMenuItem(GetMenu(hWnd), ID_GAME_STOP, MF_ENABLED);
					EnableMenuItem(GetMenu(hWnd), ID_GAME_NEXTFRAME, MF_GRAYED);
					EnableMenuItem(GetMenu(hWnd), ID_MAP_CLEAR, MF_GRAYED);
					// set running state
					g_Running = true;
				} 
				break ;
			case ID_GAME_STOP:  // stop running
				{
					// update menu items
					CheckMenuItem(GetMenu(hWnd), ID_GAME_GO, MF_UNCHECKED);
					EnableMenuItem(GetMenu(hWnd), ID_GAME_GO, MF_ENABLED);
					CheckMenuItem(GetMenu(hWnd), ID_GAME_STOP, MF_CHECKED);
					EnableMenuItem(GetMenu(hWnd), ID_GAME_STOP, MF_GRAYED);
					EnableMenuItem(GetMenu(hWnd), ID_GAME_NEXTFRAME, MF_ENABLED);
					EnableMenuItem(GetMenu(hWnd), ID_MAP_CLEAR, MF_ENABLED);
					// set running state
					g_Running = false;
					// show the frame
					RedrawFrame();
				} 
				break;
			case ID_GAME_EXIT:  // exit game
				{
					// destroy the window
					DestroyWindow(hWnd);
				} 
				break;
			case ID_MAP_CLEAR:  // clear map
				{
					// only if not currently running
					if (!g_Running)
					{
						// clear the map
						ClearMap();
						// redraw the frame
						RedrawFrame();
					}
				} 
				break ;
			case ID_GAME_NEXTFRAME:  // go one frame forward
				{
					// only execute if not currently running
					if (!g_Running)
					{
						// update the map
						UpdateMap();
						// redraw the frame
						RedrawFrame();
					}
				} 
				break ; 
			}
			// return 0
			return (0);
		} 
		break;
	case WM_LBUTTONDOWN:  // left mouse button press
		{
			// check for running state
			if (g_Running)
			{
				// is running, so return 0
				return (0);
			}

			// grab x and y location
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			// calculate cell
			x /= CELL_WIDTH;
			y /= CELL_HEIGHT;

			// toggle alive state
			g_Alive[x][y] = !g_Alive[x][y];

			// redraw the frame
			RedrawFrame();

			// return 0
			return (0);
		} 
		break;
	case WM_TIMER :
		{
			// only execute if currently running
			if (g_Running)
			{
				// update map
				UpdateMap();

				// redraw frame
				RedrawFrame();
			}
			// return 0
			return (0);
		} 
		break ; 
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
	g_hWnd = CreateWindowEx(0, WINDOW_CLASS, WINDOW_TITLE, WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_VISIBLE, 
		                    0, 0, 800, 600, NULL, LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MENU1)), g_hInstance, NULL
						   );

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
	// set up timer
	SetTimer(g_hWnd, 0, 50, NULL);

	// resize main window
	RECT rcClient;
	SetRect(&rcClient, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	AdjustWindowRectEx(&rcClient, WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_VISIBLE, TRUE, 0);
	OffsetRect(&rcClient, -rcClient.left, -rcClient.top);
	SetWindowPos(g_hWnd, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOMOVE | SWP_NOZORDER);

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

	// setup presentation parameters
	D3DPRESENT_PARAMETERS parms;
	memset(&parms, 0, sizeof(parms));

	// back buffer information
	parms.BackBufferWidth = 0;             // use client width
	parms.BackBufferHeight = 0;            // use client height
	parms.BackBufferFormat = mode.Format;  // use format of current display mode
	parms.BackBufferCount = 1;             // make one back buffer

	// multisampling
	parms.MultiSampleType = D3DMULTISAMPLE_NONE;

	// swap effect
	parms.SwapEffect = D3DSWAPEFFECT_COPY;  // we want to copy from back buffer to screen

	// destination window
	parms.hDeviceWindow = g_hWnd;
	parms.Windowed = TRUE;

	// depth buffer information
	parms.EnableAutoDepthStencil = FALSE;
	parms.AutoDepthStencilFormat = D3DFMT_UNKNOWN;

	// flags
	parms.Flags = 0;

	// refresh rate and presentation interval
	parms.FullScreen_RefreshRateInHz = 0;
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

	// clear the map
	ClearMap();

	srand(GetTickCount());

	// set up map rectangles and randomize map
	for (int x = 0; x < CELL_COLUMNS; x++)
	{
		for (int y = 0; y < CELL_ROWS; y++)
		{
			g_rccells[x][y].x1 = x * CELL_WIDTH;
			g_rccells[x][y].y1 = y * CELL_HEIGHT;
			g_rccells[x][y].x2 = x * CELL_WIDTH + CELL_WIDTH;
			g_rccells[x][y].y2 = y * CELL_HEIGHT + CELL_HEIGHT;

			if (rand() & 1)
			{
				g_Alive[x][y] = true;
			}
		}
	}

	// draw the frame
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
		fprintf(stdout, "IDirect3DDevice8 object released.\n");
	}

	// safe release of IDirect3D8 object
	if (g_pd3d)
	{
		// release
		g_pd3d->Release();

		// set to NULL
		g_pd3d = NULL;

		// report action
		fprintf(stdout, "IDirect3D8 object released.\n");
	}	
}

//  redraw frame
void RedrawFrame()
{
	D3DCOLOR col ;

	// loop through cells
	for (int x = 0; x < CELL_COLUMNS; x++)
	{
		for (int y = 0; y < CELL_ROWS; y++)
		{
			// determine color of cell
			if (g_Alive[x][y])
			{
				// alive = white
				col = D3DCOLOR_XRGB(255, 255, 255);
			}
			else
			{
				// dead = black
				col = D3DCOLOR_XRGB(0, 0, 0);
			}

			// clear the cell the appropriate color
			g_pd3ddev->Clear(1, &g_rccells[x][y], D3DCLEAR_TARGET, col, 0.0f, 0);
		}
	}

	// present the scene
	g_pd3ddev->Present( NULL, NULL, NULL, NULL);
}

// clear the map
void ClearMap()
{
	// clear out the alive map
	for (int x = 0; x < CELL_COLUMNS; x++)
	{
		for (int y = 0; y < CELL_ROWS; y++)
		{
			g_Alive[x][y] = false;
		}
	}
}

//update the map 
void UpdateMap ( ) 
{
	// loop vars
	int x, y, neighbors, nx, ny;

	// check for births and deaths
	for (x = 0; x < CELL_COLUMNS; x++)
	{
		for (y = 0; y < CELL_ROWS; y++)
		{
			// clear birth flag
			g_Birth[x][y] = false;

			// clear death flag
			g_Death[x][y] = false;

			// start counting neighbors
			neighbors = 0;
			for (nx = x - 1; nx <= x + 1; nx++)
			{
				for (ny = y - 1; ny <= y + 1; ny++)
				{
					// center square doesn't count as neighbor
					if (nx == x && ny == y) 
						continue;

					// bounds checking
					if (nx < 0) 
						continue;
					if (ny < 0) 
						continue;
					if (nx >= CELL_COLUMNS) 
						continue;
					if (ny >= CELL_ROWS) 
						continue;

					// check for a neighbor
					if (g_Alive[nx][ny])
						neighbors++;
				}
			}

			// check state of cell
			if (g_Alive[x][y])
			{
				// currently alive
				// check for death
				if (neighbors < 2 || neighbors > 3) 
				{
					g_Death[x][y] = true;
				}
			}
			else
			{
				// currently dead
				// check for birth
				if (neighbors == 3)
				{
					g_Birth[x][y] = true;
				}
			}
		}
	}

	// apply births and deaths
	for (x = 0; x < CELL_COLUMNS; x++)
	{
		for (y = 0; y < CELL_ROWS; y++)
		{
			// check for birth
			if (g_Birth[x][y])
			{
				g_Alive[x][y] = true;
			}

			// check for death
			if (g_Death[x][y])
			{
				g_Alive[x][y] = false;
			}
		}
	}
}