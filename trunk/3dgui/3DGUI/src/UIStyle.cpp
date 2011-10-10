#include "UIStyle.h"
#include "TextRender.h"
#include "tinyxml.h"
#include "UIGraph.h"

CUIStyleMgr& GetStyleMgr()
{
	static CUIStyleMgr g_UIStyleMgr;
	return g_UIStyleMgr;
}

inline unsigned int StrToTextFormat(const char* szFormat)
{
	unsigned int uFormat = 0;
	char szFormatList[256]={'\0'};
	strcpy(szFormatList,szFormat);
	const char* pszFormat=strtok(szFormatList,"|");
	while(pszFormat)
	{
		if (strcmp("TOP",pszFormat)==0)
		{
			//uFormat |= DTL_TOP;
		}
		else if (strcmp("VCENTER",pszFormat)==0)
		{
			//uFormat |= DTL_VCENTER;
		}
		else if (strcmp("BOTTOM",pszFormat)==0)
		{
			//uFormat |= DTL_BOTTOM;
		}
		if (strcmp("LEFT",pszFormat)==0)
		{
			//uFormat |= DTL_LEFT;
			uFormat=ALIGN_TYPE_LEFT;
		}
		else if (strcmp("UCENTER",pszFormat)==0)
		{
			//uFormat |= DTL_CENTER;
			uFormat=ALIGN_TYPE_CENTER;
		}
		else if (strcmp("RIGHT",pszFormat)==0)
		{
			//uFormat |= DTL_RIGHT;
			uFormat=ALIGN_TYPE_RIGHT;
		}
		pszFormat=strtok(NULL,"|");
	}
	return uFormat;
}


RECT StyleElement::updateRect(RECT rect)
{
	RECT rc;
	int nWidth = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;
	rc.left		= rect.left	+ nWidth	* rcScale.left		+ rcOffset.left;
	rc.right	= rect.left	+ nWidth	* rcScale.right		+ rcOffset.right;
	rc.top		= rect.top	+ nHeight	* rcScale.top		+ rcOffset.top;
	rc.bottom	= rect.top	+ nHeight	* rcScale.bottom	+ rcOffset.bottom;
	return rc;
}

void StyleElement::hide()
{
	uState = CONTROL_STATE_HIDDEN;
	fRate = 1.0f;
	color = setColor[CONTROL_STATE_HIDDEN];
}

void StyleElement::blend(UINT iState,float fElapsedTime)
{
	if (uState!=iState)
	{
		uState = iState;
		fRate = 0.0f;
	}
	if (fRate<1.0f)
	{
		fRate += setBlendRate[iState]*fElapsedTime;
		fRate = min(1.0f,fRate);
		color = interpolate(fRate, color, setColor[iState]);
	}
}

void StyleElement::XMLParse(const TiXmlElement& element)
{
	for (size_t i=0;i< CONTROL_STATE_MAX;++i)
	{
		setColor[i].set(0.0f,0.0f,0.0f,0.0f);
	}
	for (size_t i=0;i< CONTROL_STATE_MAX;++i)
	{
		setBlendRate[i]=0.8f;
	}
	SetRect(&rcOffset,0,0,0,0);
	SetRect(&rcScale,0,0,1,1);
	//
	const TiXmlElement *pElement = element.FirstChildElement("color");
	if (pElement)
	{
		const char* pszText = pElement->GetText();
		if(pszText)
		{
			Color32 c;
			c = pszText;
			for (int i = 0; i < CONTROL_STATE_MAX; ++i)
			{
				setColor[i] = c;
			}
			setColor[CONTROL_STATE_HIDDEN].w = 0.0f;
		}
		for (int i = 0; i < CONTROL_STATE_MAX; ++i)
		{
			pszText =  pElement->Attribute(szControlState[i]);
			if (pszText)
			{
				Color32 c;
				c = pszText;
				setColor[i] = c;
			}
		}
	}
	//
	pElement = element.FirstChildElement("blend");
	if (pElement)
	{
		const char* pszText = pElement->GetText();
		if(pszText)
		{
			float fBlend = (float)atof(pszText);
			for (size_t i=0;i< CONTROL_STATE_MAX;++i)
			{
				setBlendRate[i] = fBlend;
			}
		}
		for (size_t i=0;i< CONTROL_STATE_MAX;++i)
		{
			pszText =  pElement->Attribute(szControlState[i]);
			if (pszText)
			{
				setBlendRate[i] = (float)atof(pszText);
			}
		}
	}
	//
	pElement = element.FirstChildElement("offset");
	if (pElement)
	{
		const char* pszText = pElement->GetText();
		if (!pszText)
		{
			pszText = element.Attribute("offset");
		}
		if(pszText)
		{
			sscanf_s(pszText, "%d,%d,%d,%d", &rcOffset.right, &rcOffset.top, &rcOffset.right, &rcOffset.bottom);
		}

	}
	// 
	pElement = element.FirstChildElement("scale");
	if (pElement)
	{
		const char* pszText = pElement->GetText();
		if (!pszText)
		{
			pszText =  element.Attribute("scale");
		}
		if(pszText)
		{
			sscanf_s(pszText, "%d,%d,%d,%d", &rcScale.right, &rcScale.top, &rcScale.right, &rcScale.bottom);
		}
	}
}

