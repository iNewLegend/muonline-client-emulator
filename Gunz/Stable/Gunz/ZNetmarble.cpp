#include "stdafx.h"

#include <windows.h>
#include <crtdbg.h>
#include "ZNetmarble.h"
#include "ZLocale.h"
#include "MDebug.h"
#include "MLex.h"


//// ZNetmarbleAuthInfo ////////////////

ZNetmarbleAuthInfo::ZNetmarbleAuthInfo() : ZBaseAuthInfo()
{
	m_AuthCookie[0] = NULL;
	m_DataCookie[0] = NULL;
	strcpy( m_SpareParam, "12" );
}

ZNetmarbleAuthInfo::~ZNetmarbleAuthInfo()
{
}

////////////////ZNetmarbleAuthInfo ////

#ifdef LOCALE_KOREA

/* [ Ŭ���̾�Ʈ �۾� ���� ]
---------------------------------------------------------------------------------------------

1. Ŭ���̾�Ʈ������ Ŭ���̾�Ʈ ��ȣȭ ����� NMCrypt�� ����ؾ��Ѵ�.

  ����) ������ ��ȣȭ ����� CPSSOLib ���̺귯���� DLL�� �ݵ�� ���������� ����ؾ��Ѵ�.
        �����!! Ŭ���̾�Ʈ�� �ڵ�� DLL�� ���Խ�Ű�� �ȵ�.
        (Ŭ���̾�Ʈ�� �ڵ忡 ���Եǰ� �Ǹ� disassemble������ �ڵ� ������ ������)

        Ŭ���̾�Ʈ���� ID�� �˰��� �ϴ� ���� �ϴ��� 
        �ݵ�� �������� ��ȣȭ�Ͽ� Ŭ���̾�Ʈ�� �˷��ֵ��� �ؾ���

        ���ȿ� ���õ� ���� �߿��� ������.


2. ������ ����ϴ� �Ʒ��� �� �Լ��� �� �ڵ�� ��ü�Ѵ�.
  (��, ���ϵǴ� ��Ű�� ����� ����� ��� ������ ���۸� ����� �����ְ� ��ƾ� ��.)

3. �м��� ������ �̿��Ͽ� ������ �����Ͽ�

   g_CpCookie �� ������ ������ ������ ��ģ��.

   Ŭ���̾�Ʈ������ g_CpCookie ���� ��ȣȭ�Ǿ� �ֱ� ������ � ���� ����ִ����� �� �� ����.

   g_AuthCookie, g_DataCookie�� �ݸ��� ���� ���� ��Ű�̱� ������ CP��ü�� ������ ����(���� �ѱ��� ���� ���� �ִ�).
   (��, ī�����̼��� ���� Ư�� ������ 3���� ��Ű ��� �ʿ���) 
*/

static char* _StrTok(const char* str, const char sep)
{
	// strtok()�� ���� ����� �Լ������� separator�� 1���� �ް�
	// ���ڰ� ����ִ� ��쵵 ���ڷ� ������

	static const int TOK_BUFSIZE = 1024*40;

	static char* pnow = NULL;
	static char buf[TOK_BUFSIZE];
	static char token[TOK_BUFSIZE];

	if(str) {
		size_t slen = strlen(str);
		_ASSERT(slen < TOK_BUFSIZE);		// ASSERT
		strcpy(buf, str);
		pnow = buf;
	}

	if(!pnow || *pnow == 0)
		return NULL;

	int tlen = 0;
	char* porg = pnow;
	while(*pnow != 0)
	{
		if(*pnow == sep) {
			++pnow;
			break;
		}

		++tlen;
		++pnow;
	}

	if(tlen > 0) {
		strncpy(token, porg, tlen);
		token[tlen] = 0;
	}
	else {
		token[0] = 0;
	}

	return token;
}

static void TrimStr(const char* szSrcStr, char* outStr)
{
	char szTemp[256] = "";

	// ���� ��������
	int nSrcStrLen = (int)strlen(szSrcStr);
	for (int i = 0; i < nSrcStrLen; i++)
	{
		if (!isspace(szSrcStr[i]))
		{
			strcpy(szTemp, &szSrcStr[i]);
			break;
		}
	}
	// ������ ��������
	int nLen = (int)strlen(szTemp);
	for (int i = nLen-1; i >= 0; i--)
	{
		if (isspace(szTemp[i]))
		{
			szTemp[i] = '\0';
		}
		else
		{
			break;
		}
	}

	strcpy(outStr, szTemp);
}

