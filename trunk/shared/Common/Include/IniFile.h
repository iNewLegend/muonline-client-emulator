#pragma once
#include <string>
#include <vector>
using namespace std;
/*
程序说明
1.从INI文件中读取参数
2.将参数写入INI格式文件
3.[sectin]必须第一个字符为'[',并以']'结束
4.';'后的字符串全部为注释
5.参数以name=value的方式表示，每一行只能设置一个参数，'='不能含有空格
6.没有对程序中注释做额外的处理，在写入文件时，没有考虑将注释写入INI文件
7.关于INI文件格式定义请参考：
[url]http://www.microsoft.com/technet/archive/wfw/0_welcom.mspx?mfr=true[/url]
*/
//参数行

struct IniEntry
{
	string m_strName;
	string m_strValue;
};

//Section
struct IniSection 
{
	string m_isName;
	vector<IniEntry> m_ieEntry;
	vector<string> m_icComment;
};

class CIniFile
{
public:
	//从INI文件中读取参数
	int ReadIniFile(char* szFilename);

	//将参数写入INI文件
	int WriteIniFile(char* szFilename);

	//删除字符串两端的空格
	int Trim(char* &szString);

	//去除字符串中的注释
	int RemoveComment(char* szLine);

	vector<IniSection> m_isSection;
};

std::string IniGetStr(const char *pszFileName, const char *pszTitle, const char *pszSubTitle);
int IniGetInt(const char *pszFileName, const char *pszTitle, const char *pszSubTitle,int nDefault=0);