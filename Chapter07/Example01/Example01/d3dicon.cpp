#include "d3dicon.h"

// load an icon into a texture
IDirect3DTexture9* LoadIconTexture(IDirect3DDevice9* pDevice, LPCWSTR FileName, D3DFORMAT format) 
{
	// texture
	IDirect3DTexture9* pTexture;
	// surface
	IDirect3DSurface9* pSurface;

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

	// create texture
	pDevice->CreateTexture(bmp.bmWidth, bmp.bmHeight, 1, 0, format, D3DPOOL_DEFAULT, &pTexture, NULL);

	// create surface
	pDevice->CreateOffscreenPlainSurface(bmp.bmWidth, bmp.bmHeight, format, D3DPOOL_SYSTEMMEM, &pSurface, NULL);

	// select bitmap into dc
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdc, hbmNew);

	// calculate color component shifts
	DWORD dwAShiftL = 8 - GetFormatABits(format);
	DWORD dwRShiftL = 8 - GetFormatRBits(format);
	DWORD dwGShiftL = 8 - GetFormatGBits(format);
	DWORD dwBShiftL = 8 - GetFormatBBits(format);
	DWORD dwAShiftR = GetFormatRBits(format) + GetFormatGBits(format) + GetFormatBBits(format);
	DWORD dwRShiftR = GetFormatGBits(format) + GetFormatBBits(format);
	DWORD dwGShiftR = GetFormatBBits(format);

	// calculate bytes per pixel
	DWORD dwBytesPerPixel = (GetFormatXBits(format) + GetFormatABits(format) + GetFormatRBits(format) + GetFormatGBits(format) + GetFormatBBits(format)) / 8;

	// color variables
	DWORD dwRed, dwGreen, dwBlue, dwAlpha;
	COLORREF crTransparent = GetPixel(hdc, 0, 0);
	COLORREF crColor;
	DWORD dwColor;
	DWORD dwPosition;

	// position variables
	int x, y;

	// lock the surface
	D3DLOCKED_RECT lockrect;
	pSurface->LockRect(&lockrect, NULL, 0);
	BYTE* pbyBuffer = (BYTE*)lockrect.pBits;

	// scan convert bitmap
	for (y = 0; y < bmp.bmHeight; y++)
	{
		for (x = 0; x < bmp.bmWidth; x++)
		{
			// grab pixel
			crColor = GetPixel(hdc, x + 1, y + 1);

			// calculate position
			dwPosition = y * lockrect.Pitch + x * dwBytesPerPixel;

			// extract rgb
			dwRed = GetRValue(crColor);
			dwGreen = GetGValue(crColor);
			dwBlue = GetBValue(crColor);

			// check for transparency
			dwAlpha = 0;
			if (crColor != crTransparent)
			{
				dwAlpha |=  0xFF;
			}

			// shift bits left
			dwAlpha >>= dwAShiftL;
			dwRed >>= dwRShiftL;
			dwGreen >>= dwGShiftL;
			dwBlue >>= dwBShiftL;

			// shift bits right
			dwAlpha <<= dwAShiftR;
			dwRed <<=dwRShiftR;
			dwGreen <<=dwGShiftR;

			// combine colors
			dwColor = dwAlpha | dwRed | dwGreen | dwBlue;

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

	// copy data from image surface to texture
	IDirect3DSurface9* pTextureSurface;

	// grab surface level
	pTexture->GetSurfaceLevel(0, &pTextureSurface);

	// copy rectangle
	RECT rcSrc;
	POINT ptDst;
	SetRect(&rcSrc, 0, 0, bmp.bmWidth, bmp.bmHeight);
	ptDst.x = 0;
	ptDst.y = 0;
	pDevice->UpdateSurface(pSurface, &rcSrc, pTextureSurface, &ptDst);

	// relese surface level
	pTextureSurface->Release();

	// destroy image surface
	pSurface->Release();

	// return texture
	return (pTexture);
}