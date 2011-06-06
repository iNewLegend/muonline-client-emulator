//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* RPGSkyTextRender.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "TextRender.h"
#include "Texture.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define DRAW_TEXT_MAX	1024 * 8
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CRPGSkyTextRender : public CTextRender
{
public:
	CRPGSkyTextRender();
	// ----
	virtual void updateTextureBuffer	(unsigned char * pBuffer, size_t size, size_t width, size_t height);
	virtual void drawTextVertexBuffer	(int nVertexCount, void * pBuffer);
protected:
	CTexture*	 m_pTextTexture;
};

class CRPGSkyTextRender3D : public CTextRender
{
public:
	CRPGSkyTextRender3D();
	// ----
	virtual void updateTextureBuffer	(unsigned char * pBuffer, size_t size, size_t width, size_t height);
	virtual void drawTextVertexBuffer	(int nVertexCount, void * pBuffer);
protected:
	CTexture*	 m_pTextTexture;
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------