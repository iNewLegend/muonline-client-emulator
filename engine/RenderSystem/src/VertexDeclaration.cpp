#include "VertexDeclaration.h"
//#include "RenderSystem.h"

CVertexDeclaration::CVertexDeclaration()
{
}

CVertexDeclaration::~CVertexDeclaration()
{
}

void CVertexDeclaration::AddElement(unsigned short uSource, unsigned short uOffset, VertexElementType type, VertexElementSemantic semantic, unsigned char uIndex)
{
	VertexElement vertexElement = {uSource, uOffset, type, semantic, uIndex};
	m_ElementList.push_back(vertexElement);
}