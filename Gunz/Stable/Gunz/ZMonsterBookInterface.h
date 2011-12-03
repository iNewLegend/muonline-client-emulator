/***********************************************************************
  ZMonsterBookInterface.h
  
  ��  �� : ���� ���� �������̽�
  �ۼ��� : 29, MAR, 2004
  �ۼ��� : �ӵ�ȯ
************************************************************************/


#ifndef _ZMONSTERBOOKINTERFACE_H
#define _ZMONSTERBOOKINTERFACE_H


#include <map>
using namespace std;


// ��� ������ ����
struct ZDropItemInfo
{
	string			m_strName;									// �̸�
	MBitmap*		m_pIcon;									// ������ ��Ʈ��

	ZDropItemInfo()
	{
		m_pIcon		= NULL;
	}
};


// ���� ���� ������ ����
class ZMonsterBookPageInfo
{
public:
	int						m_nID;								// ID
	string					m_strName;							// �̸�
	int						m_nGrade;							// ���
	string					m_strDesc;							// ����
	int						m_nHP;								// HP
	list<string>			m_Skill;							// Skill
	list<ZDropItemInfo*>	m_DropItem;							// Drop item
	float					m_fCompleteRate;					// �޼���

	ZMonsterBookPageInfo()
	{
		m_nID		= 0;
		m_nGrade	= 0;
		m_nHP		= 0;
		m_fCompleteRate = 0.0f;
	}

	virtual ~ZMonsterBookPageInfo()
	{
		m_Skill.clear();

		while ( !m_DropItem.empty())
		{
			delete *m_DropItem.begin();
			m_DropItem.pop_front();
		}
	}
};

typedef map<int,ZMonsterBookPageInfo*>		ZMonsterBookPage;
typedef ZMonsterBookPage::iterator			ZMonsterBookPageItr;




// Class : ZMonsterBookInterface
class ZMonsterBookInterface
{
// protected varialbes
protected:
	MBitmapR2*			m_pBookBgImg;							// ��� å �̹���
	MBitmapR2*			m_pIllustImg;							// �Ϸ���Ʈ ��Ʈ�� �̹���

	ZMonsterBookPage	m_mapMonsterBookPage;					// ���� ���� ������ ����Ʈ

	int					m_nTotalPageNum;						// ������ ��
	int					m_nCurrentPageNum;						// ���� �����ִ� ������ ��ȣ�� ���

	float				m_fCompleteRate;						// ��ü �޼���


// Functions
protected:
	void DrawPage( void);										// �������� �׸���
	bool LoadMonsterBookInfo( void);							// ������ ������ �ε��Ѵ�

public:
	ZMonsterBookInterface( void);								// Constructor
	virtual ~ZMonsterBookInterface( void);						// Destructor

	void OnCreate( void);										// On Create
	void OnDestroy( void);										// On destroy

	void OnPrevPage( void);										// ���� ������ �ѱ�� ��ư�� ������ ��
	void OnNextPage( void);										// ���� ������ �ѱ�� ��ư�� ������ ��
};


#endif
