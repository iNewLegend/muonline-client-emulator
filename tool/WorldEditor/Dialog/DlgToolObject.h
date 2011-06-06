#pragma once
#include "UIDialog.h"
#include "UIComboObjListSceneObject.h"

class CDlgToolObject :public CUIDialog
{
public:
	CDlgToolObject();
	~CDlgToolObject();
public:
	virtual void OnControlRegister();
	void initObject();
	void OnNumFloorSnapChanged();
	void OnNumGridSnapChanged();
	void OnFocusObjectValueChanged();
	void OnFocusObjectUpdate();
	void OnAddObject();
	void OnObjectListReload();
private:
	CUIComboObjListSceneObject m_ObjListSceneObject;

	CUIComboNumber	m_NumFloorSnap;
	CUIComboNumber	m_NumGridSnap;
	CUIComboVec3D	m_Vec3DPos;
	CUIComboVec3D	m_Vec3Rotate;
	CUIComboNumber	m_NumScale;
};