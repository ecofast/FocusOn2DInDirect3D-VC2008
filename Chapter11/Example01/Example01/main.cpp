/*******************************************************************
程序描述: Direct3D 中的 2D 编程, 范例 11.1 之 VC++ 2008 版
程序作者: Ernest Pazera
程序功能: 矩阵变换演示
附加依赖: d3d9.lib
创建日期: 2012/01/21
*******************************************************************/

#include <windows.h>       // include windows stuff
#include <stdio.h>         // standard input/output
#include <math.h>
#include "d3d9.h"          // include direct3d9 stuff
#include "d3dx9math.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

// custom vertex
#define CUSTOM_VERTEX_FVF D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1
struct CustomVertex
{
	float x, y, z, rhw;
	D3DCOLOR diffuse;
	float u, v;
};

// constants
// window class name
const wchar_t* WINDOW_CLASS = L"Direct3D for 2D Graphic/Game Programming";
// window title
const wchar_t* WINDOW_TITLE = L"Example 11.1 (TGO-11-B): Transformations";
// screen width and height
const int SCREEN_WIDTH  = 800;
const int SCREEN_HEIGHT = 600;
// pi
const float PI = 3.14159f;

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
CustomVertex vert[2][4];
// matrices
D3DXMATRIX matWorld;
D3DXMATRIX matProj;

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
	case WM_DESTROY:  // window being destroyed
		{
			// quit
			PostQuitMessage(0);

			// message handled, return 0
			return 0;
		}
		break;
	case WM_KEYDOWN:  // a key has been pressed
		{
			// if escape pressed, destroy window
			if (wParam == VK_ESCAPE)
				DestroyWindow(hWnd);			

			// return 0
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
	wc.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));   // arrow cursor
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

	// set up vertices
	// dark square
	vert[0][0].x = 0.0;
	vert[0][0].y = 0.0;
	vert[0][0].z = 0.0;
	vert[0][0].diffuse = D3DCOLOR_XRGB(128, 128, 128);
	vert[0][1].x = 1.0;
	vert[0][1].y = 0.0;
	vert[0][1].z = 0.0;
	vert[0][1].diffuse = D3DCOLOR_XRGB(128, 128, 128);
	vert[0][2].x = 0.0;
	vert[0][2].y = 1.0;
	vert[0][2].z = 0.0;
	vert[0][2].diffuse = D3DCOLOR_XRGB(128, 128, 128);
	vert[0][3].x = 1.0;
	vert[0][3].y = 1.0;
	vert[0][3].z = 0.0;
	vert[0][3].diffuse = D3DCOLOR_XRGB(128, 128, 128);
	// light square
	vert[1][0].x = 0.0;
	vert[1][0].y = 0.0;
	vert[1][0].z = 0.0;
	vert[1][0].diffuse = D3DCOLOR_XRGB(192, 192, 192);
	vert[1][1].x = 1.0;
	vert[1][1].y = 0.0;
	vert[1][1].z = 0.0;
	vert[1][1].diffuse = D3DCOLOR_XRGB(192, 192, 192);
	vert[1][2].x = 0.0;
	vert[1][2].y = 1.0;
	vert[1][2].z = 0.0;
	vert[1][2].diffuse = D3DCOLOR_XRGB(192, 192, 192);
	vert[1][3].x = 1.0;
	vert[1][3].y = 1.0;
	vert[1][3].z = 0.0;
	vert[1][3].diffuse = D3DCOLOR_XRGB(192, 192, 192);

	// set up projection matrix
	D3DXMatrixIdentity(&matProj);
	matProj._11 = 0.1875;
	matProj._41 = -0.75;
	matProj._22 = -0.25;
	matProj._42 = 1.0;

	// set the projection matrix
	g_pd3ddev->SetTransform(D3DTS_PROJECTION, &matProj);

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

// redraw frame
void RedrawFrame()
{
	// clear the screen
	g_pd3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 0.0f, 0);
	
	// begin the scene
	g_pd3ddev->BeginScene();

	// loop through board squares
	int oddeven;
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			// determine color of square
			oddeven = ( x + y ) & 1;

			// set up world matrix
			D3DXMatrixTranslation(&matWorld, (float)x , (float)y, 0.0);

			// set world transformatin
			g_pd3ddev->SetTransform(D3DTS_WORLD, &matWorld);

			// draw the square
			g_pd3ddev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vert[oddeven], sizeof(CustomVertex));
		}
	}

	// end the scene
	g_pd3ddev->EndScene();

	// present the scene
	g_pd3ddev->Present(NULL, NULL, NULL, NULL);
}