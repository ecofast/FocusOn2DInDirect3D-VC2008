#include "SpriteSet.h"
#include "d3dfmtutils.h"
#include <math.h>
#include <stdio.h>

// constructor
CSpriteSet::CSpriteSet(IDirect3DDevice9* pDevice, LPCWSTR FileName, D3DFORMAT Format)
{
	// set the device
	m_pDestinationDevice = pDevice;

	m_nTextureCount = 0;

	// load the image
	Load(FileName, Format);
}

// destructor
CSpriteSet::~CSpriteSet()
{
	// destroy data
	Destroy();
}

// load a bitmap
void CSpriteSet::Load(LPCWSTR FileName, D3DFORMAT Format)
{
	// destroy all current data
	Destroy();

	// create dc
	HDC hdc = CreateCompatibleDC(NULL);

	// load bitmap
	HBITMAP hbmNew = (HBITMAP)LoadImageW(NULL, FileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	// grab statistics for bitmap
	BITMAP bmp;
	GetObject(hbmNew, sizeof(BITMAP), &bmp);

	// select bitmap into dc
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdc, hbmNew);

	// grab control colors
	COLORREF crTransparent = GetPixel(hdc, bmp.bmWidth - 1, 0);
	COLORREF crBorder = GetPixel(hdc, bmp.bmWidth - 1, 1 );
	COLORREF crCenter = GetPixel(hdc, bmp.bmWidth - 1, 2 );
	COLORREF crInside = GetPixel(hdc, bmp.bmWidth - 1, 3 );
	COLORREF crInsideCenter = GetPixel(hdc, bmp.bmWidth - 1, 4);

	// number of column and row dividers
	int nColumns = 0;
	int nRows = 0;
	int x, y;

	// scan to count column divisions
	for (x = 0; x < bmp.bmWidth; x ++)
	{
		if (GetPixel(hdc, x, 0) == crTransparent)
			nColumns ++;
	}

	// scan to count row divisions
	for (y = 0; y < bmp.bmHeight; y ++)
	{
		if (GetPixel(hdc, 0, y) == crTransparent)
			nRows++;
	}

	// allocate row and columns division array
	int* nColumnBreak = new int[nColumns];
	int* nRowBreak = new int[nRows];

	// reset counters
	nColumns = 0; 
	nRows = 0;

	// scan and store column divisions
	for (x = 0; x < bmp.bmWidth; x++)
	{
		if (GetPixel(hdc, x, 0) == crTransparent)
		{		
			// store the column break
			nColumnBreak[nColumns] = x;
			// next column break
			nColumns++;
		}
	}

	// scan to store row divisions
	for (y = 0; y < bmp.bmHeight; y++)
	{
		if (GetPixel(hdc, 0, y) == crTransparent)
		{		
			// store
			nRowBreak[nRows] = y;
			// next
			nRows++;
		}
	}

	// allocate textures and vertices
	m_pTextures = new PDIRECT3DTEXTURE9[(nRows - 1) * (nColumns - 1)];
	m_Vertices = new SpriteSetVertex[(nRows - 1) * (nColumns - 1) * 4];

	// determine largest cell
	int nImageWidth = 0;
	int nImageHeight = 0;

	// determine largest width
	for (x = 0; x < nColumns - 1; x++)
	{
		if (nColumnBreak[x + 1] - nColumnBreak[x] - 1 > nImageWidth)
		{
			nImageWidth = nColumnBreak[x + 1] - nColumnBreak[x] - 1;
		}
	}

	// determine largest height
	for (y = 0; y < nRows - 1; y++)
	{
		if (nRowBreak[y + 1] - nRowBreak[y] - 1 > nImageHeight)
		{
			nImageHeight = nRowBreak[y + 1] - nRowBreak[y] - 1;
		}
	}

	// make it square
	if (nImageWidth < nImageHeight)
	{
		nImageWidth = nImageHeight;
	}

	// make it a power of two
	x = 0;
	for ( ; ; )
	{
		if ((1 << x) >= nImageWidth)
		{
			nImageWidth = 1 << x;
			nImageHeight = 1 << x;
			break;
		}
		x++;
	}

	// create an image surface to hold the largest cell
	IDirect3DSurface9* pSurface;
	m_pDestinationDevice->CreateOffscreenPlainSurface(nImageWidth, nImageHeight, Format, D3DPOOL_SYSTEMMEM, &pSurface, NULL);

	// calculate color component shifts
	DWORD dwAShiftL = 8 - GetFormatABits(Format);
	DWORD dwRShiftL = 8 - GetFormatRBits(Format);
	DWORD dwGShiftL = 8 - GetFormatGBits(Format);
	DWORD dwBShiftL = 8 - GetFormatBBits(Format);
	DWORD dwAShiftR = GetFormatRBits(Format) + GetFormatGBits(Format) + GetFormatBBits(Format);
	DWORD dwRShiftR = GetFormatGBits( Format) + GetFormatBBits(Format) ;
	DWORD dwGShiftR = GetFormatBBits( Format);
	// calculate bytes per pixel
	DWORD dwBytesPerPixel = (GetFormatXBits(Format) + GetFormatABits(Format) + GetFormatRBits(Format) + GetFormatGBits(Format) + GetFormatBBits(Format)) / 8;
	// color variables
	DWORD dwRed, dwGreen, dwBlue, dwAlpha;
	COLORREF crColor;
	DWORD dwColor;
	DWORD dwPosition;
	BYTE* byBuffer;

	fprintf(stdout, "\n\n");

	// loop through images
	int nImage = 0;
	D3DLOCKED_RECT lockrect;
	for (int nRow = 0; nRow < nRows - 1; nRow++)
	{
		for (int nColumn = 0; nColumn < nColumns - 1; nColumn++)
		{
			// lock the image surface
			pSurface->LockRect(&lockrect, NULL, 0);
			byBuffer = (BYTE*)lockrect.pBits;

			// color convert transparent color
			dwRed = GetRValue(crTransparent);
			dwGreen = GetGValue(crTransparent );
			dwBlue = GetBValue(crTransparent );
			dwAlpha = 0;

			// shift left
			dwRed >>= dwRShiftL;
			dwGreen >>= dwGShiftL;
			dwBlue >>= dwBShiftL;
			dwAlpha >>= dwAShiftL;

			// shift right
			dwRed <<= dwRShiftR;
			dwGreen <<= dwGShiftR;
			dwAlpha <<= dwAShiftR;

			// recombine color
			dwColor = dwRed | dwGreen | dwBlue | dwAlpha;

			// clear out the surface
			for (y = 0; y < nImageHeight; y++)
			{
				for (x = 0; x < nImageWidth; x++)
				{
					// calculate position
					dwPosition = y * lockrect.Pitch + x * dwBytesPerPixel;

					// copy pixel
					memcpy(&byBuffer[dwPosition], &dwColor, dwBytesPerPixel);
				}
			}

			// set up scanning rectangle
			RECT rcImage;
			POINT ptCenter;
			SetRect(&rcImage, nColumnBreak[nColumn + 1], nRowBreak[nRow + 1], nColumnBreak[nColumn], nRowBreak[nRow]);		

			// scan top of cell
			for (x = nColumnBreak[nColumn] + 1; x < nColumnBreak[nColumn + 1]; x++)
			{
				// grab pixel
				crColor = GetPixel(hdc, x, nRowBreak[nRow]);

				// check for center
				if (crColor == crCenter || crColor == crInsideCenter)
				{
					// center spot
					ptCenter.x = x;
				}

				// check for inside
				if (crColor == crInside || crColor == crInsideCenter)
				{
					// inside, check to modify left and right
					if (x < rcImage.left)
						rcImage.left = x;
					if (x > rcImage.right)
						rcImage.right = x;
				}
			}
			// add one to right
			rcImage.right++;

			// scan left of cell
			for (y = nRowBreak[nRow] + 1; y < nRowBreak[nRow + 1]; y++)
			{
				// grab pixel
				crColor = GetPixel(hdc, nColumnBreak[nColumn], y);

				// check for center
				if (crColor == crCenter || crColor == crInsideCenter)
				{
					// center spot
					ptCenter.y = y;
				}

				// check for inside
				if (crColor == crInside || crColor == crInsideCenter)
				{
					if (y < rcImage.top)
						rcImage.top = y;
					if (y > rcImage.bottom)
						rcImage.bottom = y;
				}
			}
			// add one to bottom
			rcImage.bottom++;

			// calculate sprite width ;
			int nSpriteWidth = rcImage.right - rcImage.left;
			int nSpriteHeight = rcImage.bottom - rcImage.top;

			// calculate texture width and height4
			int nTextureWidth = nSpriteWidth;
			int nTextureHeight = nSpriteHeight;

			// make texture square
			if (nTextureHeight > nTextureWidth)
				nTextureWidth = nTextureHeight;

			// make texture square
			x = 0;
			for ( ; ; )
			{
				if (1 << x >= nTextureWidth)
				{
					nTextureWidth = 1 << x;
					nTextureHeight = 1 << x;
					break;
				}
				x++;
			}

			// scan convert to image surface
			for (y = 0; y < nSpriteHeight; y++)
			{
				for (x = 0; x < nSpriteWidth; x++)
				{
					// calculate position
					dwPosition = y * lockrect.Pitch + x * dwBytesPerPixel;

					// grab color
					crColor = GetPixel(hdc, x + rcImage.left, y + rcImage.top);

					// color convert
					dwRed = GetRValue(crColor);
					dwGreen = GetGValue(crColor);
					dwBlue = GetBValue(crColor );
					if (crColor == crTransparent)
					{
						dwAlpha = 0;
					}
					else
					{
						dwAlpha = 255;
					}

					// shift left
					dwRed >>= dwRShiftL;
					dwGreen >>= dwGShiftL;
					dwBlue >>= dwBShiftL;
					dwAlpha >>= dwAShiftL;

					// shift right
					dwRed <<= dwRShiftR;
					dwGreen <<= dwGShiftR;
					dwAlpha <<= dwAShiftR;

					// recombine color
					dwColor = dwRed | dwGreen | dwBlue | dwAlpha;

					// copy pixel
					memcpy(&byBuffer[dwPosition], &dwColor, dwBytesPerPixel);
				}
			}

			// unlock the image surface
			pSurface->UnlockRect();

			// create texture
			m_pDestinationDevice->CreateTexture(nTextureWidth, nTextureHeight, 1, 0, Format, D3DPOOL_DEFAULT, &m_pTextures[nImage], NULL);

			// grab texture surface
			IDirect3DSurface9* pTextureSurface;
			m_pTextures[nImage]->GetSurfaceLevel(0, &pTextureSurface);

			// source and destination
			POINT ptDst;
			RECT rcSrc;
			ptDst.x = 0;
			ptDst.y = 0;
			SetRect(&rcSrc, 0, 0, nTextureWidth, nTextureHeight);

			// copy rectangle
			m_pDestinationDevice->UpdateSurface(pSurface, &rcSrc, pTextureSurface, &ptDst);

			// release texture surface
			pTextureSurface->Release();

			// offset rectangle
			OffsetRect(&rcImage, -ptCenter.x, -ptCenter.y);

			// set up vertices
			m_Vertices[nImage * 4].x = rcImage.left;
			m_Vertices[nImage * 4].y = rcImage.top;
			m_Vertices[nImage * 4].z = 0.0f;
			m_Vertices[nImage * 4].rhw = 1.0f;
			m_Vertices[nImage * 4].diffuse = D3DCOLOR_XRGB(255, 255, 255);
			m_Vertices[nImage * 4].u = 0.0f;
			m_Vertices[nImage * 4].v = 0.0f;
			m_Vertices[nImage * 4 + 1].x = rcImage.right;
			m_Vertices[nImage * 4 + 1].y = rcImage.top;
			m_Vertices[nImage * 4 + 1].z = 0.0f;
			m_Vertices[nImage * 4 + 1].rhw = 1.0f;
			m_Vertices[nImage * 4 + 1].diffuse = D3DCOLOR_XRGB(255, 255, 255);
			m_Vertices[nImage * 4 + 1].u = (float)nSpriteWidth / (float)nTextureWidth;
			m_Vertices[nImage * 4 + 1].v = 0.0f;
			m_Vertices[nImage * 4 + 2].x = rcImage.left;
			m_Vertices[nImage * 4 + 2].y = rcImage.bottom;
			m_Vertices[nImage * 4 + 2].z = 0.0f;
			m_Vertices[nImage * 4 + 2].rhw = 1.0f ;
			m_Vertices[nImage * 4 + 2].diffuse = D3DCOLOR_XRGB(255, 255, 255);
			m_Vertices[nImage * 4 + 2].u = 0.0f;
			m_Vertices[nImage * 4 + 2].v = (float)nSpriteHeight / (float)nTextureHeight;
			m_Vertices[nImage * 4 + 3].x = rcImage.right;
			m_Vertices[nImage * 4 + 3].y = rcImage.bottom;
			m_Vertices[nImage * 4 + 3].z = 0.0f;
			m_Vertices[nImage * 4 + 3].rhw = 1.0f;
			m_Vertices[nImage * 4 + 3].diffuse = D3DCOLOR_XRGB(255, 255, 255);
			m_Vertices[nImage * 4 + 3].u = (float)nSpriteWidth / (float)nTextureWidth;
			m_Vertices[nImage * 4 + 3].v = (float)nSpriteHeight / (float)nTextureHeight;

			// increment image number
			nImage++;
		}
	}
	// release the image surface
	pSurface->Release();

	// restore old bitmap to dc
	SelectObject(hdc, hbmOld);

	// destroy bitmap
	DeleteObject(hbmNew);

	// destroy dc
	DeleteDC(hdc);
}

