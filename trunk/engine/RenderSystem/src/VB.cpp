#include "VB.h"
#include <fstream>
#include "RenderSystem.h"

CVBPool& GetVBPool()
{
	static CVBPool g_VBPool;
	return g_VBPool;
}

CVBPool::CVBPool(): m_LockingVB(NULL)
{
	m_LockingVB = NULL;
}

CVBPool::~CVBPool()
{
	// 退出确保解锁
	Unlock();
}

int	CVBPool::CreateVertexBuffer(UINT Length, DWORD FVF, DWORD Stride)
{
	int nVBID = CreateBuffer(Length);
	VBSub* pSub = (VBSub*)GetSub(nVBID);
	pSub->dwVertexSize = Stride;
	pSub->dwFVF = FVF;
	if (pSub)
	{
		pSub->Update();
	}
	return nVBID;
}

void* CVBPool::Lock(int nSubID, UINT OffsetToLock, UINT SizeToLock, DWORD Flags)
{
	void* pVoid = NULL;
	VBSub* pSub = (VBSub*)GetSub(nSubID);
	if (pSub)
	{
		CVBChunk* pChunk = (CVBChunk*)GetChunk(pSub->dwChunkID);
		if (pChunk)
		{
			Unlock();
			m_LockingVB = LPDIRECT3DVERTEXBUFFER9(pChunk->m_pBuffer);
			m_LockingVB->Lock(pSub->dwStart+OffsetToLock, pSub->dwLength-OffsetToLock, (void**)&pVoid, Flags);
		}
	}
	return pVoid;
}

void CVBPool::Unlock()
{
	if (m_LockingVB)
	{
		m_LockingVB->Unlock();
		m_LockingVB = NULL;
	}
}

int CVBPool::AddSubBuffer(DWORD dwLength)
{
	VBSub* pSubBuffer = new VBSub;
	pSubBuffer->dwLength = dwLength;
	return m_Subs.Add(pSubBuffer);
}

int CVBPool::AddBufferChunk(DWORD dwLength)
{
	CVBChunk* pChunk = new CVBChunk(dwLength);
	GetRenderSystem().GetDevice()->CreateVertexBuffer(dwLength, D3DUSAGE_WRITEONLY|D3DUSAGE_POINTS, D3DFVF_XYZ|D3DFVF_DIFFUSE, D3DPOOL_MANAGED, (LPDIRECT3DVERTEXBUFFER9*)&pChunk->m_pBuffer, NULL);
	return m_Chunks.Add(pChunk);
}