#ifndef MTOOLTIP_H
#define MTOOLTIP_H

#include "MWidget.h"

class MToolTip : public MWidget{
protected:
	bool	m_bUseParentName;

public:
	// szName�� NULL�̸� Parent�� m_szName�� ǥ���Ѵ�.
	MToolTip(const char* szName, MWidget* pParent);
	virtual ~MToolTip(void);

	// ToolTip�� String�� ũ�⿡ ���� Widgetũ�� �ڵ� ����
	virtual void SetBounds(void);

	// �ؽ�Ʈ �Է�, SetBounds()����ȴ�.
	void SetText(const char* szText);

	bool IsUseParentName(void);

	virtual void OnDraw(MDrawContext* pDC);

#define MINT_TOOLTIP	"ToolTip"
	virtual const char* GetClassName(void){ return MINT_TOOLTIP; }
};

#endif
