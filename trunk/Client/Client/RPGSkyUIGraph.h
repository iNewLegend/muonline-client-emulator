//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* RPGSkyUIGraph.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "UIGraph.h"
#include "TextRender.h"
#include "RPGSkyTextRender.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class RPGSkyUIGraph : public UIGraph
{
public:
	void	frameUpdate			();
	// ----
	void*	createTexture		(const char* szTexture);
	void	releaseTexture		(void* pTexture);
	// ----
	void	DrawSprite			(const CRect<float>& rcSrc, const CRect<float>& rcDest, void* pTexture, Color32 color);
	void	DrawSprite			(const CRect<float>& rcDest, void* pTexture, Color32 color);
	void	DrawSprite3x3Grid	(const CRect<float>& rcSrc, const CRect<float>& rcCenterSrc, const CRect<float>& rcDest,void* pTexture, Color32 color);
	// ----
	void	CalcTextRect		(const wchar_t* wcsText, CRect<float>& rcDest);
	// ----
	void	initDrawText		();
	void	DrawText			(const wchar_t* wcsText, CUIStyle& style, int nIndex, const CRect<float>& rcDest, int nCount = -1);
	void	drawText			(const wchar_t* wcsText, int cchText, const RECT& rc, UINT format, unsigned long color = 0xffffffff, RECT* prcRet=NULL);
	// ----
	void	initFont			(const char* szFontFilename, size_t size);
	// ----
	int		GetFontSize			();
	// ----
	Matrix	setUIMatrix			(const Matrix& mTransform, const CRect<float>& rc, const Vec3D& vTranslation, const Vec3D& vRotate);
	// ----
	bool	scriptStringAnalyse(CScriptStringAnalysis& analysis, const wchar_t* wcsText);
	// ----
	void	playSound			(const char* szFilename);
	// ----
	CTextRender & getTextRender();
	CRPGSkyTextRender3D s_RPGSkyTextRender;
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------