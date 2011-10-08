// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "UIStyle.h"
#include "ScriptStringAnalysis.h"

#if defined(_DEBUG)
#pragma comment(lib, "tinyxmld.lib")
#else
#pragma comment(lib, "tinyxml.lib")
#endif

class UIGraph
{
public:
	UIGraph(void){m_pInstace=this;}
	~UIGraph(void){}
public:
	static UIGraph& getInstance()
	{
		if (NULL == m_pInstace)
		{
			MessageBoxA(NULL,"The GUI graph has no instance.","error",0);
		}
		return  *m_pInstace;
	}
	virtual void frameUpdate()=0;
	virtual void* createTexture(const char* szTexture)=0;
	virtual void releaseTexture(void* pTexture)=0;
	virtual void drawSprite(const RECT* rcDest, void* pTexture, Color32 color=0xFFFFFFFF, const RECT* rcSrc=NULL, const RECT* rcCenterSrc=NULL)=0;
	virtual void CalcTextRect(const wchar_t* wcsText, CRect<float>& rcDest)=0;
	virtual void initDrawText()=0;
	virtual void DrawText(const wchar_t* wcsText, CUIStyle& style, int nIndex, const CRect<float>& rcDest, int nCount = -1)=0;
	virtual void drawText(const wchar_t* wcsText, int cchText, const RECT& rc, UINT format, unsigned long color = 0xffffffff, RECT* prcRet=NULL)=0;
	virtual void initFont(const char* szFontFilename, size_t size)=0;
	virtual int	GetFontSize()=0;
	virtual Matrix setUIMatrix(const Matrix& mTransform, const CRect<float>& rc, const MY3DGUI_VEC3D& vTranslation, const MY3DGUI_VEC3D& vRotate)=0;
	virtual bool scriptStringAnalyse(CScriptStringAnalysis& analysis, const wchar_t* wcsText)=0;
	virtual void playSound(const char* szFilename)=0;
protected:
	static UIGraph* m_pInstace;
};