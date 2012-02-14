#pragma once
#include "RenderSystemCommon.h"
#include <list>

class CVertexDeclaration
{
public:
	CVertexDeclaration();
	virtual ~CVertexDeclaration();
public:
	void AddElement(unsigned short uSource, unsigned short uOffset, VertexElementType type, VertexElementSemantic semantic, unsigned char uIndex = 0);
	virtual void EndElement()=0;
protected:
	std::list<VertexElement> m_ElementList;
};