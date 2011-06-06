#pragma once
#include "Common.h"
#include "HardwareIndexBuffer.h"
#include <d3d9.h>
#include <d3dx9.h>
//#include <dxerr9.h>

#ifndef D3D9S_REL
#define D3D9S_REL(p) D3D9SafeRelease(*(LPDIRECT3DRESOURCE9*)(&p));
#endif

#ifndef D3D9HR
#define D3D9HR(_hr) D3DCheckHresult(_hr, __FUNCTION__)
#endif

DWORD UsageForD3D9(CHardwareBuffer::Usage usage);
DWORD IndexTypeForD3D9(CHardwareIndexBuffer::IndexType itype);
DWORD LockOptionsForD3D9(CHardwareBuffer::LockOptions options, CHardwareBuffer::Usage usage);

void D3D9SafeRelease(LPDIRECT3DRESOURCE9& pRes);
void D3D9CheckResRef(LPDIRECT3DRESOURCE9 pRes);
bool D3DCheckHresult(HRESULT hr, const char* szInfo = "Unknown");