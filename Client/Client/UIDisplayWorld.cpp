#include "UIDisplayWorld.h"
#include "DlgTarget.h"
#include "GameCamera.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CUIDisplayWorld::CUIDisplayWorld()
	:m_pMouseRole(NULL)
	,m_pRenderNodeProps(NULL)
{
	CGameCamera::getInstance().setRadius(10.0f);
	CGameCamera::getInstance().setMinRadius(5.0f);
	CGameCamera::getInstance().setMaxRadius(15.0f);
	CGameCamera::getInstance().setYawAngle(PI / 4);
	CGameCamera::getInstance().setPitchAngle( - PI / 4);
	CGameCamera::getInstance().setMinPitchAngle(-PI / 3);
	CGameCamera::getInstance().setMaxPitchAngle(-PI / 16);
	m_pRenderNode = &CWorld::getInstance();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CUIDisplayWorld::~CUIDisplayWorld()
{
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIDisplayWorld::OnFrameMove(double fTime, float fElapsedTime)
{
	if(IsVisible() == true)
	{
		CWorld::getInstance().frameMoveRole(Matrix::UNIT, fTime, fElapsedTime);
		// ----
		CWorld::getInstance().frameMove(Matrix::UNIT, fTime, fElapsedTime);
		// ----
		CRect<int> rcViewport	= getViewport();
		// ----
		CGameCamera::getInstance().setProjParams(PI / 3, rcViewport.getWidth(), rcViewport.getHeight(), 0.1f, CWorld::getInstance().getFog().fEnd);
		// ----
// 		Vec3D vPos = CPlayerMe::getInstance().getPos();
// 		vPos.y+=CPlayerMe::getInstance().getRoleHeight()*0.6f;
// 		CGameCamera::getInstance().setTargetPos(vPos);
		// ----
		CGameCamera::getInstance().FrameMove(fElapsedTime);
		// ----
		CUIDisplay::OnFrameMove(fTime, fElapsedTime);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Matrix calcLightMatrix(const BBox& bbox, const Vec3D& vLightDir)
{
	float fLength = (bbox.vMax-bbox.vMin).length();
	Vec3D vLookAt=(bbox.vMax+bbox.vMin)*0.5f;
	Vec3D vEyePt = vLookAt - vLightDir*fLength*0.5f;

	Vec3D vUp(0,1,0); 
	Matrix mView, mProj;
	mView.MatrixLookAtLH(vEyePt,vLookAt,vUp);
	mProj.MatrixOrthoLH(fLength,fLength, 0, fLength);
	Matrix mLight = mProj*mView;
	return mLight;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIDisplayWorld::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	// ----
	Vec3D vPos = CPlayerMe::getInstance().getPos();
	vPos.y+=CPlayerMe::getInstance().getRoleHeight()*0.6f;
	CGameCamera::getInstance().setTargetPos(vPos);
	// ----
	CGameCamera::getInstance().FrameMove(fElapsedTime);
	m_mProj = CGameCamera::getInstance().getProjMatrix();
	m_mView = CGameCamera::getInstance().getViewMatrix();
	CWorld::getInstance().updateRender(CGameCamera::getInstance().getFrustum());
	CUIDisplayRenderNode::OnFrameRender(mTransform,fTime,fElapsedTime);
	CWorld::getInstance().renderDamageInfo();
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CUIDisplayWorld::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool bReturn = true;
	// ----
	if((m_bEnabled == true) || (m_bVisible == true))
	{
		switch(uMsg)
		{
			case WM_KEYDOWN:
			{
				switch(wParam)
				{
					case VK_SHIFT:
					{
						
					}
					break;

					case VK_DELETE:
					{
						CWorld::getInstance().delChildByFocus();
					}
					break;

					case VK_SUBTRACT:
					{
					}
					break;

					case VK_ADD:
					{
					}
					break;

					case 'Z':
					{
						if(GetKeyState(VK_CONTROL) < 0)
						{

						}
					}
					break;

					case 'Y':
					{
						if(GetKeyState(VK_CONTROL) < 0)
						{

						}
					}
					break;

					case VK_F9:
						{
							m_SceneEffect.setDebug(0);
						}
						break;

					case VK_F10:
						{
							m_SceneEffect.setDebug(1);
						}
						break;

					case VK_F11:
						{
							m_SceneEffect.setDebug(2);
						}
						break;
					default:
					{
						bReturn = false;
					}
					break;
				}
			}
			break;

			case WM_KEYUP:
			{
				switch(wParam)
				{
					case VK_SHIFT:
					{
					}
					break;
					
					default:
					{
						bReturn = false;
					}
					break;
				}
			}
			break;
			
			default:
			{
				bReturn = false;
			}
			break;
		}	
	}
	else
	{
		bReturn = false;
	}
	// ----
	return bReturn;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// # Warning ! Need optimize here (maybe move varibales to class)
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIDisplayWorld::MoveCamera(int x, int y)
{
	Matrix mCameraRot;
	// ----
	Vec3D vPos = CGameCamera::getInstance().getTargetPos();
	// ----
	// # 基于摄像机的 yaw 创建旋转矩阵 (Warning ! huihui need translate)
	// ----
	mCameraRot.rotationYawPitchRoll(CGameCamera::getInstance().getYawAngle(), 0, 0);
	// ----
	vPos	+= mCameraRot * Vec3D(x, 0, y) * 0.001f * CGameCamera::getInstance().getRadius();
	// ----
	vPos.y	= CWorld::getInstance().getHeight(vPos.x, vPos.z);
	// ----
	CGameCamera::getInstance().setTargetPos(vPos);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// # Warning ! Need optimize here (maybe move varibales to class)
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIDisplayWorld::OnMouseMove(POINT point)
{
	Vec3D vRayPos;
	Vec3D vRayDir;
	Vec3D vTargetPos;
	// ----
	CGameCamera::getInstance().GetPickRay(vRayPos, vRayDir, point.x, point.y, m_rcBoundingBox.getRECT());
	// ----
	CWorld::getInstance().pick(vRayPos, vRayDir, & vTargetPos);
	CWorld::getInstance().setTargetPos(vTargetPos);
	//CWorld::getInstance().getTerrain().GetBrushDecal().SetPos(vTargetPos.x, vTargetPos.z);
	// ----
	if(IsPressed() == true)
	{
		if(GetKeyState(VK_RBUTTON) < 0)
		{
			CGameCamera::getInstance().addMouseDelta(Vec3D(m_ptLastMousePosition.x - point.x, m_ptLastMousePosition.y - point.y, 0));
			// ----
			m_ptLastMousePosition	=	point;
		}
		// ----
		if(GetKeyState(VK_MBUTTON) < 0)
		{
			MoveCamera( - point.x + m_ptLastMousePosition.x, point.y - m_ptLastMousePosition.y);
			// ----
			m_ptLastMousePosition	=	point;
		}
	}
	m_pMouseRole = CWorld::getInstance().pickRole(vRayPos, vRayDir);
	// ----
	std::for_each(CWorld::getInstance().getRenderNodes().begin(), CWorld::getInstance().getRenderNodes().end(), [](iRenderNode* p) { p->setFocus(0); });
	// ----
	if (m_pMouseRole)
	{
		m_pMouseRole->setFocus(1);
		m_pRenderNodeProps = NULL;
		CDlgTarget::getInstance().SetVisible(true);
		CDlgTarget::getInstance().setTargetName(m_pMouseRole->getRoleName());
	}
	else
	{
		CDlgTarget::getInstance().SetVisible(false);
		m_pRenderNodeProps = CWorld::getInstance().pickProps(vRayPos, vRayDir);
		if (m_pRenderNodeProps)
		{
			m_pRenderNodeProps->setFocus(2);
			//CDlgTarget::getInstance().SetVisible(true);
			//CDlgTarget::getInstance().setTargetName(m_pRenderNodeProps->getName());
		}
		else
		{
			CDlgTarget::getInstance().SetVisible(false);
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIDisplayWorld::OnMouseWheel(POINT point, short wheelDelta)
{
	if(ContainsPoint(point) == true)
	{
		if(wheelDelta != 0)
		{
			CGameCamera::getInstance().addMouseDelta(Vec3D(0, 0, - wheelDelta / 12.0f * CGameCamera::getInstance().getRadius()));
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// # Warning ! Need optimize here (maybe move varibales to class)
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void CUIDisplayWorld::OnLButtonDown(POINT point)
{
	if(ContainsPoint(point) == true)
	{
		CPlayerMe::getInstance().setAttackTarget(0);
		// ----
		if(m_pMouseRole)
		{
			if(GetKeyState(VK_CONTROL) < 0)
			{
				// ----
			}
			else
			{
				CPlayerMe::getInstance().setAttackTarget(m_pMouseRole->getRoleID());
			}
		}
		else if (m_pRenderNodeProps)
		{
			const Vec3D& vPos = m_pRenderNodeProps->getPos();
			const Vec3D& vRotate = m_pRenderNodeProps->getRotate();
			// ---
			RoleCmd cmd;
			cmd.nType	= RoleCmd::MOVE;
			cmd.nParam1	= vPos.x;
			cmd.nParam2	= vPos.z;
			cmd.nParam3	= 0;
			// ---
			CPlayerMe::getInstance().addRoleCmd(cmd);
			// ----
			// If it is a circular seats, it need to rotate the direction.
			// ----
			if (m_pRenderNodeProps->getID()==133||
				m_pRenderNodeProps->getID()==145)
			{
				cmd.nType	= RoleCmd::DIR;
				cmd.nParam1	= vRotate.y*4.0f/PI;
				CPlayerMe::getInstance().addRoleCmd(cmd);
			}
			// ---
			// Do the pose.
			// ---
			cmd.nType	= RoleCmd::POSE;
			if (m_pRenderNodeProps->getID()==133)
			{
				cmd.nParam1	= CRole::RELY;
			}
			else
			{
				cmd.nParam1	= CRole::SIT;
			}
			CPlayerMe::getInstance().addRoleCmd(cmd);
		}
		else
		{
			Vec3D vRayPos;
			Vec3D vRayDir;
			Vec3D vTargetPos;
			// ---
			CGameCamera::getInstance().GetPickRay(vRayPos, vRayDir, point.x, point.y, m_rcBoundingBox.getRECT());
			// ---
			CWorld::getInstance().pick(vRayPos, vRayDir, & vTargetPos);
			// ---
			CPlayerMe::getInstance().clearRoleCmd();
			// ---
			RoleCmd cmd;
			cmd.nType	= RoleCmd::MOVE;
			cmd.nParam1	= vTargetPos.x;
			cmd.nParam2	= vTargetPos.z;
			cmd.nParam3	= 0;
			CPlayerMe::getInstance().addRoleCmd(cmd);
			// ---
			if (CPlayerMe::getInstance().getCurRoleCmd().nType==RoleCmd::MOVE)
			{
				CPlayerMe::getInstance().nextRoleCmd();
			}
		}
		// ----
		/* # Warning ! */ return;
		// ----
		SetPressed(true);
		// ----
		if(CWorld::getInstance().getFocusNodes().getChildObj().size() > 0)
		{
			m_ptLastMousePosition	=	point;
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIDisplayWorld::OnLButtonUp(POINT point)
{
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIDisplayWorld::OnRButtonDown(POINT point)
{
	if(ContainsPoint(point) == true)
	{
		SetPressed(true);
		// ----
		m_ptLastMousePosition	=	point;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIDisplayWorld::OnRButtonUp(POINT point)
{
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIDisplayWorld::OnMButtonDown(POINT point)
{
	if(ContainsPoint(point) == true)
	{
		SetPressed(true);
		// ----
		m_ptLastMousePosition	=	point;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIDisplayWorld::OnMButtonUp(POINT point)
{
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIDisplayWorld::OnFocusOut()
{
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CUIDisplayWorld::HandleMouse(UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam)
{
	if(WM_CAPTURECHANGED == uMsg)
	{
		if((HWND)lParam != UIGetHWND())
		{
			if(IsPressed() == true)
			{
				ReleaseCapture();
			}
		}
	}
	// ----
	return true;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIDisplayWorld::OnSize(const CRect<int>& rc)
{
	CUIDisplay::OnSize(rc);
	// ----
	m_SceneEffect.Reset(GetBoundingBox());
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------