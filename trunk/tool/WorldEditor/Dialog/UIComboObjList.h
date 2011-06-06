#pragma once
#include "UIDialog.h"

class CScene;
class CUIComboObjList : public CUICombo
{
public:
	CUIComboObjList();
	~CUIComboObjList();
public:
	virtual void OnControlRegister();
	virtual void OnListBoxObjectSelection() = 0;
	//void initObject();
	//void OnFocusObjectChanged();
	virtual void OnObjectListReload() = 0;
	virtual void OnObjectListEdit() = 0;
protected:
	CScene* m_pScene;
	CUIListBox	m_ListBoxObject;
	CUIListBox	m_ListBoxDisplay;
};