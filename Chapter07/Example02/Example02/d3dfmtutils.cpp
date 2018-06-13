/*************************************************************
单元描述: 基于 D3DFORMAT 值的显示格式的处理
单元作者: Ernest Pazera
创建日期: 2012/12/17
**************************************************************/

// include the header
#include "d3dfmtutils.h"

// get string name for format
void GetFormatName(D3DFORMAT Format , char* FormatName)
{
	// set default response
	strcpy(FormatName, "Unknown");
	// which format do we have?
	switch (Format)
	{
	case D3DFMT_R8G8B8:    // 24bpp, 8 for r, g, b
		{
			strcpy(FormatName, "R8G8B8");
		} break;
	case D3DFMT_A8R8G8B8:  // 32bpp, 8 for a, r, g, b
		{
			strcpy(FormatName, "A8R8G8B8");
		} break;
	case D3DFMT_X8R8G8B8:  // 32bpp, 8 for r, g, b
		{
			strcpy(FormatName , "X8R8G8B8");
		} break;
	case D3DFMT_R5G6B5:    // 16bpp, 5 for r, b, 6 for g
		{
			strcpy(FormatName, "R5G6B5");
		} break;
	case D3DFMT_X1R5G5B5:  // 16bpp, 5 for r, g, b
		{
			strcpy(FormatName, "X1R5G5B5");
		} break;
	case D3DFMT_A1R5G5B5:  // 16bpp, 5 for r, g, b, 1 for a
		{
			strcpy(FormatName, "A1R5G5B5");
		} break;
	case D3DFMT_A4R4G4B4:  // 16bpp, 4 for a, r, g, b
		{
			strcpy(FormatName, "A4R4G4B4");
		} break;
	case D3DFMT_R3G3B2:    // 8bpp, 3 for r, g, 2 for b
		{
			strcpy(FormatName, "R3G3B2");
		} break;
	case D3DFMT_A8:        // 8bpp, 8 for a
		{
			strcpy(FormatName, "A8");
		} break;
	case D3DFMT_A8R3G3B2:  // 16bpp, 8 for a, 3 for r, g, 2 for b
		{
			strcpy(FormatName, "A8R3G3B2");
		} break;
	case D3DFMT_X4R4G4B4:  // 16bpp, 4 for r, g, b
		{
			strcpy (FormatName, "X4R4G4B4");
		} break;
	}
}

// get bits for components
UINT GetFormatRBits(D3DFORMAT Format) 	// red
{
	// which format do we have?
	switch (Format)
	{
	case D3DFMT_R8G8B8:    // 24bpp, 8 for r, g, b
		{
			return (8);
		} break;
	case D3DFMT_A8R8G8B8:  // 32bpp, 8 for a, r, g, b
		{
			return (8);
		} break;
	case D3DFMT_X8R8G8B8:  // 32bpp, 8 for r, g, b
		{
			return (8);
		} break;
	case D3DFMT_R5G6B5:    // 16bpp, 5 for r, b, 6 for g
		{
			return (5);
		} break;
	case D3DFMT_X1R5G5B5:  // 16bpp, 5 for r, g, b
		{
			return (5);
		} break;
	case D3DFMT_A1R5G5B5:  // 16bpp, 5 for r, g, b, 1 for a
		{
			return (5);
		} break;
	case D3DFMT_A4R4G4B4:  // 16bpp, 4 for a, r, g, b
		{
			return (4);
		} break;
	case D3DFMT_R3G3B2:    // 8bpp, 3 for r, g, 2 for b
		{
			return (3);
		} break;
	case D3DFMT_A8R3G3B2:  // 16bpp, 8 for a, 3 for r, g, 2 for b
		{
			return (3);
		} break;
	case D3DFMT_X4R4G4B4:  // 16bpp, 4 for r, g, b
		{
			return (4);
		} break;
	}
	// not one of the above, return 0
	return (0);
}

