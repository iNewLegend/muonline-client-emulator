#ifndef MBUTTON_H
#define MBUTTON_H

#include "MWidget.h"
#include "MLookNFeel.h"

class MMsgBox;
class MButtonGroup;

/// ��ư Ÿ��
enum MButtonType{
	MBT_NORMAL = 0,		///< �Ϲ� ��ư
	MBT_PUSH,			///< Ǫ�� ��ư
	MBT_PUSH2,
};

/// ����Ʈ Ű ���
enum MButtonKeyAssigned{
	MBKA_NONE = 0,		///< �ƹ�Ű�� ��ϵǾ� ���� ���� ��ư
	MBKA_ENTER,			///< Enter Ű ������ ����Ǵ� ��ư
	MBKA_ESC,			///< ESC Ű ������ ����Ǵ� ��ư
};

class MButton;
/// Button�� Draw �ڵ尡 �ִ� Ŭ����, �� Ŭ������ ��ӹ޾Ƽ� Ŀ���ҷ��� ���� �� �ִ�.
class MButtonLook{
protected:
	bool	m_bWireLook;
protected:
	virtual void OnDownDraw(MButton* pButton, MDrawContext* pDC);
	virtual void OnUpDraw(MButton* pButton, MDrawContext* pDC);
	virtual void OnOverDraw(MButton* pButton, MDrawContext* pDC);
	virtual void OnDisableDraw(MButton* pButton, MDrawContext* pDC);
	virtual void OnCheckBoxDraw(MButton* pButton, MDrawContext* pDC, bool bPushed){};
public:
	virtual void OnDrawText(MButton* pButton, MRECT& r, MDrawContext* pDC);
	virtual void OnDraw(MButton* pButton, MDrawContext* pDC);
	virtual MRECT GetClientRect(MButton* pButton, MRECT& r);
	void SetWireLook(bool b) {	m_bWireLook = b;	}
	bool GetWireLook() const{	return m_bWireLook; }
public:
	MButtonLook():m_bWireLook(false){}
};


/// ��ư Ŭ����
class MButton : public MWidget{
protected:
	bool		m_bMouseOver;
	bool		m_bLButtonDown;
	bool		m_bRButtonDown;
	MCOLOR		m_TextColor;
	bool		m_bShowText;
	MAlignmentMode	m_AlignmentMode;
	MButtonType		m_Type;
	bool		m_bChecked;				///< MBT_PUSH�� ���
	bool		m_bComboDropped;
	bool		m_bStretch;
	
	MButtonGroup	*m_pButtonGroup;	///< �����ִ� �׷�
	int				m_nIndexInGroup;	///< ���� �׷쳻������ index

public:
	bool		m_bEnableEnter;			///< ��Ŀ�� ������ EnterŰ�� ��ư ������ Ȱ��ȭ ( Default : true )
	bool		m_bHighlight;			///< Highlight(&) ����
	MBitmap*	m_pIcon;				///< ������
	MButtonKeyAssigned	m_nKeyAssigned;	///< Key Assigned
	MMsgBox*	m_pMsgBox;				///< �޼��� �ڽ��� ���� Ȯ�� �޼���
	string		m_strIDLConfirmText;	///< xml�� ���ǵ� Ȯ�� �޽���

	MPOINT		m_ClickPos;
	MPOINT		m_LDragStartClickPos;
	int			m_LDragVariationX;
	int			m_LDragVariationY;

private:
	//void DrawButton(MDrawContext* pDC);
protected:
	//virtual void OnDraw(MDrawContext* pDC);
	/// ���콺�� ��ư �ȿ� ���ö� �߻��ϴ� �̺�Ʈ �ڵ鷯
	virtual void OnMouseIn(void);
	/// ���콺�� ��ư ������ ���� ������ �߻��ϴ� �̺�Ʈ �ڵ鷯
	virtual void OnMouseOut(void);
	/// ��ư�� ������ �߻��ϴ� �̺�Ʈ �ڵ鷯
	virtual void OnButtonDown(void);
	/// ��ư�� ������� ������ �߻��ϴ� �̺�Ʈ �ڵ鷯
	virtual void OnButtonUp(void);
	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);

	/// Ŭ���Ǿ�������
	virtual void OnButtonClick(void);	

	virtual bool OnShow(void);
	virtual void OnHide(void);
