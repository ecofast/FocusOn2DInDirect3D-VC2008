#include "d3dimageload.h"

// load an image surface from a bitmap file
IDirect3DSurface9* ImageSurfaceFromBMP(IDirect3DDevice9* pDevice, LPCWSTR FileName, D3DFORMAT format)
{
	// create a compatible dc
	HDC hdc = CreateCompatibleDC(NULL);

	// load image into hbitmap
	HBITMAP hbmNew = (HBITMAP)LoadImageW(NULL, FileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	// grab dimensions of bitmap
	BITMAP bmp;
	GetObject(hbmNew, sizeof(BITMAP), &bmp);

	// select bitmap into dc
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdc, hbmNew);

	// create image surface
	IDirect3DSurface9* pSurface;
	pDevice->CreateOffscreenPlainSurface(bmp.bmWidth, bmp.bmHeight, format, D3DPOOL_SYSTEMMEM, &pSurface, NULL);

	// calculate color component shifts
	DWORD dwRShiftL = 8 - GetFormatRBits(format);
	DWORD dwGShiftL = 8 - GetFormatGBits(format);
	DWORD dwBShiftL = 8 - GetFormatBBits(format);
	DWORD dwRShiftR = GetFormatGBits(format) + GetFormatBBits(format);
	DWORD dwGShiftR = GetFormatBBits(format);

	// calculate bytes per pixel
	DWORD dwBytesPerPixel = (GetFormatXBits(format) + GetFormatABits(format) + GetFormatRBits(format) + GetFormatGBits(format) + GetFormatBBits(format)) / 8;

	// color variables
	COLORREF crColor;
	DWORD dwColor;
	DWORD dwRed;
	DWORD dwGreen;
	DWORD dwBlue;
	DWORD dwPosition;

	// lock the surface
	D3DLOCKED_RECT lockrect;
	pSurface->LockRect(&lockrect, NULL, 0);
	BYTE* pbyBuffer = (BYTE*)lockrect.pBits;

	// scan convert bitmap
	for (int y = 0; y < bmp.bmHeight; y++)
	{
		for (int x = 0; x < bmp.bmWidth; x++)
		{
			// grab pixel
			crColor = GetPixel(hdc, x, y);

			// calculate position
			dwPosition = y * lockrect.Pitch + x * dwBytesPerPixel;

			// extract rgb
			dwRed = GetRValue(crColor);
			dwGreen = GetGValue(crColor);
			dwBlue = GetBValue(crColor);

			// shift bits left
			dwRed >>= dwRShiftL;
			dwGreen >>= dwGShiftL;
			dwBlue >>= dwBShiftL;

			// shift bits right
			dwRed <<=dwRShiftR;
			dwGreen <<=dwGShiftR;

			// combine colors
			dwColor = dwRed | dwGreen | dwBlue;

			// copy pixel
			memcpy(&pbyBuffer[dwPosition], &dwColor, dwBytesPerPixel);
		}
	}


	// unlock the surface
	pSurface->UnlockRect();

	// restore old bitmap to dc
	SelectObject(hdc, hbmOld);

	// destroy hbitmap
	DeleteObject(hbmNew);

	// destroy dc
	DeleteDC(hdc);

	// return pointer to surface
	return (pSurface);
}