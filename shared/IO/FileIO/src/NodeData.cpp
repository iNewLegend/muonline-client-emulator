#include "NodeData.h"
#include "assert.h"

NodeBuffer::NodeBuffer():
size(0),
buffer(NULL)
{
}

NodeBuffer::~NodeBuffer()
{
	clear();
}

void NodeBuffer::clear()
{
	if (buffer)
	{
		delete[] buffer;
	}
	buffer = 0;
	size = 0;
}

bool NodeBuffer::createBuffer(size_t uSize, const void* srcBuffer)
{
	clear();
	size = uSize;
	if (uSize==0)
	{
		return false;
	}
	buffer = new char[size];
	memcpy(buffer, srcBuffer, size);
	return true;
}

//////////////////////////////////////////////////////////////////////////
CNodeData::CNodeData()
{
	m_pParent		= NULL;
	m_pfirstChild	= NULL;
	m_pLastChild	= NULL;
	m_pPrev			= NULL;
	m_pNext			= NULL;

	m_NodeBuffer.size	= 0;
	m_NodeBuffer.buffer = 0;
}

CNodeData::~CNodeData()
{
	CNodeData* pNode = m_pfirstChild;
	CNodeData* pTemp = NULL;

	while (pNode)
	{
		pTemp = pNode;
		pNode = pNode->nextSibling();
		delete pTemp;
	}
}

void CNodeData::Clear()
{
	CNodeData* pNode = m_pfirstChild;
	CNodeData* pTemp = NULL;

	while (pNode)
	{
		pTemp = pNode;
		pNode = pNode->nextSibling();
		delete pTemp;
	}

	m_pfirstChild = 0;
	m_pLastChild = 0;
}

CNodeData* CNodeData::LinkPrev(CNodeData* pNode)
{
	//assert(pNode->m_pParent == NULL || pNode->m_pParent == this);

	pNode->m_pParent	= m_pParent;
	pNode->m_pPrev		= m_pPrev;
	pNode->m_pNext		= this;
	m_pPrev				= pNode;

	if (m_pPrev)
	{
		m_pPrev->m_pNext = pNode;
	}
	else if (m_pParent)
	{
		m_pParent->m_pfirstChild = pNode;
	}
	return pNode;
}

CNodeData* CNodeData::LinkNext(CNodeData* pNode)
{
	//assert(pNode->m_pParent == NULL || pNode->m_pParent == this);

	pNode->m_pParent	= m_pParent;
	pNode->m_pPrev		= this;
	pNode->m_pNext		= m_pNext;
	m_pNext				= pNode;

	if (m_pNext)
	{
		m_pNext->m_pPrev = pNode;
	}
	else if (m_pParent)
	{
		m_pParent->m_pLastChild = pNode;
	}
	return pNode;
}

CNodeData* CNodeData::LinkEndChild(CNodeData* pNode)
{
	assert(pNode->m_pParent == NULL || pNode->m_pParent == this);

	pNode->m_pParent	= this;
	pNode->m_pPrev		= m_pLastChild;
	pNode->m_pNext		= NULL;

	if (m_pLastChild)
		m_pLastChild->m_pNext = pNode;
	else
		m_pfirstChild = pNode;			// 为空的列表

	m_pLastChild = pNode;
	return pNode;
}

bool CNodeData::RemoveChild(CNodeData* pChild)
{
	if (pChild->GetParent() != this)
	{	
		assert(0);
		return false;
	}

	if (pChild->nextSibling())
		pChild->nextSibling()->m_pPrev = pChild->previousSibling();
	else
		m_pLastChild = pChild->previousSibling();

	if (pChild->previousSibling())
		pChild->previousSibling()->m_pNext = pChild->nextSibling();
	else
		m_pfirstChild = pChild->nextSibling();

	delete pChild;
	return true;
}

CNodeData* CNodeData::previousSibling(const std::string& name)const
{
	for (CNodeData* pNode = m_pPrev; pNode; pNode = pNode->previousSibling())
	{
		if(pNode->GetName() == name)
		{
			return pNode;
		}
	}
	return NULL;
}

CNodeData* CNodeData::nextSibling(const std::string& name)const
{
	for (CNodeData* pNode = m_pNext; pNode; pNode = pNode->nextSibling())
	{
		if(pNode->GetName() == name)
		{
			return pNode;
		}
	}
	return NULL;
}

