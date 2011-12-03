//
// MINT ( MAIET In-house wiNdows sysTem )
//
////////////////////////////////////////////////////////////////////////////////
#ifndef MWIDGET_H
#define MWIDGET_H

#include "CMPtrList.h"
#include "MDrawContext.h"
#include "MTypes.h"
#include "MEvent.h"



class MWidget;

/// �������� �޼����� ���� �� �ִ� Listener(Pure Virtual Class)
class MListener{
public:
	/// �޼����� �ޱ����� Virtual Function
	/// @param	pWidget		�޼����� ������ ���� ������
	/// @param	szMessage	�޼���(��Ʈ��)
	virtual bool OnCommand(MWidget* pWidget, const char* szMessage) = 0;
};


class MToolTip;
class MResourceMap;

/// ������ Z ����
enum MZOrder{
	MZ_TOP = 0,		///< �� ������
	MZ_BOTTOM,		///< �� �ڷ�
};

struct MAnchors
{
	bool m_bLeft;
	bool m_bRight;
	bool m_bTop;
	bool m_bBottom;
	MAnchors(void)	{ m_bLeft = true; m_bRight = false; m_bTop = true; m_bBottom = false; }
	MAnchors(bool bLeft, bool bRight, bool bTop, bool bBottom)
					{ m_bLeft = bLeft; m_bRight = bRight; m_bTop = bTop; m_bBottom = bBottom; }
};

#define MWIDGET_NAME_LENGTH		256			///< ���� �̸� �ִ� ����

/// �⺻ ���� Ŭ����
class MWidget : public MListener{
private:
	bool				m_bEnable;			///< Enable
	bool				m_bFocusEnable;		///< Focus Enable

	MListener*			m_pListener;		///< Listener
	MToolTip*			m_pToolTip;			///< �⺻ ����

	int					m_nAndPos;
	int					m_nAccelerator;		///< Accelerator Key

protected:
	bool				m_bVisible;			///< Visible Flag

	CMPtrList<MWidget>	m_Children;			///< Children Widgets

	MWidget*			m_pParent;			///< Parent Widget
	CMPtrList<MWidget>	m_Exclusive;		///< Exclusive Child Widget Stack ( for Modal Support )

	MCursor*			m_pCursor;			///< Default Cursor of this Widget
	MFont*				m_pFont;			///< Default Font of this Widget

	static MWidget*		m_pCapturedWidget;	///< SetCapture(), ReleaseCapture()
	static MWidget*		m_pFocusedWidget;	///< SetFocus()

	bool				m_bZOrderChangable;	///< Z Order Change by L-Button Click
	bool				m_bResizable;		///< Resizable?

	int					m_nResizeSide;		///< 0: N/A   1: Up  2: Right  4: Down  8: Left(BitFlag)

	unsigned char		m_nOpacity;			///< ������
//	bool				m_bVisibleChildren;	///< Children Widget�� Visibility

	bool				m_bClipByParent;	///< Parent Widget�� ���� Clip�Ǵ°�?

	MAlignmentMode		m_BoundsAlignment;	///< m_Rect�� ���� Alignment

public:
	char				m_szName[MWIDGET_NAME_LENGTH];	///< Name
	char				m_szIDLName[MWIDGET_NAME_LENGTH];	///< Name

	MRECT				m_Rect;				///< Rect in Parent Widget
	MRECT				m_IDLRect;			///< IDL ���� �о������� �ʱ� rect

	MAnchors			m_Anchors;			///< Parent Widget�� ����ؼ� ��ġ�ϴ°�?
	int					m_nMinWidth, m_nMinHeight;	///< ������ �ּ� ũ��

	bool				m_bisListBox;
	int					m_nDebugType;
	bool				m_bEventAcceleratorCall;///< Ű�Է��̺�Ʈ�� �ҷȴ��� ����
protected:
	// Only for Designer Mode
	bool				m_bEnableDesignerMode;	///< Designer Mode Ȱ��ȭ �÷���
	int					m_nDMDragWidget;		///< ���� Move �Ǵ� Resize �÷���
	MPOINT				m_DMDragPoint;			///< ���� Move �Ǵ� Resize ������
	bool				m_bModifiedByDesigner;	///< Designer�� ���� ��ġ,ũ�Ⱑ ������ ���
	bool				m_bAddedByDesigner;		///< Designer�� ���� �߰��� ���
	int					m_nID;					///< ������ ID

private:
	/// Local Coordinate�� �̿��� Local Event�� ��ȯ
	void MakeLocalEvent(MEvent* pLoalEvent, const MEvent* pEvent);
	/// Event���� Resize�κ� ó��
	bool EventResize(MEvent* pEvent);
protected:
	/// Child Widget �� �տ� �߰�
	void InsertChild(MWidget* pWidget);
	/// Child Widget �߰�
	void AddChild(MWidget* pWidget);
	/// Child Widget ����
	void RemoveChild(MWidget* pWidget);
	// Accelerator�� szText�� &�ڿ� ������ ���ڷ� �Ѵ�.
	//void UseAcceleractorAndCharacter(char* szText);

