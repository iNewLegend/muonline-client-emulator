#ifndef MMULTICOLLISTBOX_H
#define MMULTICOLLISTBOX_H

#include "MWidget.h"
#include "MScrollBar.h"
#include "MColorTable.h"
#include "MListBox.h"
#include <list>

// multi column listbox

class MMultiColListBox;
class MMultiColListBoxLook
{
	MAlignmentMode	m_ItemTextAlignmentMode;

public:
	MMultiColListBoxLook();

	void OnDraw(MMultiColListBox* pListBox, MDrawContext* pDC);
	MRECT GetClientRect(MMultiColListBox* pListBox, MRECT& r);
};

// �������� ����ؼ� �����ϵ��� �󰳸� �غ�
class MMultiColListItem
{
public:
	virtual ~MMultiColListItem() {}
	virtual void OnDraw(MRECT& r, MDrawContext* pDC, bool bSelected, bool bMouseOver);
	virtual const char* GetString() { return ""; }

	// �巡�� & ����� ������ ���۵� ��Ʈ��
	virtual bool GetDragItem(MBitmap** ppDragBitmap, char* szDragString, char* szDragItemString){
		return false;
	}

	virtual int GetSortHint() { return 0; }
};

typedef std::list<MMultiColListItem*>		ListMultiColListItem;
typedef list<MMultiColListItem*>::iterator	ItorMultiColListItem;

class MMultiColListBox : public MWidget
{
	ListMultiColListItem m_items;

	int m_numColumn;		// �÷� ����
	int m_desiredNumRow;	// �ѹ��� �����ְ� ���� ���
	int m_itemHeight;

	int m_maxRowCanShow; // �� ȭ�鿡 ������ �� �ִ� �ִ� �� ��

	MScrollBar* m_pScrollBar;

	int	m_nOverItem;			// Ŀ���� ���� �������� ������
	int m_nSelItem;
	bool m_bDragAndDrop;

	ZCB_ONDROP m_pOnDropFunc;

	void SetItemHeight(int h);
	void InsertSortedPos(MMultiColListItem* pItem);

public:
	MAlignmentMode m_FontAlign;

public:
	MMultiColListBox(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MMultiColListBox();


	ListMultiColListItem& GetItemList() { return m_items; }

	void SetDesiredNumRow(int n);
	void CalcItemHeight();

	int GetItemHeight() { return m_itemHeight; }
	int GetItemWidth() { return GetClientRect().w / m_numColumn; }

	void SetNumColumn(int n);
	int GetNumColumn() { return m_numColumn; }

	int GetNumItem() { return (int)m_items.size(); }

	MScrollBar* GetScrollBar() { return m_pScrollBar; }
	int GetMaxRowCanShow() { return m_maxRowCanShow; }

	int GetRowFirstVisible() {
		return GetScrollBar()->GetValue();
	}
	int GetRowLastVisible() {
		return GetRowFirstVisible() + GetMaxRowCanShow() - 1;
	}
	int GetItemFirstVisible() {
		return GetRowFirstVisible() * m_numColumn;
	}
	int GetItemLastVisible() {
		int idxItemLastShow = GetItemFirstVisible() + (GetMaxRowCanShow() * m_numColumn) - 1;
		return min(idxItemLastShow, GetNumItem()-1);
	}

	void SetRowFirstVisible(int n) { GetScrollBar()->SetValueAdjusted(n); }

	void Add(MMultiColListItem* pItem);
	void Remove(MMultiColListItem* pItem);
	void RemoveAll();

	bool GetItemRowCol(int idx, int& out_row, int& out_col);
	int GetSelIndex() { return m_nSelItem; }
	int GetMouseOverIndex() { return m_nOverItem; }
	MMultiColListItem* GetItemByIdx(int idx);
	MMultiColListItem* GetSelItem();
	bool SetSelIndex(int i);

	bool CalcItemRect(int idx, MRECT& out);
	int FindItem(MPOINT& p);

	void EnableDragAndDrop( bool bEnable) { m_bDragAndDrop = bEnable; }
	int FindNextItem(int i, char c);	// ����Ű �Է����� �׸� ã��

	const char* GetSelItemString();

	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);
	virtual bool IsDropable(MWidget* pSender) { return m_bDragAndDrop; }

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);

	MMultiColListItem* Get(int i);
	const char* GetString(int i);
	bool IsSelected() { return m_nSelItem != -1; }
	void ShowItem(int i);

	void SetOnDropCallback(ZCB_ONDROP pCallback) { m_pOnDropFunc = pCallback; }
	bool OnDrop(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString);

	bool GetItemPos(MPOINT* p, int i) { return false; }	// ���� ���ó�� ���� ��������

protected:
	virtual void OnSize(int w, int h);

	void UpdateScrollBar();

public:
	DECLARE_LOOK(MMultiColListBoxLook)
	DECLARE_LOOK_CLIENT()

#define MINT_MULTICOLLISTBOX	"MultiColListBox"
	/// Ŭ���� �̸� ���
	virtual const char* GetClassName(void){ return MINT_MULTICOLLISTBOX; }
};

#endif
