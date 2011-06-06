#include "D3D9VertexDeclaration.h"

CD3D9VertexDeclaration::CD3D9VertexDeclaration(LPDIRECT3DDEVICE9 pD3D9Device)
: m_pD3D9Device(pD3D9Device), m_pD3DDecl(NULL)
{
}

CD3D9VertexDeclaration::~CD3D9VertexDeclaration()
{
	D3D9S_REL(m_pD3DDecl);
}

inline D3DDECLTYPE VertexElementTypeForD3D9(VertexElementType type)
{
	switch (type)
	{
	case VET_COLOUR:
	case VET_COLOUR_ABGR:
	case VET_COLOUR_ARGB:
		return D3DDECLTYPE_D3DCOLOR;
		break;
	case VET_FLOAT1:
		return D3DDECLTYPE_FLOAT1;
		break;
	case VET_FLOAT2:
		return D3DDECLTYPE_FLOAT2;
		break;
	case VET_FLOAT3:
		return D3DDECLTYPE_FLOAT3;
		break;
	case VET_FLOAT4:
		return D3DDECLTYPE_FLOAT4;
		break;
	case VET_SHORT2:
		return D3DDECLTYPE_SHORT2;
		break;
	case VET_SHORT4:
		return D3DDECLTYPE_SHORT4;
		break;
	case VET_UBYTE4:
		return D3DDECLTYPE_UBYTE4;
		break;
	}
	// to keep compiler happy
	return D3DDECLTYPE_FLOAT3;
}

inline D3DDECLUSAGE VertexElementSemanticForD3D9(VertexElementSemantic semantic)
{
	switch (semantic)
	{
	case VES_BLEND_INDICES:
		return D3DDECLUSAGE_BLENDINDICES;
		break;
	case VES_BLEND_WEIGHTS:
		return D3DDECLUSAGE_BLENDWEIGHT;
		break;
	case VES_DIFFUSE:
		return D3DDECLUSAGE_COLOR; // NB index will differentiate
		break;
	case VES_SPECULAR:
		return D3DDECLUSAGE_COLOR; // NB index will differentiate
		break;
	case VES_NORMAL:
		return D3DDECLUSAGE_NORMAL;
		break;
	case VES_POSITION:
		return D3DDECLUSAGE_POSITION;
		break;
	case VES_TEXTURE_COORDINATES:
		return D3DDECLUSAGE_TEXCOORD;
		break;
	case VES_BINORMAL:
		return D3DDECLUSAGE_BINORMAL;
		break;
	case VES_TANGENT:
		return D3DDECLUSAGE_TANGENT;
		break;
	}
	// to keep compiler happy
	return D3DDECLUSAGE_POSITION;
}

void CD3D9VertexDeclaration::EndElement()
{
	D3D9S_REL(m_pD3DDecl);
	// Create D3D elements
	D3DVERTEXELEMENT9* d3delems = new D3DVERTEXELEMENT9[m_ElementList.size() + 1];

	std::list<VertexElement>::const_iterator i, iend;
	unsigned int idx;
	iend = m_ElementList.end();
	for (idx = 0, i = m_ElementList.begin(); i != iend; ++i, ++idx)
	{
		d3delems[idx].Method = D3DDECLMETHOD_DEFAULT;
		d3delems[idx].Offset = static_cast<WORD>(i->uOffset);
		d3delems[idx].Stream = i->uStream;
		d3delems[idx].Type = VertexElementTypeForD3D9(i->Type);
		d3delems[idx].Usage = VertexElementSemanticForD3D9(i->Semantics);
		d3delems[idx].UsageIndex = i->uIndex;
	}
	// Add terminator
	d3delems[idx].Stream = 0xff;
	d3delems[idx].Offset = 0;
	d3delems[idx].Type = D3DDECLTYPE_UNUSED;
	d3delems[idx].Method = 0;
	d3delems[idx].Usage = 0;
	d3delems[idx].UsageIndex = 0;

	D3D9HR( m_pD3D9Device->CreateVertexDeclaration(d3delems, &m_pD3DDecl) );

	delete [] d3delems;
}