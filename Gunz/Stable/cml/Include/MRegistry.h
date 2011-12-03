#ifndef MRegistry_H
#define MRegistry_H

class MRegistry {
public:
	static char*	szRegistryBasePath;
	static char*	szApplicationName;
public:
	// hRegKey = HKEY_LOCAL_MACHINE or HKEY_CURRENT_USER
	static bool Read(HKEY hRegKey, const char* szRegString, char* pOutBuffer);
	static void Write(HKEY hRegKey, const char* szRegString, const char* pInBuffer);
	static bool ReadBinary(HKEY hRegKey, const char* szRegString, char* pOutBuffer, DWORD* pdwBufferLen);
	static void WriteBinary(HKEY hRegKey, const char* szRegString, const char* pInBuffer, DWORD dwBufferLen);

	// ��θ� ���� ���ټ� �ִ� �Ϲ����� ���
	static bool Read(HKEY hRegKey, const char* szPath, const char* szValueName, char* pOutBuffer);
	static void Write(HKEY hRegKey, const char* szPath, const char* szValueName, const char* szValue);
};


#endif