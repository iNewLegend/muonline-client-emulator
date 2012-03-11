#include "DlgWorld.h"
#include "DlgShortcuts.h"
#include "DlgChatMessage.h"
#include "DlgChat.h"
#include "DlgMap.h"
#include "DlgTarget.h"
#include "World.h"
#include "PlayerMe.h"
#include "GameCamera.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgWorld::OnControlRegister()
{
	CUIDialog::OnControlRegister();
	// ----

	CDlgShortcuts::getInstance().Create("IDD_SHORTCUTS",this);
	CDlgChatMessage::getInstance().Create("IDD_CHAT_MESSAGE",this);
	//CDlgChatMessage::getInstance().SetVisible(false);

	CDlgChat::getInstance().Create("IDD_CHAT",this);
	CDlgChat::getInstance().SetVisible(false);
// 
// -- Equip
// dlgEquip = CUIGeneralDialog()
// dlgEquip:create("IDD_EQUIP",IDD_MAIN)
// dlgEquip:SetVisible(false)

	// Move Map List
	CDlgMap::getInstance().Create("IDD_MOVE",this);
	CDlgMap::getInstance().SetVisible(false);
// 
// -- Bag
// dlgBag = CUIGeneralDialog()
// dlgBag:create("IDD_BAG",IDD_MAIN)
// dlgBag:SetVisible(false)
// 
// -- Player Head
// dlgPlayerHead = CUIDialog()
// dlgPlayerHead:create("IDD_PLAYERHEAD",IDD_MAIN)
// dlgPlayerHead:SetVisible(false)

// -- Min Map
// dlgMinmap = CUIDialog()
// dlgMinmap:create("IDD_MINMAP",IDD_MAIN)
// dlgMinmap:SetVisible(false)

// -- Act
// dlgAct = CUIDialog()
// dlgAct:create("IDD_ACT",IDD_MAIN)
// dlgAct:SetVisible(false)

// -- Setup
// dlgSetup = CUIGeneralDialog()
// dlgSetup:create("IDD_SETUP",IDD_MAIN)
// dlgSetup:SetVisible(false)

	CDlgTarget::getInstance().Create("IDD_TARGET",this);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// void CDlgWorld::MoveCamera(int x, int y)
// {
// 	Matrix mCameraRot;
// 	// ----
// 	Vec3D vPos = CGameCamera::getInstance().getTargetPos();
// 	// ----
// 	// # 基于摄像机的 yaw 创建旋转矩阵 (Warning ! huihui need translate)
// 	// ----
// 	mCameraRot.rotationYawPitchRoll(CGameCamera::getInstance().getYawAngle(), 0, 0);
// 	// ----
// 	vPos	+= mCameraRot * Vec3D(x, 0, y) * 0.001f * CGameCamera::getInstance().getRadius();
// 	// ----
// 	vPos.y	= CWorld::getInstance().getHeight(vPos.x, vPos.z);
// 	// ----
// 	CGameCamera::getInstance().setTargetPos(vPos);
// }
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// void CDlgWorld::OnMouseMove(POINT point)
// {
// 	Vec3D vRayPos;
// 	Vec3D vRayDir;
// 	Vec3D vTargetPos;
// 	// ----
// 	CGameCamera::getInstance().GetPickRay(vRayPos, vRayDir, point.x, point.y, m_rcBoundingBox.getRECT());
// 	// ----
// 	CWorld::getInstance().pick(vRayPos, vRayDir, & vTargetPos);
// 	CWorld::getInstance().setTargetPos(vTargetPos);
// 	//CWorld::getInstance().getTerrain().GetBrushDecal().SetPos(vTargetPos.x, vTargetPos.z);
// 	// ----
// 	if(IsPressed() == true)
// 	{
// 		if(GetKeyState(VK_RBUTTON) < 0)
// 		{
// 			CGameCamera::getInstance().addMouseDelta(Vec3D(m_ptLastMousePosition.x - point.x, m_ptLastMousePosition.y - point.y, 0));
// 			// ----
// 			m_ptLastMousePosition	=	point;
// 		}
// 		// ----
// 		if(GetKeyState(VK_MBUTTON) < 0)
// 		{
// 			MoveCamera( - point.x + m_ptLastMousePosition.x, point.y - m_ptLastMousePosition.y);
// 			// ----
// 			m_ptLastMousePosition	=	point;
// 		}
// 	}
// 	m_pMouseRole = CWorld::getInstance().pickRole(vRayPos, vRayDir);
// 	// ----
// 	std::for_each(CWorld::getInstance().getRenderNodes().begin(), CWorld::getInstance().getRenderNodes().end(), [](iRenderNode* p) { p->setFocus(0); });
// 	// ----
// 	if (m_pMouseRole)
// 	{
// 		m_pMouseRole->setFocus(1);
// 		m_pRenderNodeProps = NULL;
// 		CDlgTarget::getInstance().SetVisible(true);
// 		CDlgTarget::getInstance().setTargetName(m_pMouseRole->getRoleName());
// 	}
// 	else
// 	{
// 		CDlgTarget::getInstance().SetVisible(false);
// 		m_pRenderNodeProps = CWorld::getInstance().pickProps(vRayPos, vRayDir);
// 		if (m_pRenderNodeProps)
// 		{
// 			m_pRenderNodeProps->setFocus(2);
// 			//CDlgTarget::getInstance().SetVisible(true);
// 			//CDlgTarget::getInstance().setTargetName(m_pRenderNodeProps->getName());
// 		}
// 		else
// 		{
// 			CDlgTarget::getInstance().SetVisible(false);
// 		}
// 	}
// }
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// void CDlgWorld::OnMouseWheel(POINT point, short wheelDelta)
// {
// 	if(ContainsPoint(point) == true)
// 	{
// 		if(wheelDelta != 0)
// 		{
// 			CGameCamera::getInstance().addMouseDelta(Vec3D(0, 0, - wheelDelta / 12.0f * CGameCamera::getInstance().getRadius()));
// 		}
// 	}
// }
// //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// void CDlgWorld::OnLButtonDown(POINT point)
// {
// 	if(ContainsPoint(point) == true)
// 	{
// 		CPlayerMe::getInstance().setAttackTarget(0);
// 		// ----
// 		if(m_pMouseRole)
// 		{
// 			if(GetKeyState(VK_CONTROL) < 0)
// 			{
// 				// ----
// 			}
// 			else
// 			{
// 				CPlayerMe::getInstance().setAttackTarget(m_pMouseRole->getRoleID());
// 			}
// 		}
// 		else if (m_pRenderNodeProps)
// 		{
// 			const Vec3D& vPos = m_pRenderNodeProps->getPos();
// 			const Vec3D& vRotate = m_pRenderNodeProps->getRotate();
// 			// ---
// 			RoleCmd cmd;
// 			cmd.nType	= RoleCmd::MOVE;
// 			cmd.nParam1	= vPos.x;
// 			cmd.nParam2	= vPos.z;
// 			cmd.nParam3	= 0;
// 			// ---
// 			CPlayerMe::getInstance().addRoleCmd(cmd);
// 			// ----
// 			// If it is a circular seats, it need to rotate the direction.
			// ----
// 			if (m_pRenderNodeProps->getID()==133||
// 				m_pRenderNodeProps->getID()==145)
// 			{
// 				cmd.nType	= RoleCmd::DIR;
// 				cmd.nParam1	= vRotate.y*4.0f/PI;
// 				CPlayerMe::getInstance().addRoleCmd(cmd);
// 			}
// 			// ---
// 			// Do the pose.
// 			// ---
// 			cmd.nType	= RoleCmd::POSE;
// 			if (m_pRenderNodeProps->getID()==133)
// 			{
// 				cmd.nParam1	= CRole::RELY;
// 			}
// 			else
// 			{
// 				cmd.nParam1	= CRole::SIT;
// 			}
// 			CPlayerMe::getInstance().addRoleCmd(cmd);
// 		}
// 		else
// 		{
// 			Vec3D vRayPos;
// 			Vec3D vRayDir;
// 			Vec3D vTargetPos;
// 			// ---
// 			CGameCamera::getInstance().GetPickRay(vRayPos, vRayDir, point.x, point.y, m_rcBoundingBox.getRECT());
// 			// ---
// 			CWorld::getInstance().pick(vRayPos, vRayDir, & vTargetPos);
// 			// ---
// 			CPlayerMe::getInstance().clearRoleCmd();
// 			// ---
// 			RoleCmd cmd;
// 			cmd.nType	= RoleCmd::MOVE;
// 			cmd.nParam1	= vTargetPos.x;
// 			cmd.nParam2	= vTargetPos.z;
// 			cmd.nParam3	= 0;
// 			CPlayerMe::getInstance().addRoleCmd(cmd);
// 			// ---
// 			if (CPlayerMe::getInstance().getCurRoleCmd().nType==RoleCmd::MOVE)
// 			{
// 				CPlayerMe::getInstance().nextRoleCmd();
// 			}
// 		}
// 		// ----
// 		/* # Warning ! */ return;
// 		// ----
// 		SetPressed(true);
// 		// ----
// 		if(CWorld::getInstance().getFocusNodes().getChildObj().size() > 0)
// 		{
// 			m_ptLastMousePosition	=	point;
// 		}
// 	}
// }
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// void CDlgWorld::OnRButtonDown(POINT point)
// {
// 	if(ContainsPoint(point) == true)
// 	{
// 		SetPressed(true);
// 		// ----
// 		m_ptLastMousePosition	=	point;
// 	}
// }
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------