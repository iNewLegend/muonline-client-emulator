#pragma once
#include "HardwareBuffer.h"

class CHardwareIndexBuffer : public CHardwareBuffer
{
public:
	enum IndexType {IT_16BIT, IT_32BIT};
protected:
	IndexType	m_IndexType;
	size_t		m_uIndexesCount;
	size_t		m_uIndexSize;
public:
	CHardwareIndexBuffer();
	~CHardwareIndexBuffer();
	/// Should be called by HardwareBufferManager
	virtual bool	create			(IndexType idxType, size_t numIndexes, CHardwareBuffer::Usage usage, bool useSystemMemory);
	/// Get the type of indexes used in this buffer
	IndexType		getType			(void) const { return m_IndexType; }
	/// Get the number of indexes in this buffer
	size_t			getIndexesCount	(void) const { return m_uIndexesCount; }
	/// Get the size in bytes of each index
	size_t			getIndexSize	(void) const { return m_uIndexSize; }
	// NB subclasses should override lock, unlock, readData, writeData
};