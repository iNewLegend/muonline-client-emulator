#pragma once

#ifdef GGSVR_EXPORTS
#define GGSVR_API __declspec(dllexport)
#else
#define GGSVR_API __declspec(dllimport)
#endif

#include "CSAuth2.h"

struct GG_AUTH
{
	DWORD dwProtocolCount;	// 0
	DWORD nMaxCount;	// 4
	_GG_AUTH_PROTOCOL * lpCurrentAuthProtocol;	// 8
	char szGGAuthName[256];	// C

};

DWORD   InitGameguardAuth(LPSTR szGGAuthName, DWORD nNumberOfUser);
void    CleanupGameguardAuth();
DWORD   AddAuthProtocol(LPSTR lpszAuthDll);
DWORD   LoadAuthProtocol(LPSTR lpszAuthDll, BOOL Flag);
DWORD   UnloadAuthProtocol(_GG_AUTH_PROTOCOL * pAuthProtocol);
CCSAuth2 *   GGAuthInitUser();
void   GGAuthCloseUser(CCSAuth2 * lpGGUser);
void   GGAuthGetQuery(CCSAuth2 * lpGGUser, _GG_AUTH_DATA * lpAuthQuery);
DWORD   GGAuthCheckAnswer(CCSAuth2 * lpGGUser, _GG_AUTH_DATA * lpAuthAnswer);

extern GG_AUTH ggGlobal;