// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
// GS-N 0.99.60T Status : Completed

#include "stdafx.h"
#include "gameserverauth.h"
#include "GameMain.h"
#include "user.h"

CGameServerAuth gGameServerAuth;

//={0x29, 0x23, 0xBE, 0xFF, 0xE1, 0x6C, 0xD6, 0xAE, 0x52, 0x90, 0x49, 0x7E, 0xE9, 0xBB, 0xE9, 0xEB, 0xB3, 0xA6, 0xDB, 0xFF, 0xFF, 0xFF};
// szAuthKey[0] = 41
// szAuthKey[1] = 35
// szAuthKey[2] = 
// szAuthKey[3] = 
// szAuthKey[4] = -31
// szAuthKey[5] = 
// szAuthKey[6] = 
// szAuthKey[7] = 
// szAuthKey[8] = 
// szAuthKey[9] = -112
// szAuthKey[10] = 73
// szAuthKey[11] = -15
// szAuthKey[12] = -15
// szAuthKey[13] = -69
// szAuthKey[14] = 
// szAuthKey[15] = -15
// szAuthKey[16] = 
// szAuthKey[17] = -90
// szAuthKey[18] = -37
// szAuthKey[19] = 

void GameServerAuthCallBackFunc(int Type, int pData)
{
	int var1;

if (!Type)
{

	var1=pData;

	switch (var1)
	{
	case 1:
		ReadCommonServerInfo();
		break;
	case 2:
		GameMonsterAllCloseAndReLoad();
		break;
	case 3:
		ReadServerInfo();
		break;
	}


}
}

CGameServerAuth::CGameServerAuth()
{
	this->WZAGSerInfo=NULL;
	this->WZAGSetSubInfo=NULL;
	this->m_bHandle=NULL;
	this->m_iTimer=NULL;
	this->m_bDllHandle=NULL;
}




CGameServerAuth::~CGameServerAuth()
{
	this->WzAGReleaseHandle(this->GetHandle());

	if ( this->m_bHandle != NULL)
	{
		FreeLibrary(this->m_bHandle);
	}
}




void CGameServerAuth::Init()
{
	if(this->m_bHandle)
	{
		FreeLibrary(this->m_bHandle);
	}
#ifdef _DEBUG
	this->m_bHandle=LoadLibrary("WzAGd.dll");
#else
	this->m_bHandle=LoadLibrary("WzAG.dll");
#endif
	if (this->m_bHandle==0)
	{
		return;
	}
	else
	{
		this->WZAGSerInfo= (unsigned long (*)(int, unsigned char, short, char *, char *, int, void (*)(int, int)))GetProcAddress(this->m_bHandle,"AGSetInfo");
		this->WZAGSetSubInfo=(void  (*)(unsigned long, int, int, int, int, float, int))GetProcAddress(this->m_bHandle,"AGSetSubInfo");
		this->WZAGGetKey=(void  (*)(unsigned long, char*, int, int))GetProcAddress(this->m_bHandle,"AGGetKey");
		this->WzAGRequestData=(void  (*)(unsigned long, int))GetProcAddress(this->m_bHandle,"AGRequestData");
		this->WzAGGetDataBufferSize=(int  (*)(unsigned long))GetProcAddress(this->m_bHandle,"AGGetDataBufferSize");
		this->WzAGGetDataBuffer=(char*  (*)(unsigned long))GetProcAddress(this->m_bHandle,"AGGetDataBuffer");
		this->WZAGConnectSetFS=(int  (*)(unsigned long))GetProcAddress(this->m_bHandle,"AGConnectSetFS");
		this->WzAGReleaseHandle=(void  (*)(unsigned long))GetProcAddress(this->m_bHandle,"AGReleaseHandle");
		this->AGCheckMac=(bool  (*)(char*))GetProcAddress(this->m_bHandle,"AGCheckMac");

	}
	
}

void CGameServerAuth::SendInfo()
{
	this->m_iTimer++;
	if (this->m_iTimer<=120)
	{
		this->m_iTimer=0;
		if (this->WZAGSetSubInfo!=0)
		{
			this->WZAGSetSubInfo(this->GetHandle(), gServerMaxUser, gObjTotalUser, gMonsterHp, gItemDropPer, gAddExperience, GetEventFlag());
		}
	}
}


void CGameServerAuth::GetKey(char* key, int startindex,int len)
{
	this->WZAGGetKey(this->GetHandle(), key,startindex,len);
}

void CGameServerAuth::RequestData(int type)
{
	this->WzAGRequestData(this->GetHandle(),type);
}

int CGameServerAuth::GetDataBufferSize()
{
	return this->WzAGGetDataBufferSize(this->GetHandle());
}

char* CGameServerAuth::GetDataBuffer()
{
	return this->WzAGGetDataBuffer(this->GetHandle());
}

DWORD CGameServerAuth::SetInfo(int iCountryCode, unsigned char cPartition, short iServerCode, char* szServerVersion,  char* szServerName, int ServerType,  void  (*callbackfunc)(int, int))
{
	this->m_bDllHandle=this->WZAGSerInfo(iCountryCode, cPartition, iServerCode, szServerVersion, szServerName, ServerType, callbackfunc);
	return m_bDllHandle;

}

bool CGameServerAuth::MacCheck(char *szBuffer)
{
	return this->AGCheckMac(szBuffer);
}