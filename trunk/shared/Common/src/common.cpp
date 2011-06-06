// 常量和配置
// 仙剑修，2002.9.14

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "common.h"
///////
// function///////
bool	SafeCopy(char * pTarget, const char * pSource, int nBufLen /*= 0*/)
{
	try{
		if(pTarget)
		{
			pTarget[0] = 0;
			if(pSource)
			{
				if(nBufLen && (int)strlen(pSource) >= nBufLen)
				{
					strncpy(pTarget, pSource, nBufLen-1);
					pTarget[nBufLen-1] = 0;
					return false;
				}
				strcpy(pTarget, pSource);
				return true;
			}
		}
	}catch(...){ ::OutputDebugString("CATCH: *** SafeCopy() crash! ***\n"); }

	assert(!"SafeCopy()");
	return false;
}

std::string ws2s(const std::wstring& ws)
{
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest,0,_Dsize);
	wcstombs(_Dest,_Source,_Dsize);
	std::string result = _Dest;
	delete []_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

std::wstring s2ws(const std::string& s)
{
	setlocale(LC_ALL, "chs"); 
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t *_Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
	mbstowcs(_Dest,_Source,_Dsize);
	std::wstring result = _Dest;
	delete []_Dest;
	setlocale(LC_ALL, "C");
	return result;
}

std::wstring i2ws(int i, int nRadix)
{
	char strText[256];
	memset(strText,'\0',256);
	itoa(i,strText,nRadix);
	return s2ws(strText);
}

std::wstring f2ws(float f,const int slen,const int alen)
{
	char strText[256];
	memset(strText,'\0',256);
	sprintf(strText,"%*.*lf",slen,alen,f); //-- 这里控制精度
	return s2ws(strText);
}

std::string Format(const char* pszFormat, ...)
{
	std::string str;
	try
	{
		va_list args;
		va_start(args, pszFormat);
		unsigned int nLength = _vscprintf(pszFormat, args);
		str.resize(nLength);
		_vsnprintf_s(const_cast<char*>(str.c_str()), nLength+1, _TRUNCATE, pszFormat, args);
		va_end(args);
	}
	catch (...)
	{
		str.clear();
	}
	return str;
}

void Tokenizer(const std::string& in, std::vector<std::string>& out, const std::string& delimiters)
{
	size_t offset=0;
	for(;;)
	{
		size_t i = in.find_first_of(delimiters,offset);
		if (i == std::string::npos)
		{
			out.push_back(in.substr(offset));
			return;
		}
		out.push_back(in.substr(offset,i-offset));
		offset = i+delimiters.length();
	}
}

void TokenizerW(const std::wstring& in, std::vector<std::wstring>& out, const std::wstring& delimiters)
{
	size_t offset=0;
	for(;;)
	{
		size_t i = in.find_first_of(delimiters,offset);
		if (i == std::wstring::npos)
		{
			out.push_back(in.substr(offset));
			return;
		}
		out.push_back(in.substr(offset,i-offset));
		offset = i+delimiters.length();
	}
}

std::wstring FormatW(const wchar_t* pwszFormat, ...)
{
	std::wstring wstr;
	try
	{
		va_list args;
		va_start(args, pwszFormat);
		unsigned int nLength = _vscwprintf(pwszFormat, args);
		wstr.resize(nLength);
		_vsnwprintf_s(const_cast<wchar_t*>(wstr.c_str()), nLength+1, _TRUNCATE, pwszFormat, args);
		va_end(args);
	}
	catch (...)
	{
		wstr.clear();
	}
	return wstr;
}

#define  BYTE2HEX(x) ((x)>=10?((x)-10+'a'):((x)+'0'))
#define  HEX2BYTE(x) ((x)>='a'?((x)+10-'a'):((x)-'0'))

void hexStdStr2Date(const std::string& str, void* pDate, size_t size)
{
	memset(pDate,0,size);
	unsigned char* p = (unsigned char*)pDate;
	for (size_t i=0;i<str.length()/2&&i<size;++i)
	{
		p[i] = (HEX2BYTE(str[i*2])<<4)+HEX2BYTE(str[i*2+1]);
	}
}

std::string date2HexStdStr(void* pDate, size_t size)
{
	std::string str;
	unsigned char* p = (unsigned char*)pDate;
	for (size_t i=0;i<size;++i)
	{
		str.push_back(BYTE2HEX(p[i]>>4));
		str.push_back(BYTE2HEX(p[i]&0xF));
	}
	return str;
}