UINT GetFormatGBits(D3DFORMAT Format)  // green
{
	// which format do we have?
	switch (Format)
	{
	case D3DFMT_R8G8B8:    // 24bpp, 8 for r, g, b
		{
			return (8);
		} break;
	case D3DFMT_A8R8G8B8:  // 32bpp, 8 for a, r, g, b
		{
			return (8);
		} break;
	case D3DFMT_X8R8G8B8:  // 32bpp, 8 for r, g, b
		{
			return (8);
		} break;
	case D3DFMT_R5G6B5:    // 16bpp, 5 for r, b, 6 for g
		{
			return (6);
		} break;
	case D3DFMT_X1R5G5B5:  // 16bpp, 5 for r, g, b
		{
			return (5);
		} break;
	case D3DFMT_A1R5G5B5:  // 16bpp, 5 for r, g, b, 1 for a
		{
			return (5);
		} break;
	case D3DFMT_A4R4G4B4:  // 16bpp, 4 for a, r, g, b
		{
			return (4);
		} break;
	case D3DFMT_R3G3B2:    // 8bpp, 3 for r, g, 2 for b
		{
			return (3);
		} break;
	case D3DFMT_A8R3G3B2:  // 16bpp, 8 for a, 3 for r, g, 2 for b
		{
			return (3);
		} break;
	case D3DFMT_X4R4G4B4:  // 16bpp, 4 for r, g, b
		{
			return (4);
		} break;
	}
	// not one of the above, return 0
	return (0);
}

UINT GetFormatBBits(D3DFORMAT Format)  // blue
{
	// which format do we have?
	switch (Format)
	{
	case D3DFMT_R8G8B8:    // 24bpp, 8 for r, g, b
		{
			return (8);
		} break;
	case D3DFMT_A8R8G8B8:  // 32bpp, 8 for a, r, g, b
		{
			return (8);
		} break;
	case D3DFMT_X8R8G8B8:  // 32bpp, 8 for r, g, b
		{
			return (8);
		} break;
	case D3DFMT_R5G6B5:    // 16bpp, 5 for r, b, 6 for g
		{
			return (5);
		} break;
	case D3DFMT_X1R5G5B5:  // 16bpp, 5 for r, g, b
		{
			return (5);
		} break;
	case D3DFMT_A1R5G5B5:  // 16bpp, 5 for r, g, b, 1 for a
		{
			return (5);
		} break;
	case D3DFMT_A4R4G4B4:  // 16bpp, 4 for a, r, g, b
		{
			return (4);
		} break ;
	case D3DFMT_R3G3B2:    // 8bpp, 3 for r, g, 2 for b
		{
			return (2);
		} break;
	case D3DFMT_A8R3G3B2:  // 16bpp, 8 for a, 3 for r, g, 2 for b
		{
			return (2);
		} break;
	case D3DFMT_X4R4G4B4:  // 16bpp, 4 for r, g, b
		{
			return (4);
		} break;
	}
	// not one of the above, return 0
	return (0);
}

UINT GetFormatABits(D3DFORMAT Format)  // alpha
{
	// which format do we have?
	switch (Format)
	{
	case D3DFMT_A8R8G8B8:  // 32bpp, 8 for a, r, g, b
		{
			return (8);
		} break;
	case D3DFMT_A1R5G5B5:  // 16bpp, 5 for r, g, b, 1 for a
		{
			return (1);
		} break;
	case D3DFMT_A4R4G4B4:  // 16bpp, 4 for a, r, g, b
		{
			return (4);
		} break ;
	case D3DFMT_A8:        // 8bpp, 8 for a
		{
			return (8);
		} break;
	case D3DFMT_A8R3G3B2:  // 16bpp, 8 for a, 3 for r, g, 2 for b
		{
			return (8);
		} break;
	}
	// not one of the above, return 0
	return (0);
}

UINT GetFormatDBits(D3DFORMAT Format)  // depth
{
	// which format do we have?
	switch (Format)
	{
	case D3DFMT_D16_LOCKABLE:
		{
			return (16);
		} break;
	case D3DFMT_D32:
		{
			return (32);
		} break;
	case D3DFMT_D15S1:
		{
			return (15);
		} break;
	case D3DFMT_D24S8:
		{
			return (24);
		} break;
	case D3DFMT_D16:
		{
			return (16);
		} break;
	case D3DFMT_D24X8:
		{
			return (24);
		} break;
	case D3DFMT_D24X4S4:
		{
			return (24);
		} break;
	}
	// not found, return 0
	return (0);
}

