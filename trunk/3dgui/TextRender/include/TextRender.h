// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "UBB.h"
#include "ScriptStringAnalysis.h"
#include <map>

class CTextRender
{
public:
	CTextRender();
	~CTextRender();
public:
	void registerFont(const std::wstring& strFontName, const char* szFilename); 
	void load(const char* szFilename);
	void loadFromMemory(unsigned char* pFileBuffer, size_t fileSize);
	void setFontSize(size_t fontSize);
	void setShadowBorder(unsigned int uBorder);
	void setShadowColor(unsigned long uShadowColor);
	void Free();

	void OnFrameMove();

	virtual void updateTextureBuffer(unsigned char* pBuffer, size_t size, size_t width, size_t height)=0;

	void SetInsertionPos(int x, int y) { m_nX = x; m_nY = y; }
	void SetForegroundColor(unsigned long color) { m_dwColor = color; }
	void drawText(const wchar_t* wcsText);

	// ubb
	void buildUBB(CUBB* pUBB, const wchar_t* wcsText, int nShowWidth, int cchText = -1, UINT format = 0, unsigned long dwColor = 0xffffffff);
	virtual void drawUBB(int x, int y, const CUBB* pUBB)const=0;
	void drawText(const wchar_t* wcsText, const RECT& rc, int cchText = -1, UINT format = 0, unsigned long dwColor = 0xffffffff);
	void drawText(const wchar_t* wcsText, int x, int y, unsigned long dwColor = 0xffffffff);
	void calcUBBRect(const wchar_t* wcsText, RECT& rc);
	//
	int GetCharHeight() const;
	const TexCharInfo* GetCharInfo(wchar_t ch);

	//
	bool scriptStringAnalyse(CScriptStringAnalysis& analysis, const wchar_t* wcsText);
protected:
	unsigned long	m_dwColor;
	int				m_nX;
	int				m_nY;
	int				m_nLineHeight;
protected:
	void*			m_library; 
	int				m_nH; 
	void*			m_Face;
	int				m_nShadowBorder;
	unsigned long	m_uShadowColor;
protected:
	int m_nTextTexWidth;
	int m_nTextTexHeight;

	std::map<wchar_t,TexCharInfo> m_CharsInfo;

	bool m_bUpdated;

	std::wstring	m_wstrDefaultFont;
}; 