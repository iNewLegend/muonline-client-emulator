#include "StrRes.h"
#include "CSVFile.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CStrRes::CStrRes()
{
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CStrRes::~CStrRes()
{
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CStrRes::load(const char* szFilename)
{
	m_mapStr.clear();
	// ----
	CCsvFile csvStrRes;
	// ----
	if (csvStrRes.Open(szFilename))
	{
		// ----
		while (csvStrRes.SeekNextLine())
		{
			const char* szSign		= csvStrRes.GetStr("index");
			const char* szText		= csvStrRes.GetStr("text");
			// ----
			m_mapStr[szSign] = szText;
		}
		// ----
		csvStrRes.Close();
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const std::string& CStrRes::getStr(const char* szIndex)const
{
	static const std::string s_strStrResError = "ERROR";
	// ----
	std::map<std::string, std::string>::const_iterator it = m_mapStr.find(szIndex);
	// ----
	if (it!=m_mapStr.end())
		return it->second;
	// ----
	return s_strStrResError;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------