#include "ScriptStringAnalysis.h"

CScriptStringAnalysis::CScriptStringAnalysis()
{
}

CScriptStringAnalysis::~CScriptStringAnalysis()
{
}

void CScriptStringAnalysis::CPToX(int nCP, bool bTrail, int& nX)
{
	// bTrail Î²²¿±ßÔµ
	if (!bTrail)
	{
		nCP--;
	}
	if (nCP<0)
	{
		nX = 0;
	}
	else if (m_CharPos.size() > nCP)
	{
		nX = m_CharPos[ nCP ];
	}
}

void CScriptStringAnalysis::XtoCP(int nX, int& nCP, bool& bTrail)
{
	if (nX < 0)
	{
		bTrail = false;
		nCP = 0;
		return;
	}
	else
	{
		bTrail = true;
	}

	for (nCP = 0; nCP < m_CharPos.size(); nCP++)
	{
		if (m_CharPos[ nCP ] > nX)
		{
			nCP--;
			return;
		}
	}
}