UINT GetFormatSBits(D3DFORMAT Format)  // stencil
{
	// which format do we have?
	switch (Format)
	{
	case D3DFMT_D15S1:
		{
			return (1);
		} break;
	case D3DFMT_D24S8:
		{
			return (8);
		} break;
	case D3DFMT_D24X4S4:
		{
			return (4);
		} break;
	}
	// not found, return 0
	return (0);
}

UINT GetFormatXBits(D3DFORMAT Format)  // unused bits
{
	switch (Format)
	{
	case D3DFMT_X8R8G8B8:
		{
			return (8);
		} 
		break;
	case D3DFMT_X1R5G5B5:
		{
			return (1);
		} 
		break;
	case D3DFMT_X4R4G4B4:
		{
			return (4);
		} 
		break;
	case D3DFMT_D24X8:
		{
			return (8);
		} 
		break;
	case D3DFMT_D24X4S4:
		{
			return (4);
		} 
		break;
	}
	// not one of the above, return 0
	return (0);
}

// get bit masks for components
UINT GetFormatRMask(D3DFORMAT Format)  // red
{
	// which format do we have?
	switch (Format)
	{
	case D3DFMT_R8G8B8:    // 24bpp, 8 for r, g, b
		{
			return (0x00FF0000);
		} break;
	case D3DFMT_A8R8G8B8:  // 32bpp, 8 for a, r, g, b
		{
			return (0x00FF0000);
		} break;
	case D3DFMT_X8R8G8B8:  // 32bpp, 8 for r, g, b
		{
			return (0x00FF0000);
		} break;
	case D3DFMT_R5G6B5:    // 16bpp, 5 for r, b, 6 for g
		{
			return (0xF800);
		} break;
	case D3DFMT_X1R5G5B5:  // 16bpp, 5 for r, g, b
		{
			return (0x7C00);
		} break;
	case D3DFMT_A1R5G5B5:  // 16bpp, 5 for r, g, b, 1 for a
		{
			return (0x7C00);
		} break;
	case D3DFMT_A4R4G4B4:  // 16bpp, 4 for a, r, g, b
		{
			return (0x0F00);
		} break;
	case D3DFMT_R3G3B2:    // 8bpp, 3 for r, g, 2 for b
		{
			return (0xE0);
		} break;
	case D3DFMT_A8R3G3B2:  // 16bpp, 8 for a, 3 for r, g, 2 for b
		{
			return (0x00E0);
		} break;
	case D3DFMT_X4R4G4B4:  // 16bpp, 4 for r, g, b
		{
			return (0x0F00);
		} break;
	}
	// not one of the above, return 0
	return (0);
}

UINT GetFormatGMask(D3DFORMAT Format)  // green
{
	// which format do we have?
	switch (Format)
	{
	case D3DFMT_R8G8B8:    // 24bpp, 8 for r, g, b
		{
			return (0x0000FF00);
		} break;
	case D3DFMT_A8R8G8B8:  // 32bpp, 8 for a, r, g, b
		{
			return (0x0000FF00);
		} break;
	case D3DFMT_X8R8G8B8:  // 32bpp, 8 for r, g, b
		{
			return (0x0000FF00);
		} break;
	case D3DFMT_R5G6B5:    // 16bpp, 5 for r, b, 6 for g
		{
			return (0x07E0);
		} break;
	case D3DFMT_X1R5G5B5:  // 16bpp, 5 for r, g, b
		{
			return (0x03E0);
		} break;
	case D3DFMT_A1R5G5B5:  // 16bpp, 5 for r, g, b, 1 for a
		{
			return (0x03E0);
		} break;
	case D3DFMT_A4R4G4B4:  // 16bpp, 4 for a, r, g, b
		{
			return (0x00F0);
		} break;
	case D3DFMT_R3G3B2:    // 8bpp, 3 for r, g, 2 for b
		{
			return (0x1C);
		} break;
	case D3DFMT_A8R3G3B2:  // 16bpp, 8 for a, 3 for r, g, 2 for b
		{
			return (0x001C);
		} break;
	case D3DFMT_X4R4G4B4:  // 16bpp, 4 for r, g, b
		{
			return (0x00F0);
		} break;
	}
	// not one of the above, return 0
	return (0);
}

