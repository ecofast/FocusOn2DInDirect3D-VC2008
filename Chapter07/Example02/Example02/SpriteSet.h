#ifndef __SPRITESET_H__
#define __SPRITESET_H__

#include "d3d9.h"

// vertex format description used for spriteset	
const int SPRITESET_VERTEX_FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

// custom vertex format for spriteset
struct SpriteSetVertex
{
	float x, y, z, rhw;
	DWORD diffuse;
	float u, v;
};

// spriteset class
class CSpriteSet  
{
private:
	// destination device
	IDirect3DDevice9* m_pDestinationDevice;

	// textures
	IDirect3DTexture9** m_pTextures;

	// vertex array
	SpriteSetVertex* m_Vertices;

	// number of textures
	int m_nTextureCount;

public:
	// constructor
	CSpriteSet(IDirect3DDevice9* pDevice, LPCWSTR FileName, D3DFORMAT Format);

	// destructor
	virtual ~CSpriteSet();

	// load a bitmap
	void Load(LPCWSTR FileName, D3DFORMAT Format);

	// destroy current data
	void Destroy();

	// retrieve information
	IDirect3DTexture9* GetTexture(int nImage);
	SpriteSetVertex* GetVertex(int nImage, int nVertex);
	int GetTextureCount();

	// render
	void PutSprite(int nImage, float x, float y, float angle = 0.0f, float scalex = 1.0f, float scaley = 1.0f);
};

#endif