public:
	MButton(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MButton(void);
	/// ��ư �ؽ�Ʈ �÷� ����
	void SetTextColor(MCOLOR color);
	/// ��ư �ؽ�Ʈ �÷� ���
	MCOLOR GetTextColor(void);
	/// ��ư�� �ؽ�Ʈ �����ֱ�
	void ShowText(bool bShow=true);
	virtual bool DefaultCommand(void);

	/// ��ư�� �ؽ�Ʈ ���� ���
	MAlignmentMode GetAlignment(void);
	/// ��ư�� �ؽ�Ʈ ���� ����
	MAlignmentMode SetAlignment(MAlignmentMode am);

	/// ��ư Ÿ�� ����
	void SetType(MButtonType t);
	/// ��ư Ÿ�� ���
	MButtonType GetType(void);

	/// Push ��ư�� ��� üũ
	void SetCheck(bool bCheck);
	/// Push ��ư�� ��� üũ�� ���
	bool GetCheck(void);

	/// ��ư�� ������ �ִ°�? ( ���콺�� ������ ���¸� �� �� �ִ�. )
	bool IsButtonDown(void);	// Async Button Down ?
	/// ���콺�� ��ư ���� �÷��� �ִ°�?
	bool IsMouseOver(void);

	/// Confirm Message Box
	void SetConfirmMessageBox(const char* szMessage);			/// ����Ʈ Ȯ�� �޽���
	void SetAlterableConfirmMessage(const char* szMessage);		/// Ư���� ��쿡 Ȯ�� �޽����� �ѵ��� �ٲ�ġ�� �� �� �ִ�
	void RestoreIDLConfirmMessage();							/// Ȯ�� �޽����� �ٲ�ġ���� �ʿ��� å������ ��������� �Ѵ�~

	virtual bool OnCommand(MWidget* pWidget, const char* szMessage);

	int GetLDragVariationX() {
		return m_LDragVariationX;
	}

	int GetLDragVariationY() {
		return m_LDragVariationY;
	}

	DECLARE_LOOK(MButtonLook)
	DECLARE_LOOK_CLIENT()

	void SetComboDropped(bool b) {
		m_bComboDropped = b;
	}
	bool GetComboDropped() const{
		return m_bComboDropped;
	}

	void SetStretch( bool b) {
		m_bStretch = b;
	}

	bool GetStretch( ) const {
		return m_bStretch;
	}

	void SetButtonGroup(MButtonGroup *pGroup);

	int GetIndexInGroup() { 
		return m_nIndexInGroup; 
	}

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);

#define MINT_BUTTON	"Button"
	virtual const char* GetClassName(void){ return MINT_BUTTON; }
};

#define MBTN_CLK_MSG		"click"	///< ��ư�� Ŭ���ɶ� Listener���� �߻��Ǵ� �޼���
#define MBTN_RCLK_MSG		"rclick"///< R ��ư�� Ŭ���ɶ� Listener���� �߻��Ǵ� �޼���
#define MBTN_DN_MSG			"down"	///< ��ư�� ������
#define MBTN_UP_MSG			"up"	///< ��ư�� �������
#define MBTN_RDN_MSG		"rdown"	///< R��ư�� ������
#define MBTN_RUP_MSG		"rup"	///< R��ư�� �������
#define MBTN_IN_MSG			"in"	///< ��ư ���� Ŀ���� �÷� ����
#define MBTN_OUT_MSG		"out"	///< ��ư ���� Ŀ���� ������ �÷� ����


// ��ư �׷��� �����ϴµ� �ʿ��� Ŭ����
class MButtonGroup {
	friend MButton;
public:
	MButtonGroup();
	~MButtonGroup();

protected:
	int		m_nCount;
	MButton *m_pPrevious;
};

#endif