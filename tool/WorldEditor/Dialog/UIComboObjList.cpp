#include "UIComboObjList.h"
#include "Scene.h"
#include "DlgToolbar.h"

CUIComboObjList::CUIComboObjList():
m_pScene(0)
{
}

CUIComboObjList::~CUIComboObjList()
{	
}

void CUIComboObjList::OnControlRegister()
{
	CUICombo::OnControlRegister();

	RegisterControl("IDC_LIST_BOX", m_ListBoxObject);
	RegisterControl("IDC_LIST_BOX_DISPLAY", m_ListBoxDisplay);
	RegisterControlEvent("IDC_LIST_BOX",	(PEVENT)&CUIComboObjList::OnListBoxObjectSelection, EVENT_LISTBOX_SELECTION);
	RegisterControlEvent("IDC_BTN_RELOAD",	(PEVENT)&CUIComboObjList::OnObjectListReload);
	RegisterControlEvent("IDC_BTN_EDIT",	(PEVENT)&CUIComboObjList::OnObjectListEdit);
}

//void CUIComboObjList::initObject()
//{
//	m_ListBoxObject.RemoveAllItems();
//	CScene::MAP_OBJECT_INFO& m_SceneObjectInfo = getSceneMgr().GetObjectInfo();
//	for (CScene::MAP_OBJECT_INFO::iterator it = m_SceneObjectInfo.begin();
//		it!=m_SceneObjectInfo.end(); it++)
//	{
//		m_ListBoxObject.AddItem(s2ws(it->second.strName).c_str(),(LPVOID)it->first);
//	}
//}

//void CUIComboObjList::OnFocusObjectChanged()
//{
//		int nIndex = m_ListBoxObject.getItemIndexByData((void*)(pObject->getObjectID()));
//		m_ListBoxObject.SelectItem(nIndex);
//}