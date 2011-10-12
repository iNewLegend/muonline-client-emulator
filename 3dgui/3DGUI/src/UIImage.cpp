#include "UIImage.h"
#include "UIDialog.h"
#include "tinyxml.h"

CUIImage::CUIImage()
{
	m_Type = UI_CONTROL_IMAGE;
	SetRect(&m_rcTex, 0, 0, -1, -1);
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
	CUIControl::OnFrameRender(mTransform, fTime, fElapsedTime);
}

void CUIImage::SetFilename(const char* szFilename)
{
	m_strFilename = szFilename;
	if (m_Style.m_StyleSprites.size()>0)
	{
		UIGraph::getInstance().releaseTexture(m_Style.m_StyleSprites[0].m_pTexture);
		m_Style.m_StyleSprites[0].m_pTexture = UIGraph::getInstance().createTexture(szFilename);
	}
}