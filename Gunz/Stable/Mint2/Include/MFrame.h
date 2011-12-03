#ifndef MFRAME_H
#define MFRAME_H

#include "MWidget.h"
#include "MLookNFeel.h"
#include "MBmButton.h"

class MFrame;
class MFrameLook{
public:
	virtual void OnDraw(MFrame* pFrame, MDrawContext* pDC);
	virtual MRECT GetClientRect(MFrame* pFrame, MRECT& r);
};

struct MFrameBtn
{
	bool		m_bVisible;
	bool		m_bMouseOver;
	bool		m_bLButtonDown;
//	bool		m_bDowned;
	MRECT		m_Rect;
	MAnchors	m_Anchors;
	MFrameBtn() { 
		m_bVisible = false; 
		m_bLButtonDown = false; 
		m_bMouseOver = false; 
		m_Rect = MRECT(0,0,0,0);
	}
	bool IsButtonDown() { if ((m_bLButtonDown==true) && (m_bMouseOver==true)) return true; return false; }
};

/// ������ �� �ִ� ���������� ������ ����
class MFrame : public MWidget{
	DECLARE_LOOK(MFrameLook)
	DECLARE_LOOK_CLIENT()
protected:
	bool	m_bDragWidget;		///< Drag Widget
	MPOINT	m_DragPoint;

	bool	m_bMovable;			///< ������ �� �ִ��� ����
	bool	m_bCanShade;		///< Shade �� �� �ִ��� ���� - false�̸� Shade�� �� ����.
	bool	m_bShade;
	MSIZE	m_BeforeShade;		///< Before Shade

	MRECT		m_OldRect;
	MFrameBtn	m_BtnClose;
	MFrameBtn	m_BtnMinimize;

public:
	bool		m_bTitleBar;	///< Use Title Bar

protected:
	virtual bool OnCommand(MWidget* pWindow, const char* szMessage);
	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);
	virtual void OnSize(int w, int h);
	virtual bool OnShow(void);

	void ResizeBtnsByAnchors(int w, int h);
	void OnCloseButtonClick();
	void OnMinimizeButtonClick();
public:
	MFrame(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MFrame(void);

	void SetShade(bool bShade);
	void SetMovable(bool bValue) { m_bMovable = bValue; }
	MFrameBtn*	GetCloseButton() { return &m_BtnClose; }
	MFrameBtn*	GetMinimizeButton() { return &m_BtnMinimize; }

#define MINT_FRAME	"Frame"
	virtual const char* GetClassName(void){ return MINT_FRAME; }
};

#define MFRAME_CLOSE_MSG	"close"
#define MFRAME_MINIMIZE_MSG	"minimize"

#endif