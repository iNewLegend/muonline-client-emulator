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

unsigned long CVertexDeclaration::GetTypeSize(VertexElementType type)
{
	switch(type)
	{
	case VET_COLOUR:
	case VET_COLOUR_ABGR:
	case VET_COLOUR_ARGB:
		return sizeof(Color32);
	case VET_FLOAT1:
		return sizeof(float);
	case VET_FLOAT2:
		return sizeof(float)*2;
	case VET_FLOAT3:
		return sizeof(float)*3;
	case VET_FLOAT4:
		return sizeof(float)*4;
	case VET_SHORT1:
		return sizeof(short);
	case VET_SHORT2:
		return sizeof(short)*2;
	case VET_SHORT3:
		return sizeof(short)*3;
	case VET_SHORT4:
		return sizeof(short)*4;
	case VET_UBYTE4:
		return sizeof(unsigned char)*4;
	}
	return 0;
}