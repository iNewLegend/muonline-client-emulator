#include "stdafx.h"
#include "MBaseLocale.h"

MBaseLocale::MBaseLocale()
{
	m_bIsComplete = false;
}

MBaseLocale::~MBaseLocale()
{

}

bool MBaseLocale::Init(MCountry nCountry)
{
	m_nCountry = nCountry;
	InitLanguageFromCountry();

	m_bIsComplete = OnInit();

	return m_bIsComplete;
}


// �������� �������� ��� ���� - �̰��� ������ �߰��ɶ����� ���� �־���� �Ѵ�.
void MBaseLocale::InitLanguageFromCountry()
{
	switch (m_nCountry)
	{
	case MC_KOREA:		m_nLanguage = ML_KOREAN;		break;
	case MC_US:			m_nLanguage = ML_ENGLISH;		break;
	case MC_JAPAN:		m_nLanguage = ML_JAPANESE;		break;
	case MC_BRAZIL:		m_nLanguage = ML_BRAZIL;		break;
	case MC_INDIA:		m_nLanguage = ML_INDIA;			break;
	case MC_NHNUSA :	m_nLanguage = ML_ENGLISH;		break;
	default:
		{
			// �ش� ���� ���� - �߰����ּ���.
			_ASSERT(0);
		}
	};
}


const MCountry GetCountryID( const char* pLanguage )
{
	ASSERT( (0 != pLanguage) && (0 < strlen(pLanguage)) );

	if( 0 == pLanguage )			return MC_INVALID;
	if( 0 == strlen(pLanguage) )	return MC_INVALID;

	if( 0 == stricmp("kor", pLanguage) )
		return MC_KOREA;
	else if( 0 == stricmp("international", pLanguage) )
		return MC_US;
	else if( 0 == stricmp("jpn", pLanguage) )
		return MC_JAPAN;
	else if( 0 == stricmp("brz", pLanguage) )
		return MC_BRAZIL;
	else if( 0 == stricmp("ind", pLanguage) )
		return MC_INDIA;
	else if( 0 == stricmp("usa", pLanguage) )
		return MC_NHNUSA;
	else
	{
		ASSERT( 0 );
	}

	return MC_INVALID;
}


const MLanguage GetLanguageID( const char* pLanguage )
{
	ASSERT( (0 != pLanguage) && (0 < strlen(pLanguage)) );

	if( 0 == pLanguage )		 return ML_INVALID;
	if( 0 == strlen(pLanguage) ) return ML_INVALID;

	if( 0 == stricmp("kor", pLanguage) )
		return ML_KOREAN;
	else if( 0 == stricmp("usa", pLanguage) )
		return ML_ENGLISH;
	else if( 0 == stricmp("jpn", pLanguage) )
		return ML_JAPANESE;
	else if( 0 == stricmp("brz", pLanguage) )
		return ML_BRAZIL;
	else if( 0 == stricmp("ind", pLanguage) )
		return ML_INDIA;
	else if( 0 == stricmp("grm", pLanguage) )
		return ML_GERMAN;
	else if( 0 == stricmp("spn", pLanguage) )
		return ML_SPANISH;
	else
	{
		ASSERT( 0 );
	}
	
	return ML_INVALID;
}