#include "RegData.h"
#include <windows.h>

std::wstring GetRegStr(const wchar_t* wcsKey,const wchar_t* wcsName)
{
	// read the recent path from reg.
	HKEY hKey;
	if (ERROR_SUCCESS==RegOpenKeyExW(HKEY_LOCAL_MACHINE,wcsKey,//L"software\\rpgsky\\worldeditor\\",
		0, KEY_READ, &hKey))
	{
		DWORD dwType = REG_SZ;
		wchar_t wszFilename[256];
		DWORD dwSize = sizeof(wszFilename);

		if (ERROR_SUCCESS==RegQueryValueExW(hKey, wcsName,
			NULL, &dwType, (PBYTE)&wszFilename, &dwSize))
		{
			RegCloseKey(hKey);
			return wszFilename;
		}
		RegCloseKey(hKey);
	}
	return L"";
}

void SetRegStr(const wchar_t* wcsKey,const wchar_t* wcsName,const wchar_t* wcsValue)
{
	// write the recent path to reg.
	HKEY hKey;
	if (ERROR_SUCCESS==RegCreateKeyExW(HKEY_LOCAL_MACHINE,wcsKey,
		NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&hKey,NULL))
	{
		RegSetValueExW(hKey,wcsName,0,REG_SZ,(LPBYTE)wcsValue,sizeof(wchar_t)*wcslen(wcsValue));
		RegCloseKey(hKey);
	}
}