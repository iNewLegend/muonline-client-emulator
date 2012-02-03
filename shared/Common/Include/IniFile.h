#pragma once
#include <string>
#include <vector>
using namespace std;
/*
����˵��
1.��INI�ļ��ж�ȡ����
2.������д��INI��ʽ�ļ�
3.[sectin]�����һ���ַ�Ϊ'[',����']'����
4.';'����ַ���ȫ��Ϊע��
5.������name=value�ķ�ʽ��ʾ��ÿһ��ֻ������һ��������'='���ܺ��пո�
6.û�жԳ�����ע��������Ĵ�����д���ļ�ʱ��û�п��ǽ�ע��д��INI�ļ�
7.����INI�ļ���ʽ������ο���
[url]http://www.microsoft.com/technet/archive/wfw/0_welcom.mspx?mfr=true[/url]
*/
//������

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
	//��INI�ļ��ж�ȡ����
	int ReadIniFile(char* szFilename);

	//������д��INI�ļ�
	int WriteIniFile(char* szFilename);

	//ɾ���ַ������˵Ŀո�
	int Trim(char* &szString);

	//ȥ���ַ����е�ע��
	int RemoveComment(char* szLine);

	vector<IniSection> m_isSection;
};

std::string IniGetStr(const char *pszFileName, const char *pszTitle, const char *pszSubTitle);
int IniGetInt(const char *pszFileName, const char *pszTitle, const char *pszSubTitle,int nDefault=0);