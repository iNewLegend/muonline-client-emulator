#pragma once
#include "HardwareBuffer.h"

class CHardwareVertexBuffer : public CHardwareBuffer
{
protected:
	size_t m_uVerticesCount;
	size_t m_uVertexSize;
public:
	CHardwareVertexBuffer();
	~CHardwareVertexBuffer();
	/// Should be called by HardwareBufferManager
	virtual bool	create			(size_t vertexSize, size_t numVertices, CHardwareBuffer::Usage usage, bool useSystemMemory);
	/// Gets the size in bytes of a single vertex in this buffer
	size_t			getVertexSize	(void) const { return m_uVertexSize; }
	/// Get the number of vertices in this buffer
	size_t			getVerticesCount(void) const { return m_uVerticesCount; }
	// NB subclasses should override lock, unlock, readData, writeData
};