#pragma once

#include "BufferPool.h"
#include "3DObject.h"

#define BUFFER_USAGE_WRITEONLY          (0x00000008L)	// ֪ͨϵͳֻд���棬�豸��ѡ�����ŵ��ڴ����и���Ч��д��������Ⱦ��DEFAULT�ڴ������ʹ�ô˷���
#define BUFFER_USAGE_SOFTWAREPROCESSING (0x00000010L)	// �������
#define BUFFER_USAGE_DONOTCLIP          (0x00000020L)	// �����治��Ҫ�޳�ʱ�������ã�����ǿ�Ʋ��޳�
#define BUFFER_USAGE_POINTS             (0x00000040L)	// ����������ʱ�������ã��������ʱ���ݻᱻ����system�ڴ����
#define BUFFER_USAGE_RTPATCHES          (0x00000080L)	// high-order primitives �����Ƹ߶�����ļ򵥵���ʱ�������� bsp��
#define BUFFER_USAGE_NPATCHES           (0x00000100L)	// N-patches ��������С����ʱ��������
#define BUFFER_USAGE_DYNAMIC            (0x00000200L)	// ��̬���棬����ʹ��MANAGED�ڴ����
// Direct3D 9Ex
//#define D3DUSAGE_TEXTAPI
//#define D3DUSAGE_NONSECURE


struct VBSub: public SubBuffer
{
	DWORD					dwFVF;			// �����FVFֵ
	DWORD					dwVertexSize;	// ����ṹ��С
	DWORD					dwVertexStart;	// ���㿪ʼ
	DWORD					dwVertexCount;	// ������
	void Update()
	{
		dwVertexStart = dwStart / dwVertexSize;
		dwVertexCount = dwLength / dwVertexSize;
	}
};

class RPGSKYEXPORT CVBChunk: public CBufferChunk
{
public:
	CVBChunk(DWORD dwLength): CBufferChunk(dwLength){}
	~CVBChunk()
	{
		if(m_pBuffer) ((LPDIRECT3DVERTEXBUFFER9)m_pBuffer)->Release(); m_pBuffer = NULL;
	}
	DWORD						m_dwUsage;
};

class CVBPool: public CBufferPool, public C3DObject
{
public:
	CVBPool();
	~CVBPool();
public:
	int		CreateVertexBuffer(UINT Length, DWORD FVF, DWORD Stride);
	void*	Lock(int nSubID, UINT OffsetToLock, UINT SizeToLock, DWORD Flags);
	void	Unlock();
protected:
	int		AddSubBuffer(DWORD dwLength);
	// ������Chunk
	int		AddBufferChunk(DWORD dwLength);
protected:
	LPDIRECT3DVERTEXBUFFER9 m_LockingVB;
};

CVBPool& GetVBPool();
