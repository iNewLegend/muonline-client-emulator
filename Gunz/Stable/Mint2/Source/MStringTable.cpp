#include "stdafx.h"
#include "MStringTable.h"
#include <string.h>

static char g_StringTable[][256] = {
	"MINT",
	"Ȯ��",
	"���",
	"��",
	"�ƴϿ�",
	"�޽���",
	"Overwrite?",
};

const char* MGetString(int nID)
{
	int nCount = sizeof(g_StringTable)/sizeof(char*);
	if(nID<0 || nID>=nCount) return NULL;

	return g_StringTable[nID];
}

void MSetString(int nID, const char* szString)
{
	int nCount = sizeof(g_StringTable)/sizeof(char*);
	if(nID<0 || nID>=nCount) return;

	strcpy(g_StringTable[nID], szString);
}