void StyleSprite::draw(const RECT& rc)
{
	RECT rcDest = updateRect(rc);
	//if (m_bDecolor)
	//{
	//	GetRenderSystem().SetTextureStageStateDecolor();
	//}
	switch(m_nSpriteLayoutType)
	{
	case SPRITE_LAYOUT_WRAP:
		UIGraph::getInstance().drawSprite(rcDest,m_pTexture,color.getColor(),&rcDest);
		break;
	case SPRITE_LAYOUT_SIMPLE:
		UIGraph::getInstance().drawSprite(rcDest,m_pTexture,color.getColor(),&m_rcBorder);
		break;
	case SPRITE_LAYOUT_3X3GRID:
		UIGraph::getInstance().drawSprite(rcDest,m_pTexture,color.getColor(),&m_rcBorder,&m_rcCenter);
		break;
	default:
		break;
	}
	//if (m_bDecolor)
	//{
	//	GetRenderSystem().SetupRenderState();
	//}
}

void StyleSprite::XMLParse(const TiXmlElement& element)
{
	//
	StyleElement::XMLParse(element);
	if (element.Attribute("filename"))
	{
		std::string strTexture = GetStyleMgr().getDir()+element.Attribute("filename");
		m_pTexture = UIGraph::getInstance().createTexture(strTexture.c_str());
	}
	if (element.Attribute("rect"))
	{
		const char* strRect = element.Attribute("rect");
		sscanf_s(strRect, "%d,%d,%d,%d", &m_rcBorder.right, &m_rcBorder.top, &m_rcBorder.right, &m_rcBorder.bottom);
		m_rcBorder.right	+= m_rcBorder.left;
		m_rcBorder.bottom	+= m_rcBorder.top;
		if (element.Attribute("center_rect"))
		{
			m_nSpriteLayoutType = SPRITE_LAYOUT_3X3GRID;
			const char* strCenterRect = element.Attribute("center_rect");
			sscanf_s(strCenterRect, "%d,%d,%d,%d", &m_rcCenter.right, &m_rcCenter.top, &m_rcCenter.right, &m_rcCenter.bottom);
			m_rcCenter.left		+= m_rcBorder.left;
			m_rcCenter.top		+= m_rcBorder.top;
			m_rcCenter.right	+= m_rcCenter.left;
			m_rcCenter.bottom	+= m_rcCenter.top;
		}
		else
		{
			m_nSpriteLayoutType = SPRITE_LAYOUT_SIMPLE;
		}
	}
	else
	{
		m_nSpriteLayoutType = SPRITE_LAYOUT_WRAP;
	}

	m_bDecolor = false;
	if (element.Attribute("decolor"))
	{
		if (std::string("true")==element.Attribute("decolor"))
		{
			m_bDecolor = true;
		}
	}
}

void StyleFont::XMLParse(const TiXmlElement& element)
{
	StyleElement::XMLParse(element);
	uFormat = 0;
	if (element.Attribute("format"))
	{
		uFormat = StrToTextFormat(element.Attribute("format"));
	}
}

