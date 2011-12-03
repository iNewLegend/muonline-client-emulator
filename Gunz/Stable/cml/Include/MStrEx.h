// MAIET String Extension
// ��Ʈ�� ���� ���� ���

#ifndef MSTREX_H
#define MSTREX_H

#include <ctype.h>


/// Ÿ�� ��Ʈ���� �ƽ��� ���̸�ŭ������ ��Ʈ���� ī���Ѵ�. ( NULL���ڱ��� ���� )
/// @param szDest	Ÿ�� ���� ����
/// @param nDestLen	Ÿ�� ���� ���� ũ��
/// @param szSource	�ҽ� ���ڿ�
void MStrNCpy(char* szDest, int nDestLen, const char* szSource);

/// ��Ʈ������ �ܾ������ �о�´�
class MStringCutter {
protected:
	static void SkipSpaces(char **szString) {
		for (; **szString && isspace(**szString); (*szString)++);
	}

public:
	static char* MStringCutter::GetOneArg(char *pszArg, char *pszOutArg) {
		SkipSpaces(&pszArg);
		while(*pszArg && !isspace(*pszArg)) {
			*(pszOutArg++) = *pszArg;
			pszArg++;
		}
		*pszOutArg = '\0';
		return pszArg;
	}

	static char* MStringCutter::GetTwoArgs(char* pszArg, char* pszOutArg1, char* pszOutArg2) {
		return GetOneArg(GetOneArg(pszArg, pszOutArg1), pszOutArg2);
	}
};

#endif