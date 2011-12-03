#ifndef MCOMBOLISTBOX_H
#define MCOMBOLISTBOX_H

#include "MListBox.h"

// ListBox ������ �ƴѰ��� Ŭ���ϸ� Hide�Ǵ� ComboBox�� ListBox

class MComboListBox : public MListBox{
	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);
public:
	MComboListBox(MWidget* pParent=NULL, MListener* pListener=NULL)
		: MListBox(pParent, pListener) {}
	virtual ~MComboListBox(void){}

#define MINT_COMBOLISTBOX	"ComboListBox"
	virtual const char* GetClassName(void){ return MINT_COMBOLISTBOX; }
};

#endif