UINT GetFormatBMask(D3DFORMAT Format)  // blue
{
	// which format do we have?
	switch (Format)
	{
	case D3DFMT_R8G8B8:    // 24bpp, 8 for r, g, b
		{
			return (0x000000FF);
		} break;
	case D3DFMT_A8R8G8B8:  // 32bpp, 8 for a, r, g, b
		{
			return (0x000000FF);
		} break;
	case D3DFMT_X8R8G8B8:  // 32bpp, 8 for r, g, b
		{
			return (0x000000FF);
		} break;
	case D3DFMT_R5G6B5:    // 16bpp, 5 for r, b, 6 for g
		{
			return (0x001F);
		} break;
	case D3DFMT_X1R5G5B5:  // 16bpp, 5 for r, g, b
		{
			return (0x001F);
		} break;
	case D3DFMT_A1R5G5B5:  // 16bpp, 5 for r, g, b, 1 for a
		{
			return (0x001F);
		} break;
	case D3DFMT_A4R4G4B4:  // 16bpp, 4 for a, r, g, b
		{
			return (0x000F);
		} break;
	case D3DFMT_R3G3B2:    // 8bpp, 3 for r, g, 2 for b
		{
			return (0x03);
		} break;
	case D3DFMT_A8R3G3B2:  // 16bpp, 8 for a, 3 for r, g, 2 for b
		{
			return (0x03);
		} break;
	case D3DFMT_X4R4G4B4:  // 16bpp, 4 for r, g, b
		{
			return (0x000f);
		} break;
	}
	// not one of the above, return 0
	return (0);
}

UINT GetFormatAMask(D3DFORMAT Format)  // alpha
{
	// which format do we have?
	switch (Format)
	{
	case D3DFMT_A8R8G8B8:  // 32bpp, 8 for a, r, g, b
		{
			return (0xFF000000);
		} break;
	case D3DFMT_A1R5G5B5:  // 16bpp, 5 for r, g, b, 1 for a
		{
			return (0x8000);
		} break;
	case D3DFMT_A4R4G4B4:  // 16bpp, 4 for a, r, g, b
		{
			return (0xF000);
		} break;
	case D3DFMT_A8:        // 8bpp, 8 for a
		{
			return (0xFF);
		} break;
	case D3DFMT_A8R3G3B2:  // 16bpp, 8 for a, 3 for r, g, 2 for b
		{
			return (0xFF00);
		} break;
	}
	// not one of the above, return 0
	return (0);
}

UINT GetFormatDMask(D3DFORMAT Format)  // depth
{
	// which format do we have?
	switch (Format)
	{
	case D3DFMT_D16_LOCKABLE:
		{
			return (0xFFFF);
		} break;
	case D3DFMT_D32:
		{
			return (0xFFFFFFFF);
		} break;
	case D3DFMT_D15S1:
		{
			return (0xFFFE);
		} break;
	case D3DFMT_D24S8:
		{
			return (0xFFFFFF00);
		} break;
	case D3DFMT_D16:
		{
			return (0xFFFF);
		} break;
	case D3DFMT_D24X8:
		{
			return (0xFFFFFF00);
		} break;
	case D3DFMT_D24X4S4:
		{
			return (0xFFFFFF00);
		} break;
	}
	// not found, return 0
	return (0);
}

UINT GetFormatSMask(D3DFORMAT Format)  // stencil
{
	// which format do we have?
	switch (Format)
	{
	case D3DFMT_D15S1:
		{
			return (0x0001);
		} break;
	case D3DFMT_D24S8:
		{
			return (0x000000FF);
		} break;
	case D3DFMT_D24X4S4:
		{
			return (0x0000000F);
		} break;
	}
	// not found, return 0
	return (0);
}