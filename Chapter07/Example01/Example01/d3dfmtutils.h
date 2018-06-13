/*************************************************************
单元描述: 基于 D3DFORMAT 值的显示格式的处理
单元作者: Ernest Pazera
创建日期: 2012/12/17
**************************************************************/

#ifndef __D3DFMTUTILS_H__
#define __D3DFMTUTILS_H__

// include d3d9 header
#include "d3d9.h"

// get string name for format
void GetFormatName(D3DFORMAT Format, char* FormatName);

// get bits for components
UINT GetFormatRBits(D3DFORMAT Format);  // red
UINT GetFormatGBits(D3DFORMAT Format);  // green
UINT GetFormatBBits(D3DFORMAT Format);  // blue
UINT GetFormatABits(D3DFORMAT Format);  // alpha
UINT GetFormatDBits(D3DFORMAT Format);  // depth
UINT GetFormatSBits(D3DFORMAT Format);  // stencil
UINT GetFormatXBits(D3DFORMAT Format);  // unused bits

// get bit masks for components
UINT GetFormatRMask(D3DFORMAT Format);  // red
UINT GetFormatGMask(D3DFORMAT Format);  // green
UINT GetFormatBMask(D3DFORMAT Format);  // blue
UINT GetFormatAMask(D3DFORMAT Format);  // alpha
UINT GetFormatDMask(D3DFORMAT Format);  // depth
UINT GetFormatSMask(D3DFORMAT Format);  // stencil

#endif