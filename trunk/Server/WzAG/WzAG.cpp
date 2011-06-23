//WzAG.cpp	: Desenvolvida para GameServer 1.00.18 kor 
//AuthData	: Autenticação feita pela Codeus Team
//CrackData	: Sistema de Crack feito por HermeX
//Data		: 30/03/2008
//Versão	: 2.0 (Rev 7)

#include "windows.h"
#include "WzAG.h"
#include <fstream>

FILE * fstream1, * fstream2;
char * Lsrvname, * Lsrvver,* Lpartition,* Laddrarg7,* FileBuffer;
unsigned long FileLen ;
unsigned char szServerKey[19]={0x29,0x23 ,0xBE,0x00 ,0xE1,0x6C, 0xD6,0xAE,0x52,0x90,
0x49,0xF1 ,0xF1,0xBB ,0xE9,0xEB ,0xB3,0xA6,0xDB};


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

///////////////////////////////////////////////
//Sistema de Emulação da Autenticação (GSAUTH) *??Não funciona at?agora??
///////////////////////////////////////////////
WZAG_API int AGConnectSetFS() 
{
	_asm{
		xor eax,eax
	}
	return 0;
}

WZAG_API char * AGGetDataBuffer()  
{
	return FileBuffer;
}

WZAG_API int AGGetDataBufferSize() 
{
	return FileLen;
}

WZAG_API void AGGetKey(HANDLE handle,char* key,int startindex,int len)
{
	memcpy(key,szServerKey,sizeof(szServerKey));
}

WZAG_API int AGReleaseHandle() 
{
	_asm{
		xor eax,eax
	}
	return 0;
}

///////////////////////////////////////////////
//Envia a solicitação da Data para o GameServer
///////////////////////////////////////////////
WZAG_API int AGRequestData(int NoneUse,int GetType)  
{

	char * filename ={0};

	switch (GetType)
	{
		//Item
	case 0x00:
		filename="..\\data\\Lang\\Kor\\item(Kor).txt";
		break;
	case 0x01:
		filename="..\\data\\lang\\kor\\item(kor).txt";
		break;
		//Skill
	case 0x02:
		filename="..\\data\\lang\\kor\\skill(kor).txt";
		break;
	case 0x03:
		filename="..\\data\\lang\\kor\\skill(kor)_local.txt";
		break;
		//Quest
	case 0x04:
		filename="..\\data\\lang\\kor\\quest(kor).txt";
		break;
	case 0x05:
		filename="..\\data\\lang\\kor\\quest(kor).txt";
		break;
		//Checksum
	case 0x06:
		filename="..\\data\\lang\\kor\\CheckSum.dat" ;
		break;
		//Monstros
	case 0x07:
		filename="..\\data\\Monster.txt";
		break;
		//Gates
	case 0x08:	
		filename="..\\data\\gate.txt" ;
		break;
		//Posição dos Monstros
	case 0x09:
		filename="..\\data\\MonsterSetBase.txt";
		break;
		//Reservado GameGuard
	case 0x0A:
		break;
		//Shops
	case 0x0B:
		filename="..\\data\\Shop0.txt";
		break;
	case 0x0C:
		filename="..\\data\\Shop1.txt";
		break;
	case 0x0D:
		filename="..\\data\\Shop2.txt";
		break;
	case 0x0E:
		filename="..\\data\\Shop3.txt";
		break;
	case 0x0F:
		filename="..\\data\\Shop4.txt";
		break;
	case 0x10:
		filename="..\\data\\Shop5.txt";
		break;
	case 0x11:
		filename="..\\data\\Shop6.txt";
		break;
	case 0x12:
		filename="..\\data\\Shop7.txt";
		break;
	case 0x13:
		filename="..\\data\\Shop8.txt";
		break;
	case 0x14:
		filename="..\\data\\Shop9.txt";
		break;
	case 0x15:
		filename="..\\data\\Shop10.txt";
		break;		
	case 0x16:
		filename="..\\data\\Shop11.txt";
		break;
	case 0x17:
		filename="..\\data\\Shop12.txt";
		break;
		//Requerimentos do Move
	case 0x18:
		filename="..\\data\\lang\\kor\\movereq(kor).txt";
		break;
	case 0x19:
		filename="..\\data\\lang\\kor\\movereq(kor).txt";
		break;
		//Option dos Set Anciente
	case 0x1A:
		filename="..\\data\\lang\\kor\\itemsetoption(kor).txt";
		break;
	case 0x1B:
		filename="..\\data\\lang\\kor\\itemsetoption(kor).txt";
		break;
		//Tipos de Set Anciente
	case 0x1C:
		filename="..\\data\\lang\\kor\\itemsettype(kor).txt";
		break;
	case 0x1D:
		filename="..\\data\\lang\\kor\\itemsettype(kor).txt";
		break;
		//Novo para Season3
	case 0x1E:
		filename="..\\data\\Shop13.txt";
		break;
//Verificações
	default:
		filename="Falha!" ;
		::ExitProcess(0);
		break;

	}
	if  ( fstream1=fopen(filename,"rb") )
	{
		fseek(fstream1,0,SEEK_END);
		if (FileLen=ftell(fstream1) )
		{
			fseek(fstream1,0,SEEK_SET ) ;
			FileBuffer =new char[FileLen+1];
			fread(FileBuffer,FileLen,1,fstream1);
		}
		else
		{
			MessageBox( NULL, filename,"Erro!", MB_OK );
			::ExitProcess(0);
		}
		fclose(fstream1);
	}

	else
	{
		MessageBox( NULL, filename,"Erro!", MB_OK );
		::ExitProcess(0);
	} 
	return 0;
}

/////////////////////////////////////////////
//Função Principal da DLL WzAG
/////////////////////////////////////////////
WZAG_API int AGSetInfo(int Language,char * Partition,int un0,char * SrvVer,char* SrvName,
					   int SrvType,char * Addr7)  

{
	Lsrvname=SrvName;
	Lsrvver=SrvVer;
	Lpartition=Partition;
	Laddrarg7=Addr7;
	return 0;			
}

WZAG_API int AGSetSubInfo() 
{
	_asm{
		xor eax,eax
	}
	return 0;
}
