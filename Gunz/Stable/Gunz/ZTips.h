#ifndef _ZTIPS_H
#define _ZTIPS_H

#include "MZFileSystem.h"
#include <string>
#include <vector>
#include <list>
using namespace std;

enum ZTIPS_CATEGORY
{
	ZTIPS_CATEGORY_NORMAL		=0,		// �Ϲ�
	ZTIPS_CATEGORY_SOLO			=1,		// ������
	ZTIPS_CATEGORY_TEAM			=2,		// ����
	ZTIPS_CATEGORY_CLANSERVER	=3,		// Ŭ����
	ZTIPS_CATEGORY_NEWBIE		=4,		// ���ʺ�
	ZTIPS_CATEGORY_BEGINNER		=5,		// �ʺ�
	ZTIPS_CATEGORY_ROOKIE		=6,		// �߼�
	ZTIPS_CATEGORY_MASTERY		=7,		// ���
	ZTIPS_CATEGORY_DUELTOURNAMENT =8,	// �����ʸ�Ʈ

	ZTIPS_CATEGORY_END
};

class ZTips
{
private:
	// type
	struct ZTIPS_MSG
	{
		int		nMSGID;
		int		nVectorIndex;
	};

	// data
	vector<string>		m_StringVector;
	vector<ZTIPS_MSG>	m_MsgsVectors[ZTIPS_CATEGORY_END];

	bool		m_bIsShowedNewbieTips;	// ���ʺ��� �޼����� �̹� ��������� ����. �̰� �����ִ°� �����Ѱ��� �� �𸣰���. -_-��

	// func
	void ParseTips(MXmlElement* pElement);
public:
	ZTips();
	~ZTips() { }
	bool Initialize(MZFileSystem *pfs, const MLanguage LangID );
	void Finalize();
	const char* GetRandomTips();
	const char* GetTips(ZTIPS_CATEGORY nCategory, int nID=-1);

	bool IsShowedNewbieTips()		{ return m_bIsShowedNewbieTips; }
	void SetShowedNewbieTips(bool bShowed)	{ m_bIsShowedNewbieTips = bShowed; }
};



#define ZTIPMSG_ID_NEWBIE_SHOW_HELP				1



#endif