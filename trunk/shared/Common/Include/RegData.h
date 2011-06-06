#pragma once
#include <string>
#include <vector>

std::wstring GetRegStr(const wchar_t* wcsKey,const wchar_t* wcsName);
void SetRegStr(const wchar_t* wcsKey,const wchar_t* wcsName,const wchar_t* wcsValue);