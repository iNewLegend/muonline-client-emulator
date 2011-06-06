#include "UICoordDisplay.h"
#include "UIDialog.h"
#include "Camera.h"
#include "RenderSystem.h"

CUICoordDisplay::CUICoordDisplay()
{
}

void CUICoordDisplay::SetViewDir(Vec3D& vViewDir)
{
	//m_Coord.FrameMove( vViewDir );
}

void CUICoordDisplay::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	CRenderSystem& R = GetRenderSystem();

	//RECT rcScreen = m_rcBoundingBox;
	//m_Coord.SetPos(rcScreen.left, rcScreen.top, rcScreen.bottom-rcScreen.top);
	
	//m_Coord.Render();
	//R.SetupRenderState();
	//m_Coord.RenderText();
}