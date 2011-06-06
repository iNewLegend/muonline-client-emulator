#pragma once
#include "HardwareIndexBuffer.h"
#include "HardwareVertexBuffer.h"
#include <set>

class CHardwareBufferMgr
{
public:
	CHardwareBufferMgr() {}
	virtual ~CHardwareBufferMgr() {}
	// ´´½¨VB IB
	CHardwareVertexBuffer* CreateVertexBuffer(size_t numVerts, size_t vertexSize,
		CHardwareBuffer::Usage usage=CHardwareBuffer::HBU_STATIC);
	CHardwareIndexBuffer* CreateIndexBuffer(size_t numIndexes, CHardwareIndexBuffer::IndexType itype = CHardwareIndexBuffer::IT_16BIT,
		CHardwareBuffer::Usage usage=CHardwareBuffer::HBU_STATIC);
	void _notifyVertexBufferDestroyed(CHardwareVertexBuffer* pHardwareBuffer);
	void _notifyIndexBufferDestroyed(CHardwareIndexBuffer* pHardwareBuffer);
public:
	std::set<CHardwareVertexBuffer*>	m_setVertexHardwareBuffer;
	std::set<CHardwareIndexBuffer*>		m_setIndexHardwareBuffer;
};