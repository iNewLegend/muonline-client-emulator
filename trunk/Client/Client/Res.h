//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once/* Res.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <string>
#include <map>
#include "TSingleton.h"
#include "CSVFile.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CStrRes  
{
private:
	std::map<std::string, std::string> m_mapStr;
public:
	CStrRes();
	~CStrRes();
	// ----
	void				load(const char* szFilename);
	const std::string&	getStr(const char* szIndex)const;
};

class CCSVMonster:public CCsvFile, public TSingleton<CCSVMonster>
{
public:
	CCSVMonster():CCsvFile("Data\\CSV\\MonsterV.csv"){}
};

class CCSVItem:public CCsvFile, public TSingleton<CCSVItem>
{
public:
	CCSVItem():CCsvFile("Data\\CSV\\Item.csv"){}
};

class CCSVEquip:public CCsvFile, public TSingleton<CCSVEquip>
{
public:
	CCSVEquip():CCsvFile("Data\\CSV\\Equip.csv"){}
};

class CCSVAttachWeapon:public CCsvFile, public TSingleton<CCSVAttachWeapon>
{
public:
	CCSVAttachWeapon():CCsvFile("Data\\CSV\\AttachWeapon.csv"){}
};

class CCSVAnim:public CCsvFile, public TSingleton<CCSVAnim>
{
public:
	CCSVAnim():CCsvFile("Data\\CSV\\Anim.csv"){}
};

#define CSVMONSTER	CCSVMonster::getInstance()
#define CSVITEM		CCSVItem::getInstance()
#define CSVEQUIP	CCSVEquip::getInstance()
#define CSVATTACH	CCSVAttachWeapon::getInstance()
#define CSVANIM		CCSVAnim::getInstance()
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------