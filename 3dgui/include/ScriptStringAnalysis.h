// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include <vector>

class CScriptStringAnalysis
{
public: 
	CScriptStringAnalysis();
	~CScriptStringAnalysis();
public:
	void CPToX(int nCP, bool bTrail, int& nX);
	void XtoCP(int nX, int& nCP, bool& bTrail);
//protected:
	std::vector<unsigned short int> m_CharPos;
};