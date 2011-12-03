#ifndef MPANEL_H
#define MPANEL_H

#include "MWidget.h"
#include "MDrawContext.h"
#include "MLookNFeel.h"

class MPanel;

/// Panel�� Draw �ڵ尡 �ִ� Ŭ����, �� Ŭ������ ��ӹ޾Ƽ� Ŀ���ҷ��� ���� �� �ִ�.
class MPanelLook{
protected:
	virtual void OnFrameDraw(MPanel* pPanel, MDrawContext* pDC);
public:
	
	virtual void OnDraw(MPanel* pLabel, MDrawContext* pDC);
	virtual MRECT GetClientRect(MPanel* pLabel, MRECT& r);
};

/// Panel�� ������ �׸��� �� Ÿ��
enum MBorderStyle {
	MBS_NONE = 0,		///< �� ����
	MBS_SINGLE,			///< single-line ��
};

/// Panel
class MPanel : public MWidget{
protected:
	DECLARE_LOOK(MPanelLook)
	DECLARE_LOOK_CLIENT()
protected:
	MBorderStyle	m_nBorderStyle;
	MCOLOR			m_BorderColor;
	MCOLOR			m_BackgroundColor;
public:
	MPanel(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

	/// ������ �÷� ����
	void SetBackgroundColor(MCOLOR color);
	/// ������ �÷� ���
	MCOLOR GetBackgroundColor(void);

	/// �ܰ��� �÷� ����
	void SetBorderColor(MCOLOR color);
	/// �ܰ��� �÷� ���
	MCOLOR GetBorderColor(void);
	/// �ܰ��� ��Ÿ�� ����
	void SetBorderStyle(MBorderStyle style);
	/// �ܰ��� ��Ÿ�� ���
	MBorderStyle GetBorderStyle();


#define MINT_PANEL	"Panel"
	virtual const char* GetClassName(void){ return MINT_PANEL; }
};


#endif