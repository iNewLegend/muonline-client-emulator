#ifndef _MBASELOCALE_H
#define _MBASELOCALE_H


#include "MLocaleDefine.h"

// ���� �ڵ�
enum MCountry
{
	MC_INVALID			= 0,
	MC_KOREA			= 82,		// �ѱ�
	MC_US				= 1,		// �̱�(���ͳ׼ų�)
	MC_JAPAN			= 81,		// �Ϻ�
	MC_BRAZIL			= 55,		// �����
	MC_INDIA			= 91,		// �ε�

	// 10000�̻���� CustomID.
	MC_NHNUSA			= 10001		// NHN USA.
};

//
//#ifdef LOCALE_US
//	#define DEFAULT_COUNTRY			MC_US
//#elif LOCALE_JAPAN
//	#define DEFAULT_COUNTRY			MC_JAPAN
//#elif LOCALE_BRAZIL
//	#define DEFAULT_COUNTRY			MC_BRAZIL
//#elif LOCALE_INDIA
//	#define DEFAULT_COUNTRY			MC_INDIA
//#else
//	#define DEFAULT_COUNTRY			MC_KOREA
//#endif
//

/* TODO: MLangageConf.h �� �����ؾ��� - bird */

// LANG_xxx�� winnt.h�� ���ǵǾ� ����. - by SungE.
enum MLanguage
{
	ML_INVALID				= 0x00,
	ML_CHINESE				= LANG_CHINESE,					// �߱���
	ML_CHINESE_TRADITIONAL	= SUBLANG_CHINESE_TRADITIONAL,	// �߱��� ������ -  _-)a;; 
	ML_KOREAN				= LANG_KOREAN,					// �ѱ��� (LANG_KOREAN���� ����)
	ML_ENGLISH				= LANG_ENGLISH,					// ���� (LANG_ENGLISH���� ����)
	ML_JAPANESE				= LANG_JAPANESE,				// �Ϻ��� (LANG_JAPANESE���� ����)
	ML_BRAZIL				= LANG_PORTUGUESE,				// ����� (LANG_BRAZIL���� ����)
	ML_INDIA				= LANG_INDONESIAN,				// �ε� (LANG_INDONESIAN���� ����)
	ML_GERMAN				= LANG_GERMAN,					// ���Ͼ� (LANG_GERMAN���� ����)
	ML_SPANISH				= LANG_SPANISH,					// �����ξ� (LANG_SPANISH���� ����)
};

/// ����ȭ ���� �ֻ��� ���� Ŭ���� 
class MBaseLocale
{
private:
	void InitLanguageFromCountry();
protected:
	MCountry			m_nCountry;
	MLanguage			m_nLanguage;

	bool				m_bIsComplete;

	virtual bool OnInit() = 0;
public:
	MBaseLocale();
	virtual ~MBaseLocale();
	bool Init(MCountry nCountry);

	const MCountry	GetCountry()		{ return m_nCountry; }
	const MLanguage GetLanguage()		{ return m_nLanguage; }
	void SetLanguage(MLanguage langID)	{ m_nLanguage = langID; }	// ������ ���� �ʱ�ȭ�� �� �ٸ� ������ �ٲٰ� ������

	const bool bIsComplete()			{ return m_bIsComplete; }
};

const MCountry GetCountryID( const char* pCountry );
const MLanguage GetLanguageID( const char* pLanguage );

#endif