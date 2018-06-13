/*********************************************************************************
程序描述: Direct3D 中的 2D 编程, 范例 4.8 之 VC++ 2008 版
程序作者: Ernest Pazera
程序功能: 简单演示点及三角形图元的使用
附加依赖: d3d9.lib
创建日期: 2013/01/01
*********************************************************************************/

#include <windows.h>   // include windows stuff
#include <stdio.h>     // standard input/output
#include <math.h>
#include "d3d9.h"      // include direct3d9 stuff

// custom vertex
#define CUSTOM_VERTEX_FVF D3DFVF_XYZRHW | D3DFVF_DIFFUSE 
struct CustomVertex
{
	float		x, y, z, rhw;
	D3DCOLOR	diffuse;
};

// constants
// window class name
const wchar_t* WINDOW_CLASS = L"3D42DGP";
// window title
const wchar_t* WINDOW_TITLE = L"Example 4.8";
// screen width and height
const int SCREEN_WIDTH  = 800;
const int SCREEN_HEIGHT = 600;
// vertex and primitive counts
const int VERTEX_COUNT    = 4;
const int INDEX_COUNT     = 6;
const int PRIMITIVE_COUNT = 2;
// number of stars
const int STAR_COUNT = 1000;
// pi
const float PI = 3.14159f;
// control constants
enum
{
	CONTROL_UP    = 0,
	CONTROL_LEFT  = 1,
	CONTROL_DOWN  = 2, 
	CONTROL_RIGHT = 3,
	CONTROL_FIRE  = 4,
	CONTROL_COUNT = 5
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
// device pointer
IDirect3DDevice9* g_pd3ddev = NULL;
// main viewport
D3DVIEWPORT9 g_vpmain;
// vertices
CustomVertex g_vertices[VERTEX_COUNT];
//indices
WORD g_indices[INDEX_COUNT];
// angle of rotation
float g_angle = 0.0f;
// x and y location
float g_x = SCREEN_WIDTH / 2;
float g_y = SCREEN_HEIGHT / 2;
// x and y velocity
float g_vx = 0.0;
float g_vy = 0.0;
// control array
bool g_bControls[CONTROL_COUNT] = {false, false, false, false, false};
// star vertices
CustomVertex g_stars[STAR_COUNT];

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

// window procedure
LRESULT CALLBACK TheWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// which message did we get?
	switch (uMsg)
	{
	case WM_KEYDOWN:  // a key has been pressed
		{
			// which key was pressed
			switch ( wParam )
			{
			case VK_ESCAPE:  // escape
				{
					DestroyWindow(hWnd);
				}
				break;
			case VK_LEFT:
				{
					g_bControls[CONTROL_LEFT] = true;
				}
				break;
			case VK_RIGHT:
				{
					g_bControls[CONTROL_RIGHT] = true;
				}
				break;
			case VK_UP:
				{
					g_bControls[CONTROL_UP] = true;
				}
				break;
			case VK_DOWN:
				{
					g_bControls[CONTROL_DOWN] = true;
				}
				break;
			case VK_SPACE:
				{
					g_bControls[CONTROL_FIRE] = true;
				}
				break;
			}

			// return 0
			return 0;

		}
		break;
	case WM_KEYUP:
		{
			// which key was released
			switch (wParam)
			{
			case VK_LEFT:
				{
					g_bControls[CONTROL_LEFT] = false;
				}
				break;
			case VK_RIGHT:
				{
					g_bControls[CONTROL_RIGHT] = false;
				}
				break;
			case VK_UP:
				{
					g_bControls[CONTROL_UP] = false;
				}
				break;
			case VK_DOWN:
				{
					g_bControls[CONTROL_DOWN] = false;
				}
				break;
			case VK_SPACE:
				{
					g_bControls[CONTROL_FIRE] = false;
				}
				break;
			} 

			// return 0
			return (0);
		}
		break ;
	case WM_DESTROY:  // window being destroyed
		{
			// quit
			PostQuitMessage(0);

			// message handled, return 0
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
		else
		{
			// no message, update frame
			RedrawFrame();
		}
	}	

	// clean up
	Cleanup();

	// exit
	return msg.wParam;
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

	// set vertex shader
	g_pd3ddev->SetVertexShader(NULL);
	// set FVF
	g_pd3ddev->SetFVF(CUSTOM_VERTEX_FVF);

	// turn off lighting
	g_pd3ddev->SetRenderState(D3DRS_LIGHTING, FALSE);

	// set up vertex rhw
	g_vertices[0].rhw = 1.0f;
	g_vertices[1].rhw = 1.0f;
	g_vertices[2].rhw = 1.0f;
	g_vertices[3].rhw = 1.0f;

	// set up vertex z
	g_vertices[0].z = 0.0f;
	g_vertices[1].z = 0.0f;
	g_vertices[2].z = 0.0f;
	g_vertices[3].z = 0.0f;

	// set up vertex colors
	g_vertices[0].diffuse = D3DCOLOR_XRGB(0, 0, 255);
	g_vertices[1].diffuse = D3DCOLOR_XRGB(0, 0, 255);
	g_vertices[2].diffuse = D3DCOLOR_XRGB(0, 0, 255);
	g_vertices[3].diffuse = D3DCOLOR_XRGB(0, 0, 255);

	// set up indices

	// triangle 1, vertices 0, 1, 2
	g_indices[0] = 0;
	g_indices[1] = 1;
	g_indices[2] = 2;

	// triangle 2, vertices 0, 3, 1
	g_indices[3] = 0;
	g_indices[4] = 3;
	g_indices[5] = 1;

	// set up the stars
	int nGray;
	for (int nStar = 0; nStar < STAR_COUNT; nStar++)
	{
		// random x, y position
		g_stars[nStar].x = rand() % SCREEN_WIDTH;
		g_stars[nStar].y = rand() % SCREEN_HEIGHT;

		// z = 0, rhw = 1.0
		g_stars[nStar].z = 0.0;
		g_stars[nStar].rhw = 1.0;

		// random gray color
		nGray = rand() % 256;
		g_stars[nStar].diffuse = D3DCOLOR_XRGB(nGray, nGray, nGray);
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

// redraw frame
void RedrawFrame()
{
	// update position
	g_x += g_vx;
	g_y += g_vy;

	// bounds checking
	while (g_x < 0.0)
		g_x += SCREEN_WIDTH;
	while (g_y < 0.0)
		g_y += SCREEN_HEIGHT ;
	while (g_x >= SCREEN_WIDTH)
		g_x -= SCREEN_WIDTH;
	while (g_y >= SCREEN_HEIGHT)
		g_y -= SCREEN_HEIGHT;

	// friction
	g_vx *= (15.0 / 16.0);
	g_vy *= (15.0 / 16.0);

	// thrust
	if (g_bControls[CONTROL_UP])
	{
		g_vx += cos(g_angle);
		g_vy += sin(g_angle);
	}

	// reverse thrust
	if (g_bControls[CONTROL_DOWN])
	{
		g_vx += cos(g_angle - PI);
		g_vy += sin(g_angle - PI);
	}

	// left turn
	if (g_bControls[CONTROL_LEFT])
	{
		g_angle -= (PI / 45.0);
	}

	// right turn
	if (g_bControls[CONTROL_RIGHT])
	{
		g_angle += (PI / 45.0);
	}

	// clear the screen
	g_pd3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 0.0f, 0);

	// set up vertices
	// vertex 0
	g_vertices[0].x = g_x + cos(g_angle) * 20.0;
	g_vertices[0].y = g_y + sin(g_angle) * 20.0;
	// vertex 1
	g_vertices[1].x = g_x + cos(g_angle + 5.0 * PI / 6.0) * 15.0;
	g_vertices[1].y = g_y + sin(g_angle + 5.0 * PI / 6.0) * 15.0;
	// vertex 2	
	g_vertices[2].x = g_x + cos(g_angle - 5.0 * PI / 6.0) * 15.0;
	g_vertices[2].y = g_y + sin(g_angle - 5.0 * PI / 6.0) * 15.0;
	//vertex 3
	g_vertices[3].x = g_x + cos(g_angle - PI) * 5.0;
	g_vertices[3].y = g_y + sin(g_angle - PI) * 5.0;

	// begin the scene
	g_pd3ddev->BeginScene();

	// draw stars
	g_pd3ddev->DrawPrimitiveUP(D3DPT_POINTLIST, STAR_COUNT, g_stars, sizeof(CustomVertex));

	// draw ship
	g_pd3ddev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, VERTEX_COUNT, PRIMITIVE_COUNT, g_indices, D3DFMT_INDEX16, g_vertices, sizeof(CustomVertex));

	// end the scene
	g_pd3ddev->EndScene();

	// present the scene
	g_pd3ddev->Present(NULL, NULL, NULL, NULL);
}