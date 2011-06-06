#include "UIImage.h"
#include "UIDialog.h"
#include "tinyxml.h"

CUIImage::CUIImage()
{
	m_Type = UI_CONTROL_IMAGE;

	SetRect(&m_rcTex, 0, 0, -1, -1);
	m_dwColor = 0xFFFFFFFF;
	UIGraph::getInstance().releaseTexture(m_pFilename);
}
void CUIImage::XMLParse(const TiXmlElement* pControlElement)
{
	CUIControl::XMLParse(pControlElement);
	//
	if (pControlElement->Attribute("filename"))
	{
		SetFilename(pControlElement->Attribute("filename"));
	}
}

void CUIImage::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{    
	if(m_bVisible == false)
		return;

	m_Style.Blend(/*m_rcBoundingBox.getRECT(), */GetState(), fElapsedTime);
	//UIGraph::getInstance().DrawSprite(m_rcTex, m_rcBoundingBox, m_nTexID, m_dwColor);
	UIGraph::getInstance().DrawSprite(m_rcBoundingBox.getRECT(), m_pFilename, m_Style.m_mapStyleDrawData[0].color.getColor());
	//GetRenderSystem().SetTextureStageStateDecolor();
	//UIGraph::getInstance().DrawSprite(m_rcBoundingBox, m_rcBoundingBox, m_nTexID, m_Style.m_crSpriteColor[1].c);
	//GetRenderSystem().SetupRenderState();
}

void CUIImage::SetFilename(const char* szFilename)
{
	m_strFilename = szFilename;
	UIGraph::getInstance().releaseTexture(m_pFilename);
	m_pFilename = UIGraph::getInstance().createTexture(szFilename);
}