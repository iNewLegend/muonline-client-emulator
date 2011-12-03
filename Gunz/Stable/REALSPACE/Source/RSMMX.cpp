/*
	RSMMX.cpp
	---------

	MMX helper functions

	Programming by Joongpil Cho
	All copyright (c) 1999, MAIET entertainment Inc.
*/
#include "RealSpace.h"
#include <memory.h>

// FPUȤ�� MMX �� Ư���� �����ڵ带 ����ϱ� ���� �ּ����� ���ĵ� ����Ʈ�� �̺��� ���� memcpy�� �������.
#define THIN_THRESHOLD	46

#define FPU_ALIGN		16		// FPU�� 16����Ʈ ������ ����. �׷����� MMX���ٴ� �����ٴ±�.
#define MMX_ALIGN		8		// MMX�� 8����Ʈ ������ ����

static	void*	MMX_Copy(void *pDest ,const void *pSrc,size_t size);

void*	(*RSMemCopy)(void *pDest, const void *pSrc, size_t size) = memcpy;


/*
	ruIsMMX()
	: MMX�� �Ǻ��Ѵ�. �翬�� return ���� true�̸� MMX instruction ��밡��
*/
bool __cdecl InitMMXFunction()
{
    SYSTEM_INFO si;
    int nCPUFeatures=0;
	bool bRet;

    GetSystemInfo(&si);

    if (si.dwProcessorType != PROCESSOR_INTEL_386 && si.dwProcessorType != PROCESSOR_INTEL_486){
        try {
            __asm
            {
                push eax
                push ebx
                push ecx
                push edx

                mov eax,1
                _emit 0x0f
                _emit 0xa2
                mov nCPUFeatures,edx

                pop edx
                pop ecx
                pop ebx
                pop eax
            }
        }catch(...){
			return false;
        }
    }

    bRet = ((nCPUFeatures&0x00800000)!=0);
	if(bRet){
		// MMX���
		RSMemCopy = MMX_Copy;
	} else {
		// MMX�� �ƴϸ�?
		RSMemCopy = memcpy;
	}
	return bRet;
}


/*
	MMX_Copy
	MMX ������ �̿��ؼ� 8����Ʈ ������ �Ѵ�.
*/
static	void* MMX_Copy(void *pDest ,const void *pSrc,size_t size)
{
	unsigned int prebytes,qdwords,postbyte = size;

	if( postbyte	>=	THIN_THRESHOLD )
	{
		prebytes = (MMX_ALIGN-(((long)pDest)%MMX_ALIGN))%MMX_ALIGN;
		if( prebytes>postbyte ) prebytes=postbyte;
		postbyte = postbyte-prebytes;
		qdwords	= postbyte/8;
		postbyte = postbyte-qdwords*8;

		__asm
		{
			MOV		ESI,[pSrc]
			MOV		EDI,[pDest]

			MOV		ECX,[prebytes]
			JCXZ	QWORDSTART
		REP	MOVS	[EDI],[ESI]

QWORDSTART:

			MOV		ECX,[qdwords]
			JCXZ	POSTBYTES

ALIGN 16
QWORDLOOP:
			MOVQ    MM0,[ESI]
			MOVQ    [EDI],MM0
			ADD     ESI, 8
			ADD     EDI, 8
			LOOP	QWORDLOOP

POSTBYTES:
			MOV		ECX,[postbyte]
			JCXZ	DONE
		REP	MOVS	[EDI],[ESI]

DONE:
		}
	}
	else memcpy( pDest, pSrc, postbyte );
	__asm
	{
		EMMS
	}
	return NULL;
}



