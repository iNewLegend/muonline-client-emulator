//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* DlgWorld.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "UIDialog.h"
#include "TSingleton.h"
#include "Role.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CDlgWorld : public CUIDialog, public TSingleton<CDlgWorld>
{
public:
	void OnControlRegister();
protected:
// 	void MoveCamera(int x, int y);
// 	virtual void OnMouseMove(POINT point);
// 	virtual void OnMouseWheel(POINT point, short wheelDelta);
// 	virtual void OnLButtonDown(POINT point);
// 	virtual void OnRButtonDown(POINT point);
protected:
	POINT			m_ptLastMousePosition;
	iRenderNode*	m_pRenderNodeProps;
	CRole*			m_pMouseRole;
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------