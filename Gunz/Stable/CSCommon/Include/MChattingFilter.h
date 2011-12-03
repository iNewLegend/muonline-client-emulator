/************************************************************************
  MChattingFilter

  desc : �Է��� ��Ʈ���� ��ȿ ���θ� �Ǵ��ϴ� Ŭ����
  date : 2008-02-14
  name : �ӵ�ȯ
*************************************************************************/


#pragma once

#include <list>
#include <string>

using namespace std;



/// (class) MAbuseWord
class MAbuseWord
{
/// ��� ���� ����
private:
	/// ���͸� ����
	unsigned int nFilteringLevel;

	/// ��Ģ�ܾ�
	string strAbuseWord;



/// ��� �Լ� ����
public:
	/// ǥ�� ������
	MAbuseWord()
	{
		nFilteringLevel = 0;
		strAbuseWord = "";
	}

	/// ������
	MAbuseWord( int nLevel, const char* szWord)
	{
		nFilteringLevel = nLevel;
		strAbuseWord = szWord;
	}


	/// ���͸� ������ ����
	unsigned int GetFilteringLevel()	{ return nFilteringLevel;			}

	/// ��Ģ�ܾ ����
	const char* GetAbuseWord()			{ return strAbuseWord.c_str();		}
};




/// (class) MChattingFilter
class MChattingFilter
{
/// ��� ���� ����
private :
	/// ��Ģ�ܾ� ����Ʈ
	list<MAbuseWord*>	m_AbuseWordList;

	/// ���������� ���͸��� ��Ģ�ܾ�
	string				m_strLastFilterdWord;



/// ��� �Լ� ����
public:
	/// ǥ�� ������
	MChattingFilter();

	/// ǥ�� �Ҹ���
	~MChattingFilter();


	/// �ܺ� ���Ϸκ��� ��Ģ�ܾ� ����Ʈ�� �о���δ�
	bool LoadFromFile( MZFileSystem* pfs, const char* szFileName);

	/// �ε��� ��Ģ�ܾ ����� ��ȯ�Ѵ�
	int GetNumAbuseWords();

private:
	void GetLine( char*& prfBuf, char* szLevel, char* szText);
	void SkipBlock( char*& prfBuf);


protected:
	/// ��Ʈ���� �ҹ���ȭ�ϰ� Ư�����ڸ� �����Ѵ�.
	bool PreTranslateStr( const string& strInText, string& strOutText);
	

public:
	/// �Է��� ��Ʈ���� ��ȿ ���θ� ����
	virtual bool IsValidStr( const char* szString, unsigned int nLevel, bool bCheckSpcChar =false);


	/// ���������� ���͸��� ��Ģ�ܾ ���Ѵ�
	const char* GetLastFilteredStr();


	/// �ν��Ͻ��� ���Ѵ�
	static MChattingFilter* GetInstance();
	
	///
	void Clear();
};



/// �ν��Ͻ��� ����
inline MChattingFilter* MGetChattingFilter()
{
	return MChattingFilter::GetInstance();
}