void CUIStyle::Blend(UINT iState, float fElapsedTime)
{
	if (uState!=iState)
	{
		uState = iState;
		//m_fRate = 0.0f;
		if (CONTROL_STATE_HIDDEN!=iState)
		{
			m_nVisible=true;
		}
	}
	//if (m_nVisible)
	{
// 		if (m_fRate<1.0f)
// 		{
// 			m_fRate += m_setBlendRate[iState]*fElapsedTime;//1.0f - powf(styleData.setBlendRate[iState], 30 * fElapsedTime);
// 			m_fRate = min(1.0f,m_fRate);
// 			m_vRotate			= interpolate(m_fRate, m_vRotate, m_setRotate[iState]);
// 			m_vTranslation	= interpolate(m_fRate, m_vTranslation, m_setTranslation[iState]);
// 		}
// 		else if (CONTROL_STATE_HIDDEN==iState)
// 		{
// 			for (size_t i=0; i<m_StyleSprites.size(); ++i)
// 			{
// 				m_StyleSprites[i].hide();
// 			}
// 			m_FontStyle.hide();
// 			m_nVisible = false;
// 			return;
// 		}
		for (size_t i=0; i<m_StyleSprites.size(); ++i)
		{
			m_StyleSprites[i].blend(iState,fElapsedTime);
		}
		m_FontStyle.blend(iState,fElapsedTime);
	}
}

void CUIStyle::setStyle(const std::string& strName)
{
	m_strName = strName;
	const CUIStyle& style = GetStyleMgr().getStyleData(strName);
	*this = style;
	Blend(CONTROL_STATE_HIDDEN,100);
}

void CUIStyle::draw(const RECT& rc, const wchar_t* wcsText)
{
	// ----
	for (size_t i=0; i<m_StyleSprites.size(); ++i)
	{
		m_StyleSprites[i].draw(rc);
	}
	// ----
	if(m_FontStyle.color.a!=0)
	{
		RECT rcDest = m_FontStyle.updateRect(rc);
		UIGraph::getInstance().drawText(wcsText,-1,rcDest,m_FontStyle.uFormat,m_FontStyle.color.getColor().c);
	}
}

void CUIStyle::draw(const RECT& rc, const wchar_t* wcsText, CONTROL_STATE state, float fElapsedTime)
{
	Blend(state, fElapsedTime);
	draw(rc,wcsText);
}

void CUIStyle::playSound()
{
	UIGraph::getInstance().playSound(m_strSound.c_str());
}

bool CUIStyle::isVisible()
{
	return m_nVisible!=0;
}

// CRect<float>& CUIStyle::getTextRect()
// {
// 	return m_mapStyleDrawData[m_mapStyleDrawData.size()-1].rc;
// }

CUIStyle::CUIStyle()
	:m_nVisible(true)
{

}

CUIStyle::~CUIStyle()
{
	clear();
}

void CUIStyle::clear()
{
	m_StyleSprites.clear();
}

void CUIStyle::Refresh()
{
	//m_SpriteColor.Current = m_TextureColor.States[ CONTROL_STATE_HIDDEN ];
	//m_FontColor.Current = m_FontColor.States[ CONTROL_STATE_HIDDEN ];
}

// void CUIStyle::add(const std::vector<StyleElement*>& setStyleElement)
// {
// 	//m_StyleSprites.insert(m_StyleSprites.end(), setStyleElement.begin(), setStyleElement.end()); 
// }

