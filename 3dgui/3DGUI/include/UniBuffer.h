// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "UIControl.h"
#include "ScriptStringAnalysis.h"

class CUniBuffer
{
public:
	CUniBuffer(int nInitialSize = 1);
	~CUniBuffer();

	size_t	GetTextSize()  { return m_wstrBuffer.length(); }
	const WCHAR* GetBuffer()const { return m_wstrBuffer.c_str(); }
	const WCHAR& operator[](int n) const { return m_wstrBuffer[n]; }
	WCHAR& operator[](int n);
	void Clear();

	bool InsertChar(int nIndex, WCHAR wChar); // Inserts the char at specified index. If nIndex == -1, insert to the end.
	bool RemoveChar(int nIndex,size_t count=1);  // Removes the char at specified index. If nIndex == -1, remove the last char.
	bool InsertString(unsigned int uIndex, const WCHAR *pStr, int nCount = -1);  // Inserts the first nCount characters of the string pStr at specified index.  If nCount == -1, the entire string is inserted. If nIndex == -1, insert to the end.
	bool SetText(const wchar_t* wcsText);

	// Uniscribe
	void CPtoX(int nCP, bool bTrail, int& nX);
	bool XtoCP(int nX, int& nCP, bool& bTrail);
	void GetPriorItemPos(int nCP, int *pPrior);
	void GetNextItemPos(int nCP, int *pPrior);

private:
	void Analyse();      // Uniscribe -- Analyse() analyses the string in the buffer

	std::wstring m_wstrBuffer;
	// Uniscribe-specific
	//UIFontNode* m_pFontNode;          // Font node for the font that this buffer uses
	bool m_bAnalyseRequired;            // True if the string has changed since last analysis.
	CScriptStringAnalysis m_Analysis;
};