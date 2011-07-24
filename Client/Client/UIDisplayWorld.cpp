#include "UIDisplayWorld.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CUIDisplayWorld::CUIDisplayWorld()
{
	m_Camera.setYawAngle(PI / 4);
	m_Camera.setPitchAngle( - PI / 4);
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
		m_Camera.SetProjParams(PI / 3, rcViewport.getWidth(), rcViewport.getHeight(), 0.1f, CWorld::getInstance().getFog().fEnd);
		m_Camera.FrameMove(fElapsedTime);
		m_Camera.setTargetPos(CPlayerMe::getInstance().getPos());
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
		bool bBloom				= false;
		bool bCamma				= false;
		bool bSkyBox			= false;
		bool bSetFog			= false;
		// ----
		CRect<int> rcViewport	= getViewport();
		CRenderSystem & R		= GetRenderSystem();
		CShader* pShader		= R.GetShaderMgr().getSharedShader();
		// ----
		if (pShader)
		{
			pShader->setFloat("g_fTime", fTime);
			pShader->setMatrix("g_mViewProj", m_Camera.GetProjXView());
			pShader->setMatrix("g_mView", m_Camera.getViewMatrix());
			pShader->setVec3D("g_vLightDir", CWorld::getInstance().getLight().vDirection);
			pShader->setVec3D("g_vEyePot", m_Camera.getEyePt());
		}
		// ----
		R.SetupRenderState();
		R.setWorldMatrix(Matrix::UNIT);
		// ----
		R.SetSamplerFilter(0, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
		R.SetSamplerFilter(1, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
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
		if(bSkyBox == true)
		{
			// # Render SkyBox
			// ----
			//m_SkyBox.Render(m_Camera.GetViewMatrix());
			// ----
			//pShader->setTexture("g_texEnvironment", m_SkyBox.m_pCubeMap);*/
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
			CWorld::getInstance().UpdateRender(m_Camera.GetFrustum());
			CWorld::getInstance().render(Matrix::UNIT,MATERIAL_NORMAL);
		}
		// ----
		R.SetCullingMode(CULL_NONE);
		R.setWorldMatrix(Matrix::UNIT);
		R.ClearBuffer(true, false, 0x0);
		// ----
		if((bBloom == true) || (bCamma == true))
		{
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
		if(true == true)
		{
			m_SceneEffect.glowRenderTargetBegin();
			// ----
			R.ClearBuffer(false, true, 0x00000000);
			// ----
			//CWorld::getInstance().renderGlow();
			// ----
			m_SceneEffect.glowRenderTargetEnd();
			m_SceneEffect.RenderBloom();
		}
		// ----
		if(true == true)
		{
			m_SceneEffect.renderTargetBegin();
			// ----
			R.ClearBuffer(false, true, 0x00000000);
			// ----
			//m_SceneEffect.renderTargetViewportBegin();
			// ----
			{
				R.setProjectionMatrix(m_Camera.getProjMatrix());
				R.setViewMatrix(m_Camera.getViewMatrix());
				// ----
				//CWorld::getInstance().renderGlow();
			}
			// ----
			//m_SceneEffect.renderTargetViewportEnd();
			//m_SceneEffect.renderTargetGlow();
			m_SceneEffect.renderTargetBloom();
			m_SceneEffect.renderTargetEnd();
			//m_SceneEffect.compose();
		}
		// ----
		R.SetupRenderState();
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
	vPos	+= mCameraRot * Vec3D(x, 0, y) * 0.001f * m_Camera.GetRadius();
	// ----
	vPos.y	= CWorld::getInstance().getTerrain().getHeight(vPos.x, vPos.z);
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
	CWorld::getInstance().getTerrain().pick(vRayPos, vRayDir, & vTargetPos);
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
	if (pRole)
	{
		CWorld::getInstance().getFocusNodes().removeChildren();
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
			m_Camera.addMouseDelta(Vec3D(0, 0, - wheelDelta / 12.0f * m_Camera.GetRadius()));
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
		SetFocus();
		// ----
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
			CWorld::getInstance().getTerrain().pick(vRayPos, vRayDir, & vTargetPos);
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