void CUIStyle::XMLParse(const TiXmlElement& xml)
{
	const TiXmlElement* pElement = xml.FirstChildElement();
	while (pElement)
	{
		if (pElement->ValueStr() == "texture")
		{
			StyleSprite newStyleElement;
			newStyleElement.XMLParse(*pElement);
			m_StyleSprites.push_back(newStyleElement);
		}
		else if (pElement->ValueStr() == "font"||pElement->ValueStr() == "ubb")
		{
			m_FontStyle.XMLParse(*pElement);
		}
		pElement = pElement->NextSiblingElement();
	}
	//
	if (xml.Attribute("sound"))
	{
		m_strSound = GetStyleMgr().getDir()+xml.Attribute("sound");
	}
	//

	//
	//{
	//	for (size_t i=0;i< CONTROL_STATE_MAX;++i)
	//	{
	//		setOffset[i].set(0.0f,0.0f,0.0f,0.0f);
	//	}
	//	const TiXmlElement *pElement = xml.FirstChildElement("offset");
	//	if (pElement)
	//	{
	//		const char* pszText = pElement->GetText();
	//		if(pszText)
	//		{
	//			CRect<float> rc;
	//			rc.strToRect(pszText);
	//			for (size_t i=0;i< CONTROL_STATE_MAX;++i)
	//			{
	//				setOffset[i] = rc;
	//			}
	//		}
	//		for (size_t i=0;i< CONTROL_STATE_MAX;++i)
	//		{
	//			pszText =  pElement->Attribute(szControlState[i]);
	//			if (pszText)
	//			{
	//				setOffset[i].strToRect(pszText);
	//			}
	//		}
	//	}
	//}
	//

}

const StyleFont& CUIStyle::getFontStyle()const
{
	return m_FontStyle;
}

CUIStyleMgr::CUIStyleMgr()
{
}

bool CUIStyleMgr::Create(const char* szFilename)
{
	m_strFilename = szFilename;
	m_strDir = szFilename;
	m_strDir.replace(m_strDir.find_last_of('\\'),1,"/");
	const size_t pos = m_strDir.find_last_of('/');
	m_strDir=m_strDir.substr(0,pos+1);

	m_mapStyleData.clear();
	// Load XML
	TiXmlDocument myDocument;
	myDocument.LoadFile(szFilename, TIXML_ENCODING_UTF8);
	if (myDocument.Error())
	{
		return false;
	}
	//获得根元素，即root。
	const TiXmlElement *pRootElement = myDocument.RootElement();
	if (pRootElement==NULL)
	{
		return false;
	}
	//获得第一个Style节点。
	const TiXmlElement *pStyleElement = pRootElement->FirstChildElement("element");
	while (pStyleElement)
	{
		// Style name
		if (pStyleElement->Attribute("name"))
		{
			char szNameList[256]={'\0'};
			strcpy(szNameList,pStyleElement->Attribute("name"));
			char* pszName=strtok(szNameList,",");
			while(pszName)
			{
				CUIStyle& styleData = m_mapStyleData[pszName];//.add(StyleData);
				styleData.XMLParse(*pStyleElement);
				pszName = strtok(pszName+strlen(pszName)+1,",");
			}
		}
		// 查找下一个
		pStyleElement = pStyleElement->NextSiblingElement("element");
	}
	return true;
}

bool CUIStyleMgr::CreateFromMemory(const unsigned char* pBuffer, size_t bufferSize)
{
	m_mapStyleData.clear();
	// Load XML
	TiXmlDocument myDocument;
	myDocument.LoadFormMemory((char*)pBuffer, bufferSize, TIXML_ENCODING_UTF8);
	if (myDocument.Error())
	{
		return false;
	}
	//获得根元素，即root。
	const TiXmlElement *pRootElement = myDocument.RootElement();
	if (pRootElement==NULL)
	{
		return false;
	}
	//获得第一个Style节点。
	const TiXmlElement *pStyleElement = pRootElement->FirstChildElement("element");
	while (pStyleElement)
	{
		// Style name
		if (pStyleElement->Attribute("name"))
		{
			char szNameList[256]={'\0'};
			strcpy(szNameList,pStyleElement->Attribute("name"));
			char* pszName=strtok(szNameList,",");
			while(pszName)
			{
				CUIStyle& styleData = m_mapStyleData[pszName];//.add(StyleData);
				styleData.XMLParse(*pStyleElement);
				pszName = strtok(pszName+strlen(pszName)+1,",");
			}
		}
		// 查找下一个
		pStyleElement = pStyleElement->NextSiblingElement("element");
	}
	return true;
}

const CUIStyle& CUIStyleMgr::getStyleData(const std::string& strName)
{
	if (m_mapStyleData.find(strName)!=m_mapStyleData.end())
	{
		return m_mapStyleData[strName];
	}
	static CUIStyle s_StyleData;
	return s_StyleData;
}