// destroy current data
void CSpriteSet::Destroy()
{
	// destroy all data
	if (m_nTextureCount)
	{
		// destroy all textures
		for (int index = 0; index < GetTextureCount(); index++)
		{
			GetTexture(index)->Release();
		}

		// destroy texture array
		delete [] m_pTextures;

		// set pointer to null
		m_pTextures = NULL;

		// destroy vertex array
		delete [] m_Vertices;

		// set vertex pointer to null
		m_Vertices = NULL;

		// set texture count to 0
		m_nTextureCount = 0;
	}
}

// retrieve information
IDirect3DTexture9* CSpriteSet::GetTexture(int nImage)
{
	// return the texture
	return (m_pTextures[nImage]);
}

SpriteSetVertex* CSpriteSet::GetVertex(int nImage, int nVertex)
{
	// return pointer to the vertex
	return (&m_Vertices[nImage * 4 + nVertex]);
}

int CSpriteSet::GetTextureCount()
{
	// return the vertex count
	return (m_nTextureCount);
}

// render
void CSpriteSet::PutSprite(int nImage, float x, float y, float angle, float scalex, float scaley)
{
	int index;
	SpriteSetVertex vert[4];
	float xpos, ypos;

	// copy vertices
	for (index = 0; index < 4; index++)
	{
		vert[index] = *GetVertex(nImage, index);

		// rotate
		xpos = vert[index].x * cos(angle) - vert[index].y * sin(angle);
		ypos = vert[index].y * cos(angle) + vert[index].x * sin(angle);

		// scale
		xpos *= scalex;
		ypos *= scaley;

		// offset
		xpos += x;
		ypos += y;

		// copy xy
		vert[index].x = xpos;
		vert[index].y = ypos;
	}

	// set the texture
	m_pDestinationDevice->SetTexture(0, m_pTextures[nImage]);

	// render
	m_pDestinationDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vert, sizeof(SpriteSetVertex));
}