static BOOL AnalyzeArgument(char *key)
{
	//
	// Ŭ������ ���� ���� ��Ģ(������ ���ڴ� �ĸ�(,)�� ���еǸ� ���� ������ ������ ����)
	// ���� ������ ���� : ServerIP,Port,AuthCookie,DataCookie,CpCookie,SpareParam (���ڴ� ��ȣȭ�Ǿ� �ִ�)
	// - �ݸ��� ���ӿ����� CpCookie�� �������� �ʴ´�
	// - CP ���ӿ����� AuthCookie, DataCookie�� �������� �ʴ´�(��� CP������ ����)
	// - Extra ���ڴ� �����Ǳ⵵ �Ѵ�
	//

	char buf[1024*20] = {0,};	// ���۴� ����� ũ�� ��´�

	// Ŭ�����忡�� ���� �����͸� �о�´�(�ѹ� ������ ������)
	if(!GetNMClipData(buf, sizeof(buf)-1, key, true))
		return FALSE;

	if(strlen(buf)==0)
		return FALSE;

	char arg[6][1024*2] = {0,};

	int argcnt = 0;
	char* token = _StrTok(buf, ',');
	while(token && argcnt < 6)
	{
		strcpy(arg[argcnt], token);
		argcnt++;
		token = _StrTok(NULL, ',');
	}

	// ���ڰ� �ּ��� 5���� �Ǿ�� �Ѵ�
	if(argcnt < 5)
		return FALSE;

	// IP�ּҰ� �ùٸ��� �˻�
	if(strlen(arg[0]) > 16)
		return FALSE;

	// CP ������ �Ʒ��κ��� ���� ����
	char* pServIP     = arg[0];					// ���� IP
	UINT  nServPort   = (UINT)::atoi(arg[1]);	// ���� Port
	char* pAuthCookie = arg[2];					// ���� ��Ű	- ���� ũ�⸦1024*2 ������ ��ƾ� �Ѵ�
	char* pDataCookie = arg[3];					// ������ ��Ű	- ���� ũ�⸦1024*2 ������ ��ƾ� �Ѵ�
	char* pCpCookie   = arg[4];					// CP��Ű		- ���� ũ�⸦1024*2 ������ ��ƾ� �Ѵ�
	char* pSpareParam = arg[5];					// ������ ����(CP���ӵ�� ���)

	ZNetmarbleAuthInfo* pMNInfo = (ZNetmarbleAuthInfo*)ZGetLocale()->GetAuthInfo();
	pMNInfo->SetServerIP(pServIP);
	pMNInfo->SetServerPort(nServPort);
	pMNInfo->SetAuthCookie(pAuthCookie);
	pMNInfo->SetDataCookie(pDataCookie);
	pMNInfo->SetCpCookie(pCpCookie);
	pMNInfo->SetSpareParam(pSpareParam);

	//mlog("NMSpareParam = %s \n", pSpareParam);

	char szField[64] = "";
	char szValue[64] = "";
	MLex lex;
	if(lex.SplitValue(pSpareParam, "=", szField, szValue)) {
		if (stricmp("age", szField) == 0) {
			int nAge = atoi(szValue);
		//	nAge = 10; // 17
		//	if (nAge < 18)
		//		pMNInfo->SetTeenMode(true);
		//	else
			pMNInfo->SetTeenMode(false);
		}
	}

	// ����� �Ķ���� �� ����
	char seps[] = ";=";
	token = strtok( pSpareParam, seps );
	while( token != NULL )
	{
		TrimStr(token, token);

		if(strcmp(token, "ServiceType") == 0)		// ���� Ÿ��
		{
			token = strtok( NULL, seps );
			TrimStr(token, token);
			if(strcmp(token, "test") == 0)
				pMNInfo->SetIsAlpha(true);
			else
				pMNInfo->SetIsAlpha(false);
		}
		// ���� �ٸ� ������ �߰��ɶ� ���⼭ ó��
		
		// Get next token
		token = strtok( NULL, seps );
	}

//	pMNInfo->SetServerIP("192.168.0.30");	// RAON DEBUG

#ifndef _PUBLISH
mlog("ServerIP: '%s' \n", pServIP);
mlog("ServPort: '%d' \n", nServPort);
mlog("AuthCookie: '%s' \n", pAuthCookie);
mlog("DataCookie: '%s' \n", pDataCookie);
mlog("CpCookie: '%s' \n", pCpCookie);
mlog("SpareParam: '%s' \n", pSpareParam);
#endif
	
	return TRUE;
}


bool NetmarbleParseArguments(const char* pszArgs)
{
	if (AnalyzeArgument((char*)pszArgs) == TRUE)
		return true;
	else
		return false;
}

#endif // #ifdef LOCALE_KOREA