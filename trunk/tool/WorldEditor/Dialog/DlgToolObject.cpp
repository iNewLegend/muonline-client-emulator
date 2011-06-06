#include "DlgToolObject.h"
#include "DlgToolbar.h"
#include "..\MainRoot.h"

CDlgToolObject::CDlgToolObject()
{
}

CDlgToolObject::~CDlgToolObject()
{	
}

void CDlgToolObject::OnControlRegister()
{
	CUIDialog::OnControlRegister();


	RegisterControl("IDC_OBJLIST_SCENE_OBJECTS",		m_ObjListSceneObject);

	RegisterControl("IDC_NUM_FLOOR_SNAP",		m_NumFloorSnap);
	RegisterControl("IDC_NUM_GRID_SNAP",		m_NumGridSnap);

	RegisterControl("IDC_VEC3D_POS",		m_Vec3DPos);
	RegisterControl("IDC_VEC3D_ROTATE",		m_Vec3Rotate);
	RegisterControl("IDC_NUM_SCALE",		m_NumScale);

	RegisterControlEvent("IDC_VEC3D_POS",		(PEVENT)&CDlgToolObject::OnFocusObjectValueChanged);
	RegisterControlEvent("IDC_VEC3D_ROTATE",	(PEVENT)&CDlgToolObject::OnFocusObjectValueChanged);
	RegisterControlEvent("IDC_NUM_SCALE",		(PEVENT)&CDlgToolObject::OnFocusObjectValueChanged);

	RegisterControlEvent("IDC_NUM_FLOOR_SNAP",	(PEVENT)&CDlgToolObject::OnNumFloorSnapChanged);
	RegisterControlEvent("IDC_NUM_GRID_SNAP",	(PEVENT)&CDlgToolObject::OnNumGridSnapChanged);

	RegisterEvent("MSG_FOCUS_OBJECT_UPDATE",(PEVENT)&CDlgToolObject::OnFocusObjectUpdate);
	RegisterEvent("MSG_ADD_OBJECT",(PEVENT)&CDlgToolObject::OnAddObject);
}

void CDlgToolObject::initObject()
{
	m_ObjListSceneObject.initObject(WE_SCENE);
}

void CDlgToolObject::OnNumFloorSnapChanged()
{
	CMainRoot::getInstance().getMainDialog().getDisplay().setFloorSnap(m_NumFloorSnap.getFloat());
}

void CDlgToolObject::OnNumGridSnapChanged()
{
	CMainRoot::getInstance().getMainDialog().getDisplay().setGridSnap(m_NumGridSnap.getFloat());
}

void CDlgToolObject::OnFocusObjectValueChanged()
{
	CFocusNode& focusNode  = WE_SCENE.getFocusObjects();
	if (focusNode.getChildObj().size()>0)
	{
		Vec3D vPos=m_Vec3DPos.getVec3D();
		Vec3D vRotate=m_Vec3Rotate.getVec3D()*(PI/180);
		float fScale = m_NumScale.getFloat();
		Vec3D vScale(fScale,fScale,fScale);
		if (fScale<0.1f||fScale>=100.f)
		{
			fScale = 1.0f;
			m_NumScale.setFloat(fScale,0,2);
		}

		focusNode.setCenterPos(vPos);
		focusNode.setCenterRotate(vRotate);
		focusNode.setCenterScale(vScale);
		// ----
		WE_SCENE.updateObjTreeByFocus();
	}
	OnFocusObjectUpdate();
}

void CDlgToolObject::OnFocusObjectUpdate()
{
	CFocusNode& focusNode  = WE_SCENE.getFocusObjects();
	LIST_RENDER_NODE& listRenderNode = focusNode.getChildObj();
	if (listRenderNode.size()>0)
	{
		Vec3D vPos = focusNode.getCenterPos();
		Vec3D vRotate = focusNode.getCenterRotate();
		Vec3D vScale = focusNode.getCenterScale();

		m_Vec3DPos.setVec3D(vPos);
		m_Vec3Rotate.setVec3D(vRotate*180/PI);
		m_NumScale.setFloat(vScale.x,0,2);

		if (listRenderNode.size()==1)
		{
			CMapObj* pObject = (CMapObj*)(*listRenderNode.begin());
			if (pObject->GetObjType()==MAP_3DSIMPLE)
			{
				m_ObjListSceneObject.SelectObjectByObjectID(((C3DMapSceneObj*)pObject)->getObjectID());
			}
		}
	}
}

void CDlgToolObject::OnAddObject()
{
	Vec3D vRotate=m_Vec3Rotate.getVec3D()*PI/180;
	float fScale = m_NumScale.getFloat();
	if (fScale<0.1f||fScale>=100.f)
	{
		fScale = 1.0f;
		m_NumScale.setFloat(fScale,0,2);
	}
	Vec3D vScale=Vec3D(fScale,fScale,fScale);
	CRenderNode * pObject = WE_SCENE.add3DMapSceneObj(m_ObjListSceneObject.getSelectedObjectID(),WE_SCENE.getTargetPos(),vRotate,vScale);
	WE_SCENE.getFocusObjects().addChild(pObject);
	OnFocusObjectUpdate();
}