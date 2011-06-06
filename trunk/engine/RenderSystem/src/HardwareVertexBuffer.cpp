#include "HardwareVertexBuffer.h"
#include "RenderSystem.h"

CHardwareVertexBuffer::CHardwareVertexBuffer() 
{
}

CHardwareVertexBuffer::~CHardwareVertexBuffer()
{
	if (m_pHardwareBufferMgr)
	{
		m_pHardwareBufferMgr->_notifyVertexBufferDestroyed(this);
	}
	//HardwareBufferManager* mgr = HardwareBufferManager::getSingletonPtr();
	//if (mgr)
	//{
	//	mgr->_notifyVertexBufferDestroyed(this);
	//}
}

bool CHardwareVertexBuffer::create(size_t vertexSize, size_t numVertices, CHardwareBuffer::Usage usage, bool useSystemMemory) 
{
	m_Usage = usage;
	m_bSystemMemory = useSystemMemory;
	// ----
	m_uVerticesCount = numVertices;
	m_uVertexSize = vertexSize;
	// Calculate the size of the vertices
	m_uBufferSize = m_uVerticesCount * m_uVertexSize;
	return true;
}