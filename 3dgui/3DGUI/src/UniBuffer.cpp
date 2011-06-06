#include "UniBuffer.h"

// Uniscribe -- Analyse() analyses the string in the buffer
void CUniBuffer::Analyse()
{
	if(m_bAnalyseRequired)
	{
		m_bAnalyseRequired = !UIGraph::getInstance().scriptStringAnalyse(m_Analysis, m_wstrBuffer.c_str());
		//m_bAnalyseRequired = !m_Analysis.ScriptStringAnalyse(m_wstrBuffer);
	}
}

CUniBuffer::CUniBuffer(int nInitialSize)
{
	m_bAnalyseRequired = true;
}

CUniBuffer::~CUniBuffer()
{
}

WCHAR& CUniBuffer::operator[](int n)  // No param checking
{
	// This version of operator[] is called only
	// if we are asking for write access, so
	// re-analysis is required.
	m_bAnalyseRequired = true;
	return m_wstrBuffer[n];
}

void CUniBuffer::Clear()
{
	m_wstrBuffer.clear();
	m_bAnalyseRequired = true;
}

// Inserts the char at specified index.
// If nIndex == -1, insert to the end.
#include "assert.h"
bool CUniBuffer::InsertChar(int nIndex, WCHAR wChar)
{
	assert(nIndex >= 0);

	if(nIndex < 0 || nIndex > m_wstrBuffer.length())
		return false;  // invalid index

	m_wstrBuffer.insert(m_wstrBuffer.begin()+nIndex, wChar);
	m_bAnalyseRequired = true;
	return true;
}

// Removes the char at specified index.
// If nIndex == -1, remove the last char.

bool CUniBuffer::RemoveChar(int nIndex,size_t count)
{
	if(m_wstrBuffer.length()==0 || nIndex >= m_wstrBuffer.length())
		return false;  // Invalid index

	m_wstrBuffer.erase(nIndex,count);
	m_bAnalyseRequired = true;
	return true;
}

// Inserts the first nCount characters of the string pStr at specified index.
// If nCount == -1, the entire string is inserted.
// If nIndex == -1, insert to the end.

bool CUniBuffer::InsertString(unsigned int uIndex, const WCHAR *pStr, int nCount)
{
	if(uIndex > m_wstrBuffer.length())
		return false;  // invalid index

	if(-1 == nCount)
		nCount = lstrlenW(pStr);

	m_wstrBuffer.insert(uIndex, pStr);
	m_bAnalyseRequired = true;

	return true;
}

bool CUniBuffer::SetText(const wchar_t* wcsText)
{
	m_wstrBuffer = wcsText;
	m_bAnalyseRequired = true;
	return true;
}

void CUniBuffer::CPtoX(int nCP, bool bTrail, int& nX)
{
	nX = 0;  // Default
	Analyse();
	m_Analysis.CPToX(nCP, bTrail, nX);
}

bool CUniBuffer::XtoCP(int nX, int& nCP, bool& bTrail)
{
	nCP = 0;
	bTrail = false;  // Default
	Analyse();
	m_Analysis.XtoCP(nX, nCP, bTrail);

	// If the coordinate falls outside the text region, we
	// can get character positions that don't exist.  We must
	// filter them here and convert them to those that do exist.
	if(nCP == -1 && bTrail == true)
	{
		nCP = 0; bTrail = false;
	}
	else if(nCP > m_wstrBuffer.length() && bTrail == false)
	{
		nCP = m_wstrBuffer.length(); bTrail = true;
	}

	return true;
}

inline bool IsWord(wchar_t wchar)
{
	if ((wchar>=L'0'&&wchar<=L'9')||
		(wchar>=L'a'&&wchar<=L'z')||
		(wchar>=L'A'&&wchar<=L'Z')||
		wchar>0xFF)
	{
		return true;
	}
	return false;
}

void CUniBuffer::GetPriorItemPos(int nCP, int *pPrior)
{
 	*pPrior = nCP;  // Default is the char itself
 
	//Analyse();
// 
// 	const SCRIPT_LOGATTR *pLogAttr = _ScriptString_pLogAttr(m_Analysis);
// 	if(!pLogAttr)
// 		return;
// 
// 	if(!_ScriptString_pcOutChars(m_Analysis))
// 		return;
 	int nInitial = nCP-1;//*_ScriptString_pcOutChars(m_Analysis);
	// 	if(nCP - 1 < nInitial)
	// 		nInitial = nCP - 1;

	for(int i = nInitial; i > 0; --i)
	{
		if (!IsWord(m_wstrBuffer[i]))
		{
			if (IsWord(m_wstrBuffer[nInitial]))
			{
				*pPrior = i+1;
				return;
			}
			for(int n = i; n > 0; --n)
			{
				if (IsWord(m_wstrBuffer[n]))
				{
					*pPrior = n+1;
					return;
				}
			}
			break;
		}
	}
 	*pPrior = 0;
}



void CUniBuffer::GetNextItemPos(int nCP, int *pPrior)
{
 	*pPrior = nCP;  // Default is the char itself
	int nInitial = nCP;
	int nStrLength = m_wstrBuffer.length();
	for(int i = nInitial; i < nStrLength; ++i)
	{
		if (!IsWord(m_wstrBuffer[i]))
		{
			if (IsWord(m_wstrBuffer[nInitial]))
			{
				*pPrior = i;
				return;
			}
			for(int n = i; n < nStrLength; ++n)
			{
				if (IsWord(m_wstrBuffer[n]))
				{
					*pPrior = n;
					return;
				}
			}
			break;
		}
	}
	*pPrior = nStrLength;
// 
// 	HRESULT hr = S_OK;
	//Analyse();
// 	if(FAILED(hr))
// 		return;
// 
// 	const SCRIPT_LOGATTR *pLogAttr = _ScriptString_pLogAttr(m_Analysis);
// 	if(!pLogAttr)
// 		return;
// 
// 	if(!_ScriptString_pcOutChars(m_Analysis))
// 		return;
// 	int nInitial = *_ScriptString_pcOutChars(m_Analysis);
// 	if(nCP + 1 < nInitial)
// 		nInitial = nCP + 1;
// 	for(int i = nInitial; i < *_ScriptString_pcOutChars(m_Analysis) - 1; ++i)
// 	{
// 		if(pLogAttr[i].fWordStop)      // Either the fWordStop flag is set
// 		{
// 			*pPrior = i;
// 			return;
// 		}
// 		else
// 			if(pLogAttr[i].fWhiteSpace &&  // Or this whitespace but the next char isn't.
// 				!pLogAttr[i+1].fWhiteSpace)
// 			{
// 				*pPrior = i+1;  // The next char is a word stop
// 				return;
// 			}
// 	}
// 	// We have reached the end. It's always a word stop, so simply return it.
// 	*pPrior = *_ScriptString_pcOutChars(m_Analysis) - 1;
}
