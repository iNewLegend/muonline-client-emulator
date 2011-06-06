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
	char cName[30];	//1��Ʒ����
	unsigned char ucHand;		//2 0���֣�1˫��
	unsigned char ucDroplv;	//3 ������Ʒ�Ĺ���ȼ�Level
	unsigned char ucX;		//4 X,Y ��ռ����
	unsigned char ucY;        //5
	unsigned char ucDamMin;	//6 ��С������
	unsigned char ucDamMax;	//7 ��󹥻���
	unsigned char ucDefxx;	//8 ������
	unsigned char ucDef;		//9 ������Def
	unsigned char ucMagDef;	//10 00 MagDef ħ������?
	unsigned char ucSpeed;	//11 �����ٶ�/���ֵ�����
	unsigned char ucWalkSeed;	//12 WalkSpeed Ь������
	unsigned char ucDur;		//13 �־ö�
	unsigned char ucUnknown1;	//14 00
	unsigned short usiReqStr; //15 ����,���������������Ҫ����,���и���������*10 ���� /����12����Ʒ,������ֱ�Ӱ�ֵ����
	unsigned short usiReqDex; //16 ����,���������������Ҫ����,���и����ݲ���*10 ���� /����12����Ʒ
	//unsigned char ucUnknown2;  //17
	//unsigned char ucUnknown3;      //18
	unsigned char ucReqLev;	//19 ��С�����Ʒ�ȼ�
	unsigned char ucValue;		//20 14����Ʒ����۸����
	unsigned char ucUnknown4;	//21 ?�� 12�����
	unsigned char ucUnknown5;        //22
	unsigned char ucUnknown6;	//23 ? �� �������Ϸ���
	unsigned char ucUnknown7;	//24 00
	unsigned char ucType1; //25 ?��04 ..��ͷ04//װ���඼Ϊ04 ��ͨ����������1 ����3,��������0,����2 ��ʦ��03
	unsigned char ucDs;		//26 ��ʦ
	unsigned char ucDk;		//27 սʿ
	unsigned char ucElf;		//28 ����
	unsigned char ucMg;		//29 ħ��ʿ
	unsigned char ucBing;		//30 ��������
	unsigned char ucDu;		//31 ��������
	unsigned char ucLei;		//32 ��������
	unsigned char ucHuo;		//33 ��������
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
			file << "����,����,��Ʒ����,˫������,���ֵȼ�,ռ����,ռ��߶�,��С������,��󹥻���,������,������,ħ������,�����ٶ�,Ь������,�;ö�,ħ��������,��������,��������,�ȼ�����,�۸����,��,22,�鼫�����Ϸ���,00,����,9,9,9,9,��ʦ,սʿ,����,ħ��ʿ,����,����,����,����"<< std::endl;
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
