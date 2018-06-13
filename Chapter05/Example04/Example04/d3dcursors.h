#ifndef __D3DCURSORS_H__
#define __D3DCURSORS_H__

#include <windows.h>
#include <d3d9.h>
#include "d3dfmtutils.h"

// cursor structure
struct D3DCursor
{
	// image surface
	IDirect3DSurface9* m_pSurface;

	// hot spot
	POINT m_ptHot;
} ;

// load a cursor
D3DCursor* D3DCursor_Load(IDirect3DDevice9* pDevice, LPCWSTR FileName);

// destroy a cursor
void D3DCursor_Destroy(D3DCursor* pCursor);

// set the cursor
void D3DCursor_Set(IDirect3DDevice9* pDevice, D3DCursor* pCursor);

#endif