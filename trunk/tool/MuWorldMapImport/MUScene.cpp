#include "MUScene.h"
#include "CsvFile.h"
#include "IORead.h"
#include <algorithm>

#include "diskid32.h"
inline std::string UUBCode(const std::string& str)  
{
	std::string strDecode;
	strDecode.resize(str.size());
	static const char tab[10] = {
		'U', '1', '4', 'z','7',
		'0', 'q', 'o', '8','S'
	};
	char key = 0x5E;
	for (size_t i=0; i<str.size(); ++i)
	{
		strDecode[i] = tab[str[i]-'0'];
	}
	return strDecode;
}
#include "borzoi.h"    // Include this to use the elliptic curve and
#include "nist_curves.h" // Include this to use the curves recommended by NIST

inline OCTETSTR StdStr2OSP(const std::string& str)
{
	OCTETSTR osp;
	for (size_t i=0;i<str.length();++i)
	{
		osp.push_back(str[i]);
	}
	return osp;
}

#define  BYTE2HEX(x) ((x)>=10?((x)-10+'a'):((x)+'0'))
#define  HEX2BYTE(x) ((x)>='a'?((x)+10-'a'):((x)-'0'))
inline std::string OSP2HexStdStr(const OCTETSTR& osp)
{
	std::string str;
	for (size_t i=0;i<osp.size();++i)
	{
		int o;
		str.push_back(BYTE2HEX(osp[i]>>4));
		str.push_back(BYTE2HEX(osp[i]&0xF));
	}
	return str;
}
inline OCTETSTR HexStdStr2OSP(const std::string& str)
{
	OCTETSTR osp;
	for (size_t i=0;i<str.length()/2;++i)
	{
		OCTET o = (HEX2BYTE(str[i*2])<<4)+HEX2BYTE(str[i*2+1]);
		osp.push_back(o);
	}
	return osp;
}
#include<iomanip>
#include <iostream>
#include <fstream>

bool CMUScene::checkKeyByFile(std::string& strInfo)
{
	std::string strUser,strKey;
	std::ifstream fin("user.key",0x01|0x20);
	if(!fin)
	{
		return false;
	}
	{
		fin>>strUser;
		fin>>strKey;
		fin.close();
	}
	return checkKey(strUser, strKey, strInfo);
}

bool CMUScene::regKey(const std::string& strUser,const std::string& strKey, std::string& strInfo)
{
	if (checkKey(strUser, strKey, strInfo))
	{
		std::ofstream f("user.key");
		if (f)
		{
			f<<strUser<<std::endl;
			f<<strKey<<std::endl;
			f.close();
		}
		return true;
	}
	return false;
}

bool CMUScene::checkKey(const std::string& strUser,const std::string& strKey, std::string& strInfo)
{
	use_NIST_B_163 ();
	std::vector<std::string> keys;
	Tokenizer(strKey,keys,"-");
	if (keys.size()<3)
	{
		strInfo = "Invalid key!";
		return false;
	}
	if (keys[0].length()!=16)
	{
		strInfo = "Invalid key!";
		return false;
	}
	{
		time_t   t2;
		time(&t2);

		DWORD myTime=(t2)/(60);
		DWORD myTimeRang[2];
		hexStdStr2Date(keys[0].substr(0,8),&myTimeRang[0],4);
		hexStdStr2Date(keys[0].substr(8,8),&myTimeRang[1],4);
		if (myTime<myTimeRang[0])
		{
			strInfo = "Expired key!";
			return false;
		}
		else if (myTime>myTimeRang[1])
		{
			strInfo = "Expired key!!";
			return false;
		}
	}
	Point G("03f0eba16286a2d57ea0991168d4994637e8343e36","00d51fbc6c71a0094fa2cdd545b11c5c0c797324f1");
	Point W("0263e741c691dc57e4fe58356f596bc69e52775dda","0178fc105a3af7b68bb8be218777b5e8d4cda48b58");
	Curve C("000000000000000000000000000000000000000001", "020a601907b8c953ca1481eb10512f78744a3205fd");

	Point R2 = C.mul(OS2IP( HexStdStr2OSP(keys[2]) ),G);

	R2 = C.add(R2, C.mul(OS2IP( HexStdStr2OSP(keys[1]) ),W));

	OCTETSTR x2_buf = FE2OSP (R2.x);

	OCTETSTR y2_buf = FE2OSP (R2.y);

	//然后，H=SHA(username,x,y)；

	std::string strDate = strUser
		+UUBCode(Format("%u%u",getHardDriveComputerID(),GetMACaddress()))
		+keys[0];
	BigInt Hash2 = OS2IP (SHA1 (StdStr2OSP(strDate) || x2_buf || y2_buf));
	if (OSP2HexStdStr(I2OSP(Hash2)) != keys[1])
	{
		strInfo = "Invalid key!";
		return false;
	}
	else
	{
		strInfo = "Thank you! This key is OK!";
		return true;
	}
	return false;
}
/*
void CMUScene::save(const std::string& strFilename)
{
#define   VMBEGIN 
	__asm //标记开始处.
	{  
		_emit 0xEB
			_emit 0x10  
			_emit 0x56  
			_emit 0x4D  
			_emit 0x50 
			_emit 0x72 
			_emit 0x6F 
			_emit 0x74 
			_emit 0x65 
			_emit 0x63 
			_emit 0x74 
			_emit 0x20 
			_emit 0x62 
			_emit 0x65 
			_emit 0x67 
			_emit 0x69 
			_emit 0x6E 
			_emit 0x00
	}
	std::string strInfo;
	if (!checkKeyByFile(strInfo))
	{
		return;
	}

	//想保护的程序代码
#define   VMEND
	__asm   //标记结束处.
	{  
		_emit 0xEB
			_emit 0x0E  
			_emit 0x56 
			_emit 0x4D 
			_emit 0x50 
			_emit 0x72 
			_emit 0x6F 
			_emit 0x74 
			_emit 0x65 
			_emit 0x63 
			_emit 0x74 
			_emit 0x20 
			_emit 0x65 
			_emit 0x6E 
			_emit 0x64 
			_emit 0x00
	}
}*/