	/// Exclusive Stack�� �߰��Ѵ�.
	void AddExclusive(MWidget* pWidget);
	/// Exclusive Stack���� �����Ѵ�.
	bool RemoveExclusive(MWidget* pWidget);

	/// Runtime Loop
	virtual void OnRun(void);
	/// Draw Handler
	virtual void OnDraw(MDrawContext* pDC);
	/*
	/// Draw Handler
	/// @deprecated	3D Rendering Pass�� ���� ó���ϱ� ���� �ڵ�� ���̻� ������� �ʴ´�.
	virtual void OnDraw3D(MDrawContext3D* pDC);
	*/
	/// Event Handler. Event�� ó���Ǹ� return true, �ƴϸ� return false.
	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);

	/// ��Ű�� �������� �ҷ����� �̺�Ʈ �ڵ鷯
	//virtual bool OnHotKey(int a, bool bShift, bool bCtrl, bool bAlt){ return false; }
	virtual bool OnHotKey(int nID){ return false; }

	/// Show()ȣ�⿡ ���� Event Handler
	virtual bool OnShow(void);
	virtual void OnHide(void);

	/// ��Ŀ�� �ڵ鷯
	virtual void OnSetFocus(void){}
	virtual void OnReleaseFocus(void){}

	/// SetSize(), SetBounds()ȣ�⿡ ���� Event Handler
	virtual void OnSize(int w, int h);

	/// Show(), Hide()ȣ�⿡ ���� Event Handler
	void OnShow(bool bVisible);

	/// Tab Key
	virtual bool OnTab(bool bForward=true);

	/// Drop&Drop�� ���� �������� �ڵ鷯
	virtual bool OnDrop(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString);

	/// Listener Event Handler ( ������ �޼����� ���������� return true�� �Ѵ�. )
	virtual bool OnCommand(MWidget* pWidget, const char* szMessage){ return false; }

	/// Anchor ������ ���� Children Resize
	/// @param w	new width
	/// @param h	new height
	void ResizeChildrenByAnchor(int w, int h);

	/// Bounds Alignment�� ���� ��ġ��
	void GetBoundsAlignmentPosition(MPOINT* p, MAlignmentMode am, int w=-1, int h=-1);

public:
	/// Workspace�� ����ɶ� ȣ��Ǵ� �Լ�
	//virtual void OnReSize();