CNodeData* CNodeData::indexChild(unsigned long uIndex)
{
	CNodeData* pChild = firstChild();
	unsigned long uChildIndex = 0;
	while (pChild)
	{
		if (uChildIndex==uIndex)
		{
			return pChild;
		}
		pChild = nextSibling();
		uChildIndex++;
	}
	return NULL;
}

CNodeData* CNodeData::indexChild(unsigned long uIndex, const std::string& name)
{
	CNodeData* pChild = firstChild(name);
	unsigned long uChildIndex = 0;
	while (pChild)
	{
		if (uChildIndex==uIndex)
		{
			return pChild;
		}
		pChild = nextSibling(name);
		uChildIndex++;
	}
	return NULL;
}

CNodeData* CNodeData::firstChild(const std::string& name)const
{
	CNodeData* pNode;
	for (pNode = m_pfirstChild; pNode; pNode = pNode->nextSibling())
	{
		if(pNode->GetName() == name)
		{
			return pNode;
		}
	}
	return NULL;
}

CNodeData* CNodeData::firstChild(unsigned long uIndex)const
{
	char szIndex[25];
	sprintf(szIndex, "[%d]", uIndex);
	return firstChild(szIndex);
}

CNodeData* CNodeData::lastChild(const std::string& name)const
{
	CNodeData* pNode;
	for (pNode = m_pLastChild; pNode; pNode = pNode->previousSibling())
	{
		if(pNode->GetName() == name)
		{
			return pNode;
		}
	}
	return NULL;
}

CNodeData* CNodeData::GetNodeBuffer(const std::string& name, NodeBuffer& lump)
{
	CNodeData* pNode = firstChild(name);
	if (pNode)
	{
		lump = pNode->GetNodeBuffer();
	}
	return pNode;
}

void CNodeData::GetString(std::string& val)const
{
	for (size_t i=0; i<m_NodeBuffer.size; ++i)
	{
		val += ((char*) m_NodeBuffer.buffer)[i];
	}
}

CNodeData* CNodeData::GetString(const std::string& name, std::string& val)const
{
	CNodeData* pNode = firstChild(name);
	if (pNode)
	{
		pNode->GetString(val);
	}
	return pNode;
}

void CNodeData::SetVal(size_t uSize,const void* pVal)
{
	m_NodeBuffer.createBuffer(uSize,pVal);
}

void CNodeData::getVal(size_t uSize, void* pVal)const
{
	if (m_NodeBuffer.size!=uSize)
	{
		return;
	}
	if (uSize>0)
	{
		memcpy(pVal,m_NodeBuffer.buffer, uSize);
	}
}

CNodeData*	CNodeData::AddNode(unsigned long uIndex)
{
	char szIndex[25];
	sprintf(szIndex, "[%d]", uIndex);
	return AddNode(szIndex);
}

CNodeData* CNodeData::AddNode(const std::string& name)
{
	CNodeData* pNode = new CNodeData();
	LinkEndChild(pNode);
	pNode->SetName(name);
	return pNode;
}

CNodeData* CNodeData::SetVal(const std::string& name, size_t uSize,const void* pVal)
{
	CNodeData* pNode = firstChild(name);
	if (pNode)
	{
		pNode->SetVal(uSize, pVal);
	}
	else
	{
		pNode = AddNode(name);
		pNode->SetVal(uSize, pVal);
	}
	return pNode;
}

CNodeData* CNodeData::AddNode(unsigned long uIndex, const std::string& name)
{
	CNodeData* pNode = new CNodeData();
	LinkEndChild(pNode);
	for (unsigned long i=0; i<=uIndex; ++i)
	{
		CNodeData* pNode = indexChild(i, name);
		if (pNode)
		{
		}
	}
	pNode->SetName(name);
	return pNode;
}

CNodeData*	CNodeData::SetVal(unsigned long uIndex, const std::string& name, size_t uSize,const void* pVal)
{
	CNodeData* pNode = indexChild(uIndex, name);
	if (pNode)
	{
		pNode->SetVal(uSize, pVal);
	}
	else
	{
		pNode = AddNode(uIndex, name);
		pNode->SetVal(uSize, pVal);
	}
	return pNode;
}