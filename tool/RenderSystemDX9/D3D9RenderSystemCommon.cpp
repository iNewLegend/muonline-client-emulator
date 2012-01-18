#pragma once
#include "D3D9RenderSystemCommon.h"
#include <DxErr.h>


DWORD UsageForD3D9(CHardwareBuffer::Usage usage)
{
	DWORD ret = 0;
	if (usage & CHardwareBuffer::HBU_DYNAMIC)
	{
		//#if OGRE_D3D_MANAGE_BUFFERS
		// Only add the dynamic flag for default pool, and
		// we use default pool when buffer is discardable
		if (usage & CHardwareBuffer::HBU_DISCARDABLE)
			ret |= D3DUSAGE_DYNAMIC;
		//#else
		//		ret |= D3DUSAGE_DYNAMIC;
		//#endif
	}
	if (usage & CHardwareBuffer::HBU_WRITE_ONLY)
	{
		ret |= D3DUSAGE_WRITEONLY;
	}
	return ret;
}

DWORD IndexTypeForD3D9(CHardwareIndexBuffer::IndexType itype)
{
	if (itype == CHardwareIndexBuffer::IT_32BIT)
	{
		return D3DFMT_INDEX32;
	}
	else
	{
		return D3DFMT_INDEX16;
	}
}

DWORD LockOptionsForD3D9(CHardwareBuffer::LockOptions options, CHardwareBuffer::Usage usage)
{
	DWORD ret = 0;
	if (options == CHardwareBuffer::HBL_DISCARD)
	{
		//#if OGRE_D3D_MANAGE_BUFFERS
		// Only add the discard flag for dynamic usgae and default pool
		if ((usage & CHardwareBuffer::HBU_DYNAMIC) &&
			(usage & CHardwareBuffer::HBU_DISCARDABLE))
			ret |= D3DLOCK_DISCARD;
		//#else
		// D3D doesn't like discard or no_overwrite on non-dynamic buffers
		//		if (usage & HardwareBuffer::HBU_DYNAMIC)
		//			ret |= D3DLOCK_DISCARD;
		//#endif
	}
	if (options == CHardwareBuffer::HBL_READ_ONLY)
	{
		// D3D debug runtime doesn't like you locking managed buffers readonly
		// when they were created with write-only (even though you CAN read
		// from the software backed version)
		if (!(usage & CHardwareBuffer::HBU_WRITE_ONLY))
			ret |= D3DLOCK_READONLY;

	}
	if (options == CHardwareBuffer::HBL_NO_OVERWRITE)
	{
		//#if OGRE_D3D_MANAGE_BUFFERS
		// Only add the nooverwrite flag for dynamic usgae and default pool
		if ((usage & CHardwareBuffer::HBU_DYNAMIC) &&
			(usage & CHardwareBuffer::HBU_DISCARDABLE))
			ret |= D3DLOCK_NOOVERWRITE;
		//#else
		//		// D3D doesn't like discard or no_overwrite on non-dynamic buffers
		//		if (usage & HardwareBuffer::HBU_DYNAMIC)
		//			ret |= D3DLOCK_NOOVERWRITE;
		//#endif 
	}

	return ret;
}

void D3D9CheckResRef(LPDIRECT3DRESOURCE9 pRes)
{
	if (pRes)
	{
		int nCount = pRes->AddRef()-1;
		pRes->Release();
		if (nCount>1)
		{
			char szTemp[255];
			sprintf(szTemp,"ref count > 0, count=%d", nCount);
			MessageBoxW(NULL,szTemp,"Error",MB_OK);
		}
	}
}

void D3D9SafeRelease(LPDIRECT3DRESOURCE9& pRes)
{
	if(pRes)
	{
		D3D9CheckResRef(pRes);
		D3D9HR( pRes->Release() );
		pRes = NULL;
	}
}

bool D3DCheckHresult(HRESULT hr, const char* szInfo)
{
	if (FAILED(hr))
	{
		std::string msg;
		msg.append(DXGetErrorDescriptionA(hr));
		msg.append(" : ");
		msg.append(szInfo);
		MessageBoxA(NULL,msg.c_str(),"error",MB_OK);
		assert(false);
		return false;
	}
	return true;
}