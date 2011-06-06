#pragma once
#include "VertexDeclaration.h"
#include "D3D9RenderSystemCommon.h"

class CD3D9VertexDeclaration : public CVertexDeclaration 
{
public:
	CD3D9VertexDeclaration(LPDIRECT3DDEVICE9 pD3D9Device);
	~CD3D9VertexDeclaration();
	void EndElement();
	LPDIRECT3DVERTEXDECLARATION9 GetD3D9Decl(){return m_pD3DDecl;};
protected:
	LPDIRECT3DDEVICE9 m_pD3D9Device;
	LPDIRECT3DVERTEXDECLARATION9 m_pD3DDecl;
};