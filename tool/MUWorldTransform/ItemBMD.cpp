#include "ItemBMD.h"
#include "DecryptFuncs.h"
#include "CSVFile.h"
#include<iostream>
#include<iomanip>
#include<fstream>

struct itemdata97
{
	char cName[30];
	unsigned char ucInt[34];
};

struct itemdatastruct97 //96y,97,98c
{
	char cName[30];	//1物品名称
	unsigned char ucHand;		//2 0单手，1双手
	unsigned char ucDroplv;	//3 所掉物品的怪物等级Level
	unsigned char ucX;		//4 X,Y 所占格子
	unsigned char ucY;        //5
	unsigned char ucDamMin;	//6 最小攻击力
	unsigned char ucDamMax;	//7 最大攻击力
	unsigned char ucDefxx;	//8 防御率
	unsigned char ucDef;		//9 防御力Def
	unsigned char ucMagDef;	//10 00 MagDef 魔法防御?
	unsigned char ucSpeed;	//11 攻击速度/护手等属性
	unsigned char ucWalkSeed;	//12 WalkSpeed 鞋子属性
	unsigned char ucDur;		//13 持久度
	unsigned char ucUnknown1;	//14 00
	unsigned short usiReqStr; //15 力量,此力量非所佩带需要力量,是有个力量参数*10 得来 /除了12项物品,该项是直接按值计算
	unsigned short usiReqDex; //16 敏捷,此力量非所佩带需要敏捷,是有个敏捷参数*10 得来 /除了12项物品
	//unsigned char ucUnknown2;  //17
	//unsigned char ucUnknown3;      //18
	unsigned char ucReqLev;	//19 最小佩带物品等级
	unsigned char ucValue;		//20 14项物品计算价格参数
	unsigned char ucUnknown4;	//21 ?书 12项非零
	unsigned char ucUnknown5;        //22
	unsigned char ucUnknown6;	//23 ? 书 极光以上非零
	unsigned char ucUnknown7;	//24 00
	unsigned char ucType1; //25 ?盾04 ..风头04//装备类都为04 普通武器刀剑类1 天雷3,玛雅武器0,弓弩2 法师杖03
	unsigned char ucDs;		//26 法师
	unsigned char ucDk;		//27 战士
	unsigned char ucElf;		//28 精灵
	unsigned char ucMg;		//29 魔剑士
	unsigned char ucBing;		//30 防冰属性
	unsigned char ucDu;		//31 防毒属性
	unsigned char ucLei;		//32 防雷属性
	unsigned char ucHuo;		//33 防火属性
}; //64byte

void encryptItemBMD(const std::string& strSrcFilename, const std::string& strDestFilename)
{
	CCsvFile csvFile;
	if (csvFile.Open(strSrcFilename))
	{
		unsigned char buffer[512*64];
		memset(buffer,0,512*64);
		for (size_t i=0;i<512;++i)
		{
			itemdata97* pItem=(itemdata97*)(buffer+i*64);
			itoa(i,pItem->cName,10);
		}
		while (csvFile.SeekNextLine())
		{
			int id = csvFile.GetInt(0)*32+csvFile.GetInt(1);
			if (id>=512)
			{
				continue;
			}
			itemdata97* pItem=(itemdata97*)(buffer+id*64);
			std::string strName = csvFile.GetStr(2);
			strcpy(pItem->cName,strName.c_str());
			for (size_t n=0;n<34;++n)
			{
				pItem->ucInt[n]=csvFile.GetInt(n+3);
			}
		}
		csvFile.Close();
		//
		for (size_t i=0;i<512;++i)
		{
			decryptMuBufferXOR3(buffer+i*64,64);
		}
		DWORD dwCheck;
		//
		FILE* fp = fopen(strDestFilename.c_str(), "wb");
		if (fp)
		{
			fwrite(buffer,512*64,1,fp);
			fwrite(&dwCheck,4,1,fp);
			fclose(fp);
		}
	}
}

void decryptItemBMD(const std::string& strSrcFilename, const std::string& strDestFilename)
{
	FILE* fp = fopen(strSrcFilename.c_str(), "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		size_t size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		unsigned char* buffer = new unsigned char[size];
		fread(buffer,size,1,fp);
		fclose(fp);
		fp = NULL;
		for (size_t i=0;i<size/64;++i)
		{
			decryptMuBufferXOR3(buffer+i*64,64);
		}

		std::ofstream file;
		file.open(strDestFilename.c_str(), std::ios::out);
		if ( file.is_open() )
		{
			file << "分组,索引,物品名称,双手武器,掉怪等级,占格宽度,占格高度,最小攻击力,最大攻击力,防御率,防御力,魔法防御,攻击速度,鞋子属性,耐久度,魔法攻击率,力量需求,敏捷需求,等级需求,价格参数,书,22,书极光以上非零,00,类型,9,9,9,9,法师,战士,精灵,魔剑士,防冰,防毒,防雷,防火"<< std::endl;
			for (size_t i=0;i<size/64;++i)
			{
				itemdata97* pItem = (itemdata97*)(buffer+i*64);
				if (strlen(pItem->cName)!=0)
				{
					file<<(i/32)<<","<<(i%32)<<","<<pItem->cName;
					for (size_t n=0;n<34;++n)
					{
						file<<","<<(int)pItem->ucInt[n];
					}
					file<<std::endl;
				}
			}
		}
		file.close();

		delete buffer;
	}
}
