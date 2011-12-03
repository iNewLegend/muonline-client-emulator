#include "stdafx.h"
#include <windows.h>
#include "MRegistry.h"
#include <stdio.h>

char* MRegistry::szRegistryBasePath = "SOFTWARE\\MAIET entertainment\\";
char* MRegistry::szApplicationName = "Unknown";

bool MRegistry::Read(HKEY hRegKey, const char* szRegString, char* pOutBuffer)
{
	HKEY hKey;
	char szValue[_MAX_PATH] = "";
	DWORD nLen = _MAX_PATH;
	DWORD dwType = REG_SZ;

	char szRegistryPath[256];
	sprintf(szRegistryPath, "%s%s", szRegistryBasePath, szApplicationName);
	
	// Extract patch address in registry.
	if (RegOpenKeyEx(hRegKey, szRegistryPath, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
#ifdef _DEBUG
		OutputDebugString("Can't find a registry Key:");
		OutputDebugString(szRegistryPath);
		OutputDebugString("\n");
#endif
		return false;
	}
	if (RegQueryValueEx(hKey, szRegString, 
			NULL, &dwType, (unsigned char *)szValue, &nLen) != ERROR_SUCCESS) {
#ifdef _DEBUG
		OutputDebugString("Can't find a registry ID:");
		OutputDebugString(szRegString);
		OutputDebugString("\n");
		RegCloseKey(hKey);
#endif
		return false;
	}
	strcpy(pOutBuffer, (char*)szValue);
	RegCloseKey(hKey);
	return true;
}

void MRegistry::Write(HKEY hRegKey, const char* szRegString, const char* pInBuffer)
{
	HKEY hKey;
	DWORD dwType = REG_SZ;

	char szRegistryPath[256];
	sprintf(szRegistryPath, "%s%s", szRegistryBasePath, szApplicationName);
	if (RegOpenKeyEx(hRegKey, szRegistryPath, 0, KEY_WRITE, &hKey ) != ERROR_SUCCESS ) {
		RegCreateKey( hRegKey, szRegistryPath, &hKey);
	}
	RegSetValueEx(hKey, szRegString, NULL, dwType, (LPBYTE)pInBuffer, strlen(pInBuffer));
	RegCloseKey( hKey );
}

bool MRegistry::ReadBinary(HKEY hRegKey, const char* szRegString, char* pOutBuffer, DWORD* pdwBufferLen)
{
	HKEY hKey;
	DWORD dwType = REG_BINARY;

	char szRegistryPath[256];
	sprintf(szRegistryPath, "%s%s", szRegistryBasePath, szApplicationName);
	
	// Extract patch address in registry.
	if (RegOpenKeyEx(hRegKey, szRegistryPath, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
#ifdef _DEBUG
		OutputDebugString("Can't find a registry Key:");
		OutputDebugString(szRegistryPath);
		OutputDebugString("\n");
#endif
		return false;
	}
	int nResult = RegQueryValueEx(hKey, szRegString, 
								NULL, &dwType, (LPBYTE)pOutBuffer, pdwBufferLen);
	if (nResult != ERROR_SUCCESS) {
#ifdef _DEBUG
		if (nResult == ERROR_MORE_DATA)
			OutputDebugString("MORE_DATA!\n");
		char szError[128];
		wsprintf(szError, "Can't read a registry. Error=%d", GetLastError());
		OutputDebugString(szError);
#endif

		RegCloseKey(hKey);
		return false;
	}
	RegCloseKey(hKey);
	return true;
}

void MRegistry::WriteBinary(HKEY hRegKey, const char* szRegString, const char* pInBuffer, DWORD dwBufferLen)
{
	HKEY hKey;
	DWORD dwType = REG_BINARY;

	char szRegistryPath[256];
	sprintf(szRegistryPath, "%s%s", szRegistryBasePath, szApplicationName);
	if (RegOpenKeyEx(hRegKey, szRegistryPath, 0, KEY_WRITE, &hKey ) != ERROR_SUCCESS ) {
		RegCreateKey(hRegKey, szRegistryPath, &hKey);
	}
	RegSetValueEx(hKey, szRegString, NULL, dwType, (LPBYTE)pInBuffer, dwBufferLen);
	RegCloseKey( hKey );
}


//////////	��θ� ���� ���ִ� ���
bool MRegistry::Read(HKEY hRegKey, const char* szPath, const char* szValueName, char* pOutBuffer)
{
	HKEY hKey;
	char szValue[_MAX_PATH] = "";
	DWORD nLen = _MAX_PATH;
	DWORD dwType = REG_SZ;

	// Extract patch address in registry.
	if (RegOpenKeyEx(hRegKey, szPath, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
#ifdef _DEBUG
		OutputDebugString("Can't find a registry Key:");
		OutputDebugString(szPath);
		OutputDebugString("\n");
#endif
		return false;
	}
	if (RegQueryValueEx(hKey, szValueName, 
		NULL, &dwType, (unsigned char *)szValue, &nLen) != ERROR_SUCCESS) {
#ifdef _DEBUG
		OutputDebugString("Can't find a registry ID:");
		OutputDebugString(szValueName);
		OutputDebugString("\n");
#endif
		RegCloseKey(hKey);
		return false;
	}

	strcpy(pOutBuffer, (char*)szValue);
	RegCloseKey(hKey);
	return true;
}

void MRegistry::Write(HKEY hRegKey, const char* szPath, const char* szValueName, const char* szValue)
{
	HKEY hKey;
	DWORD dwType = REG_SZ;

	if (RegOpenKeyEx(hRegKey, szPath, 0, KEY_WRITE, &hKey ) != ERROR_SUCCESS ) {
		RegCreateKey( hRegKey, szPath, &hKey);
	}
	RegSetValueEx(hKey, szValueName, NULL, dwType, (LPBYTE)szValue, strlen(szValue));
	RegCloseKey( hKey );
}

