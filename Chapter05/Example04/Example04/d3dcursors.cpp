#include "d3dcursors.h"

// load a cursor
D3DCursor* D3DCursor_Load(IDirect3DDevice9* pDevice, LPCWSTR FileName)
{
	// allocate new cursor
	D3DCursor* pCursor = new D3DCursor;

	// create a compatible dc
	HDC hdc = CreateCompatibleDC(NULL);

	// load image into hbitmap
	HBITMAP hbmNew = (HBITMAP)LoadImageW(NULL, FileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	// grab dimensions of bitmap
	BITMAP bmp;
	GetObject(hbmNew, sizeof(BITMAP), &bmp);

	// decrease width and height by 1
	bmp.bmWidth--;
	bmp.bmHeight--;

	// select bitmap into dc
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdc, hbmNew);

	// create image surface
	pDevice->CreateOffscreenPlainSurface(bmp.bmWidth, bmp.bmHeight, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pCursor->m_pSurface, NULL);

	// color variables
	COLORREF crTransparent = GetPixel(hdc, 0, 0);
	COLORREF crColor;
	DWORD dwColor;
	DWORD dwPosition;

	// position variables
	int x, y;

	// find hot spot
	pCursor->m_ptHot.x = 0;
	pCursor->m_ptHot.y = 0;

	// horizontal hot spot
	for (x = 0; x < bmp.bmWidth; x++)
	{
		if (GetPixel(hdc, x + 1, 0) == crTransparent)
		{
			pCursor->m_ptHot.x = x;
		}
	}

	// vertical hot spot
	for (y = 0; y < bmp.bmHeight; y++)
	{
		if (GetPixel(hdc, 0, y + 1) == crTransparent)
		{
			pCursor->m_ptHot.y = y;
		}
	}

	// lock the surface
	D3DLOCKED_RECT lockrect;
	pCursor->m_pSurface->LockRect(&lockrect, NULL, 0);
	BYTE* pbyBuffer = (BYTE*)lockrect.pBits;

	// scan convert bitmap
	for (y = 0; y < bmp.bmHeight; y++)
	{
		for (x = 0; x < bmp.bmWidth; x++)
		{
			// grab pixel
			crColor = GetPixel(hdc, x + 1, y + 1);

			// calculate position
			dwPosition = y * lockrect.Pitch + x * 4;

			// combine colors
			dwColor = D3DCOLOR_ARGB(0, GetRValue(crColor), GetGValue(crColor), GetBValue(crColor));

			// check for transparency
			if (crColor != crTransparent)
			{
				dwColor |= 0xFF000000;
			}

			// copy pixel
			memcpy(&pbyBuffer[dwPosition], &dwColor, 4);
		}
	}

	// unlock the surface
	pCursor->m_pSurface->UnlockRect();

	// restore old bitmap to dc
	SelectObject(hdc, hbmOld);

	// destroy hbitmap
	DeleteObject(hbmNew);

	// destroy dc
	DeleteDC(hdc);

	// return pointer to the cursor
	return (pCursor);
}

// destroy a cursor
void D3DCursor_Destroy(D3DCursor* pCursor)
{
	// destroy image surface
	if (pCursor->m_pSurface)
	{
		// release
		pCursor->m_pSurface->Release();

		// set to null
		pCursor->m_pSurface = NULL;
	}

	// destroy cursor struct
	delete pCursor;
}

// set the cursor
void D3DCursor_Set(IDirect3DDevice9* pDevice, D3DCursor* pCursor)
{
	// set the cursor
	pDevice->SetCursorProperties(pCursor->m_ptHot.x, pCursor->m_ptHot.y, pCursor->m_pSurface);
}