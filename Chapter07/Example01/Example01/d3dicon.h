//include guards
#ifndef __D3DICON_H__
#define __D3DICON_H__

#include "d3d9.h"
#include "d3dfmtutils.h"

IDirect3DTexture9* LoadIconTexture(IDirect3DDevice9* pDevice, LPCWSTR FileName, D3DFORMAT format);

#endif