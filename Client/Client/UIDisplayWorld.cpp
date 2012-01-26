#include "UIDisplayWorld.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CUIDisplayWorld::CUIDisplayWorld()
{
	m_Camera.setRadius(10.0f);
	m_Camera.setMinRadius(5.0f);
	m_Camera.setMaxRadius(15.0f);
	m_Camera.setYawAngle(PI / 4);
	m_Camera.setPitchAngle( - PI / 4);
	m_Camera.setMinPitchAngle(-PI / 3);
	m_Camera.setMaxPitchAngle(-PI / 16);
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
		CRect<int> rcViewport	= getViewport();
		// ----
		m_Camera.setProjParams(PI / 3, rcViewport.getWidth(), rcViewport.getHeight(), 0.1f, CWorld::getInstance().getFog().fEnd);
		m_Camera.FrameMove(fElapsedTime);
		// ----
		Vec3D vPos = CPlayerMe::getInstance().getPos();
		vPos.y+=CPlayerMe::getInstance().getRoleHeight()*0.6f;
		m_Camera.setTargetPos(vPos);
		// ----
		CUIDisplay::OnFrameMove(fTime, fElapsedTime);
		// ----
		CWorld::getInstance().frameMove(Matrix::UNIT, fTime, fElapsedTime);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIDisplayWorld::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	if(IsVisible() == true)
	{
		bool bBloom				= true;
		bool bCamma				= false;
		bool bSetFog			= false;
		// ----
		CRect<int> rcViewport	= getViewport();
		CRenderSystem & R		= CRenderSystem::getSingleton();
		// ----
		R.setShaderFloat("g_fTime",			fTime);
		R.setShaderMatrix("g_mViewProj",	m_Camera.getProjXView());
		R.setShaderVec3D("g_vLightDir",		CWorld::getInstance().getLight().vDirection);
		R.setShaderVec3D("g_vEyePot",		m_Camera.getEyePt());
		R.setShaderVec3D("g_vPointLight",	CPlayerMe::getInstance().getPos()+Vec3D(0.0f,2.0f,0.0f));
		
		// ----
		R.setWorldMatrix(Matrix::UNIT);
		// ----
		R.SetSamplerFilter(0, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
		R.SetSamplerFilter(1, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
		R.SetSamplerFilter(2, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
		// ----
		R.setProjectionMatrix(m_Camera.getProjMatrix());
		R.setViewMatrix(m_Camera.getViewMatrix());
		// ----
		if((bBloom == true) || (bCamma == true))
		{
			m_SceneEffect.renderTargetBegin();
			// ----
			CRect<int> rcRenderTarget(0,0,rcViewport.getWidth(),rcViewport.getHeight());
			// ----
			R.setViewport(rcRenderTarget);
		}
		else
		{
			R.setViewport(rcViewport);
		}
		// ----
		if(bSetFog == true)
		{
			// # Set Fog
			// ----
			//R.ClearBuffer(true, true, GetFog().dwColor);
			// ----
			//GetRenderSystem().setFog(16.0f, 32.0f, 0.04f, 0xFF000044);
			//GetRenderSystem().setFogEnable(false);
		}
		// ----
		{
			// # Render World
			// ----
			R.setProjectionMatrix(m_Camera.getProjMatrix());
			R.setViewMatrix(m_Camera.getViewMatrix());
			R.setWorldMatrix(Matrix::UNIT);
			// ----
			CWorld::getInstance().updateRender(m_Camera.getFrustum());
			CWorld::getInstance().render(Matrix::UNIT,MATERIAL_NORMAL);
		}
		// ----
		R.SetCullingMode(CULL_NONE);
		R.setWorldMatrix(Matrix::UNIT);
		R.ClearBuffer(true, false, 0x0);
		// ----
		if((bBloom == true) || (bCamma == true))
		{
			R.SetSamplerFilter(0, TEXF_LINEAR, TEXF_POINT, TEXF_LINEAR);
			R.SetSamplerFilter(1, TEXF_LINEAR, TEXF_POINT, TEXF_LINEAR);
			R.SetSamplerFilter(2, TEXF_LINEAR, TEXF_POINT, TEXF_LINEAR);
			//m_SceneEffect.RenderTemporalBloom();
			//m_SceneEffect.RenderBloom();
			// ----
			if(bCamma == true)
			{
				m_SceneEffect.renderGammaCorrection();
			}
			// ----
			if(bBloom == true)
			{
				m_SceneEffect.renderTargetBloom();
			}
			m_SceneEffect.renderTargetEnd();
			m_SceneEffect.compose(rcViewport);
			// ----
			R.setViewport(rcViewport);
		}
		// ----
		R.setViewport(GetParentDialog()->GetBoundingBox());
		// ----
		CUIDisplay::OnFrameRender(mTransform,fTime,fElapsedTime);
	}
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
	Vec3D vPos = m_Camera.getTargetPos();
	// ----
	// # 基于摄像机的 yaw 创建旋转矩阵 (Warning ! huihui need translate)
	// ----
	mCameraRot.rotationYawPitchRoll(m_Camera.getYawAngle(), 0, 0);
	// ----
	vPos	+= mCameraRot * Vec3D(x, 0, y) * 0.001f * m_Camera.getRadius();
	// ----
	vPos.y	= CWorld::getInstance().getHeight(vPos.x, vPos.z);
	// ----
	m_Camera.setTargetPos(vPos);
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
	m_Camera.GetPickRay(vRayPos, vRayDir, point.x, point.y, m_rcBoundingBox.getRECT());
	// ----
	CWorld::getInstance().pick(vRayPos, vRayDir, & vTargetPos);
	CWorld::getInstance().setTargetPos(vTargetPos);
	//CWorld::getInstance().getTerrain().GetBrushDecal().SetPos(vTargetPos.x, vTargetPos.z);
	// ----
	if(IsPressed() == true)
	{
		if(GetKeyState(VK_RBUTTON) < 0)
		{
			m_Camera.addMouseDelta(Vec3D(m_ptLastMousePosition.x - point.x, m_ptLastMousePosition.y - point.y, 0));
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
	CRole* pRole	=	CWorld::getInstance().pickRole(vRayPos, vRayDir);
	// ----
	CWorld::getInstance().getFocusNodes().removeChildren();
	if (pRole)
	{
		CWorld::getInstance().getFocusNodes().addChild(pRole);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIDisplayWorld::OnMouseWheel(POINT point, short wheelDelta)
{
	if(ContainsPoint(point) == true)
	{
		if(wheelDelta != 0)
		{
			m_Camera.addMouseDelta(Vec3D(0, 0, - wheelDelta / 12.0f * m_Camera.getRadius()));
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
		CRole* pRole = CWorld::getInstance().getFocusRole();
		// ----
		if(pRole == NULL)
		{
			Vec3D vRayPos;
			Vec3D vRayDir;
			Vec3D vTargetPos;
			// ---
			m_Camera.GetPickRay(vRayPos, vRayDir, point.x, point.y, m_rcBoundingBox.getRECT());
			// ---
			CWorld::getInstance().pick(vRayPos, vRayDir, & vTargetPos);
			// ---
			CPlayerMe::getInstance().walk(vTargetPos.x, vTargetPos.z);
		}
		else
		{
			if(GetKeyState(VK_CONTROL) < 0)
			{
				// ----
			}
			else
			{
				CPlayerMe::getInstance().setAttackTarget(pRole->getID());
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