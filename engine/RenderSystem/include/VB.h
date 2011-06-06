#pragma once

#include "BufferPool.h"
#include "3DObject.h"

#define BUFFER_USAGE_WRITEONLY          (0x00000008L)	// 通知系统只写缓存，设备会选择最优的内存块进行更有效的写操作和渲染，DEFAULT内存管理需使用此方法
#define BUFFER_USAGE_SOFTWAREPROCESSING (0x00000010L)	// 软件处理
#define BUFFER_USAGE_DONOTCLIP          (0x00000020L)	// 当绘面不需要剔除时可以设置，且是强制不剔除
#define BUFFER_USAGE_POINTS             (0x00000040L)	// 当绘制粒子时可以设置，软件处理时数据会被载入system内存管理
#define BUFFER_USAGE_RTPATCHES          (0x00000080L)	// high-order primitives 当绘制高度有序的简单的面时可以设置 bsp？
#define BUFFER_USAGE_NPATCHES           (0x00000100L)	// N-patches 当绘制碎小的面时可以设置
#define BUFFER_USAGE_DYNAMIC            (0x00000200L)	// 动态缓存，不能使用MANAGED内存管理
// Direct3D 9Ex
//#define D3DUSAGE_TEXTAPI
//#define D3DUSAGE_NONSECURE


struct VBSub: public SubBuffer
{
	DWORD					dwFVF;			// 顶点的FVF值
	DWORD					dwVertexSize;	// 顶点结构大小
	DWORD					dwVertexStart;	// 顶点开始
	DWORD					dwVertexCount;	// 顶点数
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
	// 增加新Chunk
	int		AddBufferChunk(DWORD dwLength);
protected:
	LPDIRECT3DVERTEXBUFFER9 m_LockingVB;
};

CVBPool& GetVBPool();
