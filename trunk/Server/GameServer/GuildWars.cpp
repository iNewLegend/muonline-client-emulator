// GuildWars.cpp: implementation of the GuildWars class.
// Developer : leo123
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuildWars.h"
#include "logproc.h"
#include "xml.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuildWars::GuildWars()
{

}

GuildWars::~GuildWars()
{

}

//////////////////////////////////////////////////////////////////////
// Class Functions
//////////////////////////////////////////////////////////////////////

void GuildWars::Init()
{

}

bool GuildWars::Load(char* szFileName)
{
	/*
	xmlDocPtr hFile = xmlParseFile(szFileName);
	if(hFile)
	{
		xmlNodePtr nodeRoot = xmlDocGetRootElement(hFile);
		if( xmlStrcmp(nodeRoot->name, (const xmlChar*)"GuildWars") == 0)
		{
			xmlNodePtr subChild = NULL;
			subChild = nodeRoot->children;
			while(SubChild)
			{
				if( xmlStrcmp(subChild->name, (const xmlChar*)"Time") == 0)
				{
				}
			}
		}
		
	}
	else
	{
		MsgBox("[%s.xml] :: File not found",szFileName);
		return false;
	}*/
	return true;
}
