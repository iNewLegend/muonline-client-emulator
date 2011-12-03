/************************************************************************
MChattingFilter

desc : �Է��� ��Ʈ���� ��ȿ ���θ� �Ǵ��ϴ� Ŭ����
date : 2008-02-14
name : �ӵ�ȯ
*************************************************************************/

#include "stdafx.h"
#include "MChattingFilter.h"
#include "MZFileSystem.h"



/************************************************************************
  MChattingFilter

  desc : ������
*************************************************************************/
MChattingFilter::MChattingFilter()
{
	m_strLastFilterdWord = "";
}


/************************************************************************
  ~MChattingFilter

  desc : �Ҹ���
*************************************************************************/
MChattingFilter::~MChattingFilter()
{
	Clear();
}

void MChattingFilter::Clear()
{
	// ��Ģ�ܾ� ����Ʈ ����
	while ( m_AbuseWordList.empty() == false)
	{
		delete *m_AbuseWordList.begin();
		m_AbuseWordList.pop_front();
	}
}


/************************************************************************
  LoadFromFile

  desc : �ܺ� ���Ϸκ��� ��Ģ�ܾ� ����Ʈ�� �о���δ�
  arg  : pfs = �ý��� ���� ����Ʈ
         szFileName = ���� �̸�
  ret  : true(success) or false(fail)
*************************************************************************/
bool MChattingFilter::LoadFromFile( MZFileSystem* pfs, const char* szFileName)
{
	if ( szFileName == 0)
		return false;


	// ���� ����
	MZFile mzf;
	if ( !mzf.Open( szFileName, pfs)) 
		return false;


	// ���� ���� �о���̱�
	char *buffer;
	char* tembuf;
	buffer = new char[ mzf.GetLength() + 1];
	mzf.Read( buffer, mzf.GetLength());
	buffer[ mzf.GetLength()] = 0;
	tembuf = buffer;


	// ��Ģ�ܾ� ����Ʈ ����
	while ( m_AbuseWordList.empty() == false)
	{
		delete *m_AbuseWordList.begin();
		m_AbuseWordList.pop_front();
	}

#ifdef _DEBUG
	FILE* fp = fopen( "abuse_db.txt", "w" );
#endif


	// ��Ģ�ܾ� �˻�
	while ( 1)
	{
		char szLevel[ 256];
		char szText[ 256];

		GetLine( tembuf, szLevel, szText);


		if ( (int)strlen( szLevel) == 0)
			continue;

		if ( stricmp( szLevel, "END") == 0)
			break;


		MAbuseWord* pAbuseWord = new MAbuseWord( atoi( szLevel), szText);
		m_AbuseWordList.push_back( pAbuseWord);


#ifdef _DEBUG
		fprintf( fp, "insert into abuselist(workd) values('%%%s%%')\n"
			, szText );
#endif


		SkipBlock( tembuf);
	}

#ifdef _DEBUG
	fclose( fp );
#endif


	mzf.Close();
	tembuf = 0;
	delete [] buffer;


	return true;
}


void MChattingFilter::GetLine( char*& prfBuf, char* szLevel, char* szText)
{
	bool bType = true;
	int  nTypeCount = 0;
	int  nTextCount = 0;

	*szLevel = 0;
	*szText = 0;

	while ( 1)
	{
		char ch = *prfBuf++;

		if ( (ch == 0) || (ch == '\n') || (ch == '\r'))
			break;
			

		if ( ch == ',')
		{
			bType = false;

			continue;
		}


		if ( bType)
		{
			*(szLevel + nTypeCount++) = ch;
			*(szLevel + nTypeCount) = 0;
		}
		else
		{
			*(szText + nTextCount++) = ch;
			*(szText + nTextCount) = 0;
		}
	}
}


void MChattingFilter::SkipBlock( char*& prfBuf)
{
	for ( ; *prfBuf != '\n'; ++prfBuf )
		NULL;
		
	++prfBuf;
}


