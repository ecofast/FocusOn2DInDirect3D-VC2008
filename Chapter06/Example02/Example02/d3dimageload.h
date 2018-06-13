// include guard
#ifndef __D3DIMAGELOAD_H__
#define __D3DIMAGELOAD_H__

#include <windows.h>
#include <d3d9.h>
#include "d3dfmtutils.h"

// load an image surface from a bitmap file
IDirect3DSurface9* ImageSurfaceFromBMP(IDirect3DDevice9* pDevice, LPCWSTR FileName, D3DFORMAT format);

#endif