// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
// Gate.cpp
// GS-N 0.99.60T 0x004FF2E0
//	GS-N	1.00.18	JPN	0x0052F200	-	Completed

#include "stdafx.h"
#include "gate.h"
#include "logproc.h"
#include "GameMain.h"
#include "GameServer.h"
#include "..\common\WzMemScript.h"
#include "..\include\readscript.h"
#include "GameServerAuth.h"

CGate gGateC;


CGate::CGate ()
{
	return;
}


CGate::~CGate()
{
	return;
}


void CGate::Init()
{
	int n;

	for (n=0;n<MAX_GATES;n++)
	{
		this->m_This[n] = 0xFF;	// -1 NULL
		this->m_Level[n]=0;
	}
}


void CGate::Load(char* filename)
{
	int Token;
	int number;
	
	this->Init();

	SMDFile=fopen(filename, "r");
	
	if (SMDFile==0)
	{
		LogAdd("[%s] gate information load error", filename);
		return;
	}

	while ( true )
	{
		Token = GetToken();

		if ( Token == 2 )
		{
			break;
		}
		else
		{
			if ( Token == 1 )
			{

				number=TokenNumber;
				this->m_This[number]=number;

				Token=GetToken();
				this->m_Flag[number]=TokenNumber;

				Token=GetToken();
				this->m_MapNumber[number]=TokenNumber;

				Token=GetToken();
				this->m_Sx[number]=TokenNumber;

				Token=GetToken();
				this->m_Sy[number]=TokenNumber;

				Token=GetToken();
				this->m_Ex[number]=TokenNumber;

				Token=GetToken();
				this->m_Ey[number]=TokenNumber;

				Token=GetToken();
				this->m_TargetGate[number]=TokenNumber;

				Token=GetToken();
				this->m_Dir[number]=TokenNumber;

				Token=GetToken();
				this->m_Level[number]=TokenNumber;
			}
		}
	}	// Iterator

	fclose(SMDFile);
	LogAdd("[%s] Gate information data load complete.", filename);
}


void CGate::Load(char* Buffer, int iSize)	// Load with Parameters
{
	CWzMemScript WzMemScript;
	int Token;
	int number;
	
	WzMemScript.SetBuffer(Buffer, iSize);
	this->Init();

	while ( true )
	{
		Token=WzMemScript.GetToken();
		if ( Token == 2 )
		{
			break;
		}

		if ( Token == 1 )
		{

			number=WzMemScript.GetNumber();
			this->m_This[number]=number;

			Token=WzMemScript.GetToken();
			this->m_Flag[number]=WzMemScript.GetNumber();

			Token=WzMemScript.GetToken();
			this->m_MapNumber[number]=WzMemScript.GetNumber();

			Token=WzMemScript.GetToken();
			this->m_Sx[number]=WzMemScript.GetNumber();

			Token=WzMemScript.GetToken();
			this->m_Sy[number]=WzMemScript.GetNumber();

			Token=WzMemScript.GetToken();
			this->m_Ex[number]=WzMemScript.GetNumber();

			Token=WzMemScript.GetToken();
			this->m_Ey[number]=WzMemScript.GetNumber();

			Token=WzMemScript.GetToken();
			this->m_TargetGate[number]=WzMemScript.GetNumber();

			Token=WzMemScript.GetToken();
			this->m_Dir[number]=WzMemScript.GetNumber();

			Token=WzMemScript.GetToken();
			this->m_Level[number]=WzMemScript.GetNumber();
		}
	}

	LogAdd("Gate information data load complete.");
}


BOOL CGate::IsGate(int GateNumber)
{
	if ( (GateNumber < 0) || (GateNumber > MAX_GATES-1) )
	{
		LogAdd(lMsg.Get(MSGGET(1, 195)), GateNumber, __FILE__, __LINE__);
		return 0;
	}

	if ( this->m_This[GateNumber] == (BYTE)-1 )
	{
		LogAdd( lMsg.Get(MSGGET(1, 196)), __FILE__, __LINE__);
		return 0;
	}

	return 1;
}



int CGate::GetGate(int mgt, short& x, short& y, BYTE& MapNumber, BYTE& dir, short& Level)
{
	int gt;
	int tx;
	int ty;
	int loopcount;
	BYTE attr;
	
	if (this->IsGate(mgt) == 0)
	{
		return -1;
	}

	gt=this->m_TargetGate[mgt];
	if ( gt == 0 )
	{
		gt=mgt;	// This is for M Key
	}
	
	loopcount=10;
	while ( loopcount-- != 0 )
	{
		// For X coords
		if ( (this->m_Ex[gt] - this->m_Sx[gt]) > 0 )
		{
			tx = this->m_Sx[gt] + ( rand() % ( this->m_Ex[gt] - this->m_Sx[gt] ) );	// Choose any X position between XBegin and XEnd
		}
		else
		{
			tx = this->m_Sx[gt];	// If there is amistake while putting values in gate.txt
		}
		// For Y coords
		if ( (this->m_Ey[gt] - this->m_Sy[gt]) > 0 )
		{
			ty = this->m_Sy[gt] + ( rand() % ( this->m_Ey[gt] - this->m_Sy[gt] ) );	// Choose any Y position between YBegin and YEnd
		}
		else
		{
			ty = this->m_Sy[gt];	// If there is amistake while putting values in gate.txt
		}

		attr = MapC[MapNumber].GetAttr( tx, ty);

		if ( (attr&4) != 4 && (attr&2) != 2 && (attr&8) != 8 )
		{
			break;
		}
	}

	x = tx;
	y = ty;
	MapNumber = this->m_MapNumber[gt];
	dir = this->m_Dir[gt];
	Level = this->m_Level[gt];
	return gt;

}
			

int CGate::GetLevel(int GateNumber)
{
	if (GateNumber < 0 || GateNumber > MAX_GATES-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 195)), GateNumber, __FILE__, __LINE__);
		return -1;
	}

	return this->m_Level[GateNumber];
}



BOOL CGate::IsInGate(int aIndex, int GateNumber)
{
	LPOBJ lpObj = &gObj[aIndex];

	if ( this->IsGate(GateNumber) == FALSE )
	{
		return false;
	}

	int level = this->m_Level[GateNumber];

	if ( lpObj->Class == 4 || lpObj->Class == 3 )
	{
		if ( level > 0 )
		{
			level = level / 3 * 2;
		}
	}

#if (FOREIGN_GAMESERVER==1)

	if (szAuthKey[15] != AUTHKEY15 )
	{
		DestroyGIocp();
	}

#endif

	if ( lpObj->Level < level )
	{
		return FALSE;
	}

	if ( lpObj->MapNumber != this->m_MapNumber[GateNumber] )
	{
		return FALSE;
	}

	if ( lpObj->X < this->m_Sx[GateNumber]-5 || lpObj->X > this->m_Ex[GateNumber]+5 || lpObj->Y < this->m_Sy[GateNumber]-5 || lpObj->Y > this->m_Ey[GateNumber]+5 )
	{
		return FALSE;
	}

	return true;
}

BOOL CGate::CheckGateLevel(int aIndex, int GateNumber)
{
	LPOBJ lpObj = &gObj[aIndex];

	if ( this->IsGate(GateNumber) == FALSE )
	{
		return false;
	}

	int level = this->m_Level[GateNumber];

	if ( lpObj->Class == 4 || lpObj->Class == 3 )
	{
		if ( level > 0 )
		{
			level = (level/3)*2;
		}
	}

	if ( lpObj->Level < level )
	{
		return false;
	}

	return true;
}
