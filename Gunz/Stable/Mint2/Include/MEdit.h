#ifndef MEDIT_H
#define MEDIT_H

#pragma warning(disable:4786)

#include "MWidget.h"
#include <list>
#include "MLookNFeel.h"
#include "MDebug.h"

using namespace std;

class MEdit;

/// Edit�� Draw �ڵ尡 �ִ� Ŭ����, �� Ŭ������ ��ӹ޾Ƽ� Ŀ���ҷ��� ���� �� �ִ�.
class MEditLook{
protected:
	bool		m_bCustomLook;
public:
	MEditLook(){}
	virtual void OnFrameDraw(MEdit* pEdit, MDrawContext* pDC);
	virtual void OnTextDraw(MEdit* pEdit, MDrawContext* pDC, bool bShowLanguageTab);

	virtual void OnDraw(MEdit* pEdit, MDrawContext* pDC, bool bShowLanguageTab = true);
	virtual MRECT GetClientRect(MEdit* pEdit, MRECT& r);

	void SetCustomLook(bool b) {
		m_bCustomLook = b;
	}
	bool GetCustomLook() const{
		return m_bCustomLook;
	}
};


/// Edit
class MEdit : public MWidget{
protected:
//	MPOINT		m_TextOffset;
	bool		m_bMouseOver;
	MCOLOR		m_TextColor;
	int			m_nMaxLength;
	char*		m_pBuffer;
	char		m_szIMECompositionString[MIMECOMPOSITIONSTRING_LENGTH];
	bool		m_bPassword;
	bool		m_bNumberOnly;	// ���ڸ� �Է� ������ ��Ÿ��
	int			m_nCaretPos;	
	int			m_nStartPos;
	MWidget*	m_pTabHandler;

public:
	int			m_nSelectionRange;	//< [m_nCaretPos, m_nCaretPos+m_nSelectionRange]
protected:
	list<char*>				m_History;			///< ���ڿ� �����丮
	list<char*>::iterator	m_nCurrentHistory;	///< ���ڿ� �����丮�� ���� ���ڿ�

	// Look & Feel
	DECLARE_LOOK(MEditLook)
	DECLARE_LOOK_CLIENT()

public:
	bool		m_bSupportHistory;

protected:
	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);
	
	/// Ű �Է��� ������ �� Ű���� ���� ó���� �� �ִ�.
	/// @return	true�� �����ϸ� InputFilter���� ó�������� �ǹ��Ѵ�. \n
	///			false�� �����ϸ� Edit�� ��Ʈ���� �߰����� �ǹ��Ѵ�.
	virtual bool InputFilterKey(int nKey);	// MWM_KEYDOWN
	virtual bool InputFilterChar(int nKey);	// MWM_CHAR

	virtual void OnSetFocus(void);
	virtual void OnReleaseFocus(void);

	virtual void OnChangedText();

	void Initialize(int nMaxLength, const char* szName);
public:
	MEdit(int nMaxLength, const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	MEdit(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL );
	virtual ~MEdit();

	virtual void OnHide(void);

	//void SetTextOffset(MPOINT p);
	//void SetTextColor(MCOLOR color);
	//MCOLOR GetTextColor(void);

	virtual void SetText(const char* szText);
	virtual const char* GetText(void);
	void AddText(const char* szText);

	int MoveCaretHome(void);
	int MoveCaretEnd(void);
	int MoveCaretPrev(void);
	int MoveCaretNext(void);

	void SetMaxLength(int nMaxLength);
	int GetMaxLength(void);
	const char* GetCompositionString(void);
	int GetCarretPos(void);
	int GetStartPos(void) { return m_nStartPos; }
	bool SetStartPos(int nStartPos);
	int GetPosByScreen(int x);

	void SetPasswordField(bool bPassword);
	bool IsPasswordField(void);

	void SetNumberField(bool b);
	bool IsNumberField();

	bool GetClipboard(char* szText, int nSize);
	bool SetClipboard(const char* szText);

	void AddHistory(const char* szText);

	MWidget* GetTabHandler()				{ return m_pTabHandler; }
	void SetTabHandler(MWidget* pWidget)	{ m_pTabHandler = pWidget; }

#define MINT_EDIT	"Edit"
	virtual const char* GetClassName(void){ return MINT_EDIT; }
};

#define MEDIT_KEYDOWN_MSG		"keydown"	///< Ű�� �Է�������
#define MEDIT_CHAR_MSG			"char"		///< Ű�� �Է�������
#define MEDIT_ENTER_VALUE		"entered"	///< EnterŰ�� �Է�������
#define MEDIT_ESC_VALUE			"esc"		///< ESCŰ�� �Է�������
#define MEDIT_KILL_FOCUS		"killfocus" ///< ��Ŀ���� �Ҿ�����
#define MEDIT_TEXT_CHANGED		"textchgd"	///< �ؽ�Ʈ�� ����Ǿ�����



#define MEDIT_BLINK_TIME		400		///< Ŀ���� �����̴� ������, ������ mili-second
#endif