public:
	/// @param szName	���� �̸�
	/// @param pParent	�θ� ����
	/// @param szName	�޼����� ���޹��� ������, ������ �θ� ������ �ش�ȴ�.
	MWidget(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MWidget(void);

	/// Loop�� �������� Widget System���� ó���� ���� ������ �����Ѵ�.
	void Run(void);
	/// �� Widget�� ����� Child Widget�� �׸���.
	void Draw(MDrawContext* pDC);
	/// UI��ο찡 ��� ���� �� �� ���� ���� pDC�� ����� Ư���� ������ �۾��� ������ �� �ֵ��� ��ȸ ����
	virtual void DrawAfterWidgets(MDrawContext* pDC);
	/// ���� �ٽ� �׸���.
	void Redraw(void);
	/// �̺�Ʈ�� ó���� �ڵ鷯 ȣ��
	bool Event(MEvent* pEvent);
	/// Ű���� Accelerator �̺�Ʈ�� ó��
	bool EventAccelerator(MEvent* pEvent);
	/// ����Ʈ Ű(Enter, ESC) �ڵ鷯 ȣ��
	bool EventDefaultKey(MEvent* pEvent);

	/// ������ �����ش�.
	/// @param bVisible	Visible Flag
	/// @param bModal	Modal Dialog Flag
	virtual void Show(bool bVisible=true, bool bModal=false);
	/// ������ �����.
	void Hide(void){ Show(false); }
	/// ������ Ȱ��ȭ ��Ų��.
	/// @param bEnable	Enable Flag
	void Enable(bool bEnable=true);
	/// ������ ���̴°�?
	bool IsVisible(void);
	/// ������ Ȱ��ȭ �Ǿ��°�?
	bool IsEnable(void);

	void SetResizable(bool bEnable);
	bool IsResizable(void);

	/// ������ ����
	/// @param pListener	������ ������
	virtual void SetListener(MListener* pListener);
	/// ���� ������ ���
	/// @return ���� ������
	virtual MListener* GetListener(void);


	/// ���� ID ���
	int GetID(void);
	/// ���� ID �����ϱ�
	void SetID(int nID);

	/// ���� �ؽ�Ʈ(Name) �����ϱ�
	virtual void SetText(const char* szText);
	/// ���� �ؽ�Ʈ(Name) ���
	virtual const char* GetText(void);

	/// ���� ������ ��� ���콺 �޼����� ������ ĸ��
	void SetCapture(void);
	/// ���� ĸ�ĵ� ������ ����
	void ReleaseCapture(void);

	void SetFocusEnable(bool bEnable);
	bool IsFocusEnable(void);

	/// ������ ��Ŀ���� ��ġ��Ų��. ��Ŀ���� ������ Ű���� �޼����� �켱������ ����.
	void SetFocus(void);
	/// ������ ��Ŀ���� ������Ų��.
	void ReleaseFocus(void);
	/// ������ ��Ŀ���� ������ �ִ°�?
	bool IsFocus(void);

	/// �θ� ������ ����.
	MWidget* GetParent(void);
	/// �ڽ� ���� ������ ����.
	int GetChildCount(void);
	/// �ڽ� ������ ����.
	/// @param i	�ڽ� ���� �ε���
	MWidget* GetChild(int i);
	/// �ڽ� ������ �ε����� ����.
	/// @param	pWidget	�ڽ� ����
	/// @return	�ڽ� ������ �ε���
	int GetChildIndex(MWidget* pWidget);

	/// Exculsive ���(Modal)�� ��ȯ�Ѵ�. Exclusive ���� �����Ǿ� �ִ� ������ ��� �޼����� �����Ѵ�.
	void SetExclusive(void);
	/// Exclusive ��带 �����ϳ�.
	void ReleaseExclusive(void);
	/// Exclusive Stack�� ���� ������ ������ ����.
	MWidget* GetLatestExclusive(void);
	/// Child Widget�� Exclusive���� �׽�Ʈ
	bool IsExclusive(MWidget* pWidget);

	/// ������ Ŀ���� �����Ѵ�.
	/// @param pCursor	������ ������ Ŀ��
	/// @return			������ �����Ǿ� �ִ� Ŀ��
	MCursor* SetCursor(MCursor* pCursor);
	/// ���� Ŀ���� ����.
	MCursor* GetCursor(void);

	/// ������ ��Ʈ�� �����Ѵ�.
	MFont* SetFont(MFont* pFont);
	/// ���� ��Ʈ�� ����.
	MFont* GetFont(void);

	/// Size �����ϱ�
	/// @param w, h		���� ���� ũ��
	void SetSize(int w, int h);
	/// Size �����ϱ�
	/// @param s		������
	void SetSize(MSIZE& s);

	/// Parent Widget�� Local ��ǥ�迡�� Poition �����ϱ�
	/// @param x, y		��ġ
	void SetPosition(int x, int y);
	/// Parent Widget�� Local ��ǥ�迡�� Poition �����ϱ�
	/// @param p		��ġ
	void SetPosition(MPOINT& p);
	/// Parent Widget�� Local ��ǥ�迡�� Initial Rect �����ϱ�
	void SetInitialBounds(MRECT& r);
	/// Parent Widget�� Local ��ǥ�迡�� Rect �����ϱ�
	/// @param r		Rectangle
	void SetBounds(MRECT& r);
	/// Parent Widget�� Local ��ǥ�迡�� Rect �����ϱ�
	/// @param x,y,w,h		Rectangle
	void SetBounds(int x, int y, int w, int h);
	/// Parent Widget�� Local ��ǥ�迡�� Position ����
	MPOINT GetPosition(void);
	/// Parent Widget�� Local ��ǥ�迡�� Rect ����
	MRECT GetRect(void);
	/// idl���� ���� rect�� ��´�
	MRECT GetIDLRect(void);
	/// ��ġ������ ����θ�Ʈ ����
	void SetBoundsAlignment(MAlignmentMode am, int w, int h);
	/// ��ġ������ ����θ�Ʈ ���
	MAlignmentMode GetBoundsAlignment(void);

	void SetOpacity(unsigned char nOpacity);
	unsigned char GetOpacity();

	/// Screen ��ǥ�迡�� Rect ����
	/// @deprecated		���� ��ǥ��� ȥ���� �������Ƿ� ���� ��ǥ�� �Լ���� ��ü, ���� ��ǥ�� �˰� ������ MClientToScreen(),MScreenToClient()�� �̿�
	MRECT GetScreenRect(void);

	/// ���� ��Ʈ�� ����
	void AttachToolTip(const char* szToolTipString=NULL);
	/// ���� ������Ʈ ����
	void AttachToolTip(MToolTip* pToolTip);
	/// ������ ����
	void DetachToolTip(void);
	/// ���� ���� ������Ʈ ���
	MToolTip* GetToolTip(void);

	/// Accelerator ����
	void SetAccelerator(int a);
	/// Label���� &������ ������ ���ڷ� Accelerator ����
	void SetLabelAccelerator(void);
	/// ���� Accelerator ���
	char GetLabelAccelerator(void);
	/// ���� ToolTip�� Accelerator ���
	char GetToolTipAccelerator(void);

	/// Widget�� Ŀ���͸������ Client Rect
	virtual MRECT GetClientRect(void);
	/// Widget�� ���� Client Rect(0, 0, width, height)
	MRECT GetInitialClientRect(void);

	/// Child Widget ������ �ٲ� �׸��� ������ �����Ѵ�.
	void SetZOrder(MZOrder z);
	/// �ڼ� Widget�� ��ó�� �߰ߵ� Exclusive Widget ����
	MWidget* FindExclusiveDescendant(void);

	/// ��ġ�� ���� �˻�
	MWidget* Find(int x, int y){ return Find(MPOINT(x, y)); }
	/// ��ġ�� ���� �˻�
	MWidget* Find(MPOINT& p);
	MWidget* FindDropAble(MPOINT& p);

	/// �ش� ID�� Drag Object�� Drop�����Ѱ�?
	virtual bool IsDropable(MWidget* pSender){ return false; }
	bool Drop(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString);

	void SetVisible(bool b) { m_bVisible = b; }
	bool GetVisible() { return m_bVisible; }

	/// ���� ������ ������ �̸� ���� ( ��: Parent/Child )
	void GetHierarchicalName(char* szName);
	/// ���� ������ ������ �̸����� ���� ã��
	MWidget* FindWidgetByHierarchicalName(const char* szName);

	/// ���� �������� Accelerator� ���� ����� �� �ִ� Default Command�� ���� ( �������̵��� ��� return true�� �Ѵ�. )
	virtual bool DefaultCommand(void){ return false; }

	/// �޼����� ��ġ�ϴ°�?
	static bool IsMsg(const char* szMsg1, const char* szMsg2);

	/// Query Anything
	virtual void* Query(const char* szQuery);

	void SetClipByParent(bool b) {
		m_bClipByParent = b;
	}

	/// �Ϲ� ������� �ƴ� �ܼ��� ���������ϱ� ���� �Լ� �ڼ��� �ּ��� cpp����
	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);


#define MINT_WIDGET	"Widget"
	/// Ŭ���� �̸� ���
	virtual const char* GetClassName(void){ return MINT_WIDGET; }
};


// Help Functions
/// &�� �ִ� ��ġ�� �˷��ش�. szText�� NULL�̸� m_szName���� &�� ��ġ�� ã�´�.
int GetAndPos(const char* szText);
/// & �ڿ� �پ��ִ� ���ڸ� ����.
char GetAndChar(const char* szText);
/// &���� ����
int RemoveAnd(char* szText);
/// &���� ����
/// @param szRemovedText	[out]	���ŵ� ���ڿ�
/// @param szText			[in]	�ҽ� ���ڿ�
int RemoveAnd(char* szRemovedText, const char* szText);
/// &���� ����
/// @param szRemovedFrontText	[out]	&���� ���ڿ�
/// @param cUnderLineChar		[out]	&�� ���� ������ ���� ���ڿ�
/// @param szRemovedBackText	[out]	&���� ���ڿ�
/// @param szText				[in]	�ҽ� ���ڿ�
int RemoveAnd(char* szRemovedFrontText, char* cUnderLineChar, char* szRemovedBackText, const char* szText);

/// ���� ��ġ�� ���� ��ġ�� ��ȯ
MPOINT MClientToScreen(MWidget* pWidget, MPOINT& p);
/// ���� ��ġ�� ���� ��ġ�� ��ȯ
MPOINT MScreenToClient(MWidget* pWidget, MPOINT& p);
/// ���� ������ ���� �������� ��ȯ
MRECT MClientToScreen(MWidget* pWidget, MRECT& p);
/// ���� ������ ���� �������� ��ȯ
MRECT MScreenToClient(MWidget* pWidget, MRECT& p);

/// ���� Ŀ�� ��ġ ���
MPOINT GetCursorPosition(void);

/// ���� ���� Ŭ�������� �˻�
inline bool MIsSameWidgetClass(MWidget* pWidget, const char* szClassName);
/// ���� ���� Ŭ�������� �˻�
inline bool MIsSameWidgetClass(MWidget* pWidgetA, MWidget* pWidgetB);


#endif