/************************************************************************
  PreTranslateStr

  desc : ��Ʈ���� �ҹ���ȭ�ϰ� Ư�����ڸ� �����Ѵ�.
  arg  : strInText = ó���� ��Ʈ��
         strOutText = ó���� ��Ʈ��
  ret  : true = ������ �ʴ� Ư�����ڸ� �����ϰ� ����
         false = ����� Ư������ ����
*************************************************************************/
bool MChattingFilter::PreTranslateStr( const string& strInText, string& strOutText)
{	
	char *pStrLowercase;
//	_strlwr_s( pStrLowercase = _strdup( strInText.c_str()), 512);			// ���ȿ�
//	pStrLowercase = _strlwr( _strdup( strInText.c_str()));					// �񺸾ȿ�
	pStrLowercase = _strdup( strInText.c_str());
	strOutText = pStrLowercase;

	// ĳ���� �˻�
	int nPos = 0;					// ��Ʈ�� �˻� ��ġ
	bool bHaveSpcChar = false;		// ����� Ư������ ���� ����
	char ch;
	unsigned char u_ch;


	bool bAllowDoubleByteChar = true;
	// �̱����� 2����Ʈ ���ڸ� �̿��� ĳ���� ��ĭ�� �ִ� ����� �����ؼ� �ƿ� �̱����� 2����Ʈ ���ڸ� ������� ����
	// �̷ν� �̱������� ĳ����� ä�ÿ� 2����Ʈ ���ڸ� ���� ����� �� ���� ��.
#ifdef LOCALE_NHNUSA
	bAllowDoubleByteChar = false;
#endif

	while ( 1)
	{
		// ����� ����� ����
		if ( nPos >= (int)strOutText.size())
			break;

		// ���� ��ġ�� ĳ���͸� ����
		ch = strOutText.at( nPos);
		u_ch = ch;


		// ���� ĳ���Ͱ� 2����Ʈ ������ ù ����Ʈ�̸� ���� ����Ʈ������ �˻縦 �н�
		if ( bAllowDoubleByteChar && IsDBCSLeadByte( ch))
		{
#ifdef LOCALE_JAPAN   // �Ϻ� ���� ������ ��� Ư�����ڵ� ���� 2����Ʈ��. 
			if ( ch == -127 && strOutText.at( nPos +1) == 64) // �ϴ� �����̽��� ���� 
			{
				bHaveSpcChar = true;
				m_strLastFilterdWord = strOutText.substr(nPos, 2);;
			}
#endif
			nPos += 2;
		}

		// �Ϲ� ASCII �������� Ȯ��
#ifdef LOCALE_JAPAN   
		else if ( ( (ch >= 'a') && (ch <= 'z') ) ||	
				  ( (ch >= 'A') && (ch <= 'Z') ) ||
				  ( (ch >= '0') && (ch <= '9') ) ||
				  ( (u_ch >= 0xA1 ) && (u_ch <= 0xDF ) ) )  // �Ϻ� �ݰ� �������� Ȯ��
#else 
		else if ( ( (ch >= 'a') && (ch <= 'z') ) ||	
			( (ch >= 'A') && (ch <= 'Z') ) ||
			( (ch >= '0') && (ch <= '9') )  )  
#endif
		{
			if( ( (ch >= 'A') && (ch <= 'Z') ) )  // �ҹ��� ��ȯ
			{
				strOutText[nPos] += 0x20;
			}
			nPos += 1;
		}
		else
		{
			// �����ϱ� ���� �ش� ĳ���Ͱ� ����� Ư���������� �˻�
			if ( (ch != '_') && (ch != '[') && (ch != ']') )
			{
				bHaveSpcChar = true;

				m_strLastFilterdWord = ch;
			}

			// ĳ���� ����
			strOutText.erase( nPos, 1);
		}
	}

	free( pStrLowercase);

	return bHaveSpcChar;
}




/************************************************************************
  IsValidStr

  desc : �Է��� ��Ʈ���� ��ȿ ���θ� ����
  arg  : szString = ��ȿ ���θ� �Ǵ��� ��Ʈ��
         nFilteringLevel = ���͸� ����
		 bCheckSpcChar = ����� Ư������ �˻� ����
  ret  : true(valid) or false(invalid)
*************************************************************************/
bool MChattingFilter::IsValidStr( const char* szString, unsigned int nFilteringLevel, bool bCheckSpcChar)
{
	int i, j;
	int len1, len2;


	if ( szString == 0)
		return false;

	// Ư�� ���ڸ� �����Ѵ�.
	string str;
	bool bHaveSpcChar = PreTranslateStr( szString, str);

	// ����� Ư�����ڰ� �ִ��� �˻�
	if ( (bCheckSpcChar == true) && (bHaveSpcChar == true) )
		return false;

	// ��Ģ� �ִ��� �����Ѵ�.
	for ( list<MAbuseWord*>::iterator itr = m_AbuseWordList.begin();  itr != m_AbuseWordList.end();  itr++)
	{
		MAbuseWord* pAbuseWord = (*itr);


		// ���� �˻�
		if ( nFilteringLevel < pAbuseWord->GetFilteringLevel())
			continue;


		// ��Ģ�ܾ� �˻�
		len2 = (int)strlen( pAbuseWord->GetAbuseWord());
		len1 = (int)str.length() - len2 + 1;

		for ( i = 0;  i < len1;  i++)
		{
			for ( j = 0;  j < len2;  j++)
			{
				if ( str.at( i + j) != pAbuseWord->GetAbuseWord()[ j])
					break;
			}


			if ( j == len2)
			{
				m_strLastFilterdWord = pAbuseWord->GetAbuseWord();

				return false;
			}


			if ( IsDBCSLeadByte( str.at( i)) == TRUE)
				i++;
		}
	}

	return true;
}


/************************************************************************
  GetLastFilteredStr

  desc : ���������� ���͸��� ��Ģ�ܾ ���Ѵ�
  arg  : none
  ret  : filtered string
*************************************************************************/
const char* MChattingFilter::GetLastFilteredStr()
{
	return m_strLastFilterdWord.c_str();
}


/************************************************************************
  GetInstance

  desc : �ν��Ͻ��� ���Ѵ�
  arg  : none
  ret  : none
*************************************************************************/
MChattingFilter* MChattingFilter::GetInstance()
{
	static MChattingFilter ChattingFilter;
	return &ChattingFilter;
}


/************************************************************************
	GetNumAbuseWords

	desc : �ε��� ��Ģ�ܾ ����� ��ȯ�Ѵ�
	arg  : none
	ret  : none
*************************************************************************/
int MChattingFilter::GetNumAbuseWords()
{
	return (int)m_AbuseWordList.size();
}