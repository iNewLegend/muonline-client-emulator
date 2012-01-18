#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "common.h"

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

// std::string Format(const char* pszFormat, ...)
// {
// 	std::string str;
// 	try
// 	{
// 		va_list args;
// 		va_start(args, pszFormat);
// 		unsigned int nLength = _vscprintf(pszFormat, args);
// 		str.resize(nLength);
// 		_vsnprintf_s(const_cast<char*>(str.c_str()), nLength+1, _TRUNCATE, pszFormat, args);
// 		va_end(args);
// 	}
// 	catch (...)
// 	{
// 		str.clear();
// 	}
// 	return str;
// }