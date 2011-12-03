#pragma once


// CReportCtrl

#define NUM_COLUMNS 12

static const char*	COLUMN_NAME[ NUM_COLUMNS ]	= { "�����̸�", 
													"IP",
													"S-����",
													"A-����",
													"�������", 
													"���������˻�",
													"������ �۾� ����", 
													"S-�ڵ�����", 
													"A-�ڵ�����",
													"��ġ�ޱ�", 
													"��ġ�غ�", 
													"��ġ����" };

static const int	COLUMN_LENTH[ NUM_COLUMNS ] = { 100, 100, 50, 50, 175, 100, 150, 75, 75, 60, 60, 60 };

static const int	REPORT_WND_STYLE = LVS_REPORT | WS_BORDER | LVS_SHOWSELALWAYS;

class CReportCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CReportCtrl)
	
public:
	CReportCtrl();
	virtual ~CReportCtrl();

	static CReportCtrl& GetInst()
	{
		static CReportCtrl Report;
		return Report;
	}

	void InsertColumns();
	void InsertItems();
	void Update( const int nID );

	BOOL Create( CWnd* pParentWnd, UINT nID );

	void SetGridLines( bool bSet );
	void SetCheckboxStyle( int nStyle = 1 );
	void SetFullrowSelectStyle( bool bSet );

private :
	int FindSelectedColumn( const CPoint Point );
	int FindSelectedItem();
	void ConvertSelectedColumnConfigState( const int nItemID, const int nColumnID );

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

#define GetReport CReportCtrl::GetInst()