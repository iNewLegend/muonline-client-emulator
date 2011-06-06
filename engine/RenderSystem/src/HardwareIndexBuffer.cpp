#include "HardwareIndexBuffer.h"
#include "RenderSystem.h"

CHardwareIndexBuffer::CHardwareIndexBuffer()
{
}

CHardwareIndexBuffer::~CHardwareIndexBuffer()
{
	if (m_pHardwareBufferMgr)
	{
		m_pHardwareBufferMgr->_notifyIndexBufferDestroyed(this);
	}
	//HardwareBufferManager* mgr = HardwareBufferManager::getSingletonPtr();
	//if (mgr)
	//{
	//	mgr->_notifyIndexBufferDestroyed(this);
	//}
}

bool CHardwareIndexBuffer::create(IndexType idxType, size_t numIndexes, CHardwareBuffer::Usage usage, bool useSystemMemory) 
{
	m_Usage = usage;
	m_bSystemMemory = useSystemMemory;
	// ----
	m_IndexType = idxType;
	m_uIndexesCount = numIndexes;
	// ----
	// Calculate the size of the indexes
	switch (m_IndexType)
	{
	case IT_16BIT:
		m_uIndexSize = sizeof(unsigned short);
		break;
	case IT_32BIT:
		m_uIndexSize = sizeof(unsigned int);
		break;
	}
	m_uBufferSize = m_uIndexSize * m_uIndexesCount;
	return true;
}
