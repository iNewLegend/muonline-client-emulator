// INI�ļ�
// �ɽ��ޣ�2001.11.19

// ֻ�������ļ��ӿ�
// ���ڶ�ȡ��ʽ�����ַ�����������
// ����ʵ���ο������е�INI�ļ��ͱ��ĺ��˵��

#pragma once
#include <string>
// ���е�KEY�������ݴ������ܴ���512��
#define INIWORDSIZE		512			// INI ��ʹ�õĴ��Ŀռ䳤��
#define	INIDATASIZE		8192		//	INI �е����ݴ�����

class	CIniFile
{
public:
	// pFilename��Ҫ��ȡ���ļ���
	// pSection: Ҫ��ȡ������������INI�ļ��е�[SECTION]���֣�pSectionΪ "SECTION" ��
	CIniFile(const char * pFilename, const char * pSection);
	~CIniFile();
public:
	// �������Ҫ��ȡ������
	// pSect���������Ĵ�
	void	SetSection(const char * pSect);

	// ��ָ�������ж�ȡһ������������
	// pKey:	��=����ǰ��KEY����
	// bufString:	Ԥ�ȷ���Ĵ��ռ䡣���빻�󣬷���ֻ�ܶ��벿�ִ���
	// nBufSize��	bufString��ָ��Ŀռ�ĳߴ硣
	// return:		��KEY��ʱ������true�����򷵻�false
	bool	GetString(char * bufString, const char * pKey, int nBufSize);
	bool	getString(std::string& str, const std::string& strKey);

	// ��ָ�������ж�ȡһ������
	// pKey:	��=����ǰ��KEY��
	// return :	ȡ����������(��KEYʱ����0)
	int		GetInt(const char * pKey);

// ����Ϊ�ڲ����� ////////////////////////////////////////////////////////////
protected:
	char m_bufFilename[INIWORDSIZE];
	char m_bufSection[INIWORDSIZE];
};

std::string IniGetStr(const char *pszFileName, const char *pszTitle, const char *pszSubTitle);
int IniGetInt(const char *pszFileName, const char *pszTitle, const char *pszSubTitle,int nDefault=0);

/*	INI�ļ���ʽ�淶

[SECTION]

KEY1=STRING

KEY2=INT

; COMMENT


	˵����

	SECTION�У�	����Ϊ����ʶ�����������ִ�Сд��([)���붥�У�([])�м䲻���пհס�(])������Դ�ע�͡�

	KEY�У�		KEY���붥�У�����Ϊ����ʶ�����������ִ�Сд��
				(=)ǰ������пհס�
				STRING������(")��ס������ᱻ�����ַ�����һ���֡�β�ջᱻɾ����
				INTΪʮ��������
				STRING��INT������ע�͡�

	COMMENT��	(;)���붥�У��������������塣
	���У�		�����塣



//*/