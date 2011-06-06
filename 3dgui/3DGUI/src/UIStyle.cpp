#include "UIStyle.h"
#include "TextRender.h"
#include "tinyxml.h"
#include "UIGraph.h"

CUIStyleMgr& GetStyleMgr()
{
	static CUIStyleMgr g_UIStyleMgr;
	return g_UIStyleMgr;
}

const char* szControlState[]=
{
	"normal",
	"disabled",
	"hidden",
	"focus",
	"mouseover",
	"pressed",
};

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

CRect<float> StyleDrawData::updateRect(CRect<float> rect)
{
	CRect<float> rc;
	rc.left		= rect.left+rect.getWidth()*scale.left;
	rc.right	= rect.left+rect.getWidth()*scale.right;
	rc.top		= rect.top+rect.getHeight()*scale.top;
	rc.bottom	= rect.top+rect.getHeight()*scale.bottom;
	rc+= offset;
	return rc;
}

CUIStyle::CUIStyle():
m_nVisible(0)
{
	vTranslation.x=0.0f;
	vTranslation.y=0.0f;
	vTranslation.z=0.0f;

	vRotate.x=0.0f;
	vRotate.y=0.0f;
	vRotate.z=0.0f;
}

void CUIStyle::Blend(UINT iState, float fElapsedTime)
{
	if (uState!=iState)
	{
		uState = iState;
		fRate = 0.0f;
		if (CONTROL_STATE_HIDDEN!=iState)
		{
			m_nVisible=true;
		}
	}
	if (m_nVisible)
	{
		if (fRate<1.0f)
		{
			const CUIStyleData& styleData = getStyleData();

			fRate += styleData.setBlendRate[iState]*fElapsedTime;//1.0f - powf(styleData.setBlendRate[iState], 30 * fElapsedTime);
			fRate = min(1.0f,fRate);
			vRotate			= interpolate(fRate, vRotate, styleData.setRotate[iState]);
			vTranslation	= interpolate(fRate, vTranslation, styleData.setTranslation[iState]);
		}
		else if (CONTROL_STATE_HIDDEN==iState)
		{
			const CUIStyleData& styleData = getStyleData();
			styleData.blend(iState,1000,m_mapStyleDrawData);
			m_nVisible = false;
			return;
		}
		const CUIStyleData& styleData = getStyleData();
		styleData.blend(iState,fElapsedTime,m_mapStyleDrawData);
	}
}

void CUIStyle::SetStyle(const std::string& strName)
{
	m_strName = strName;
	Blend(CONTROL_STATE_HIDDEN,100);
}

const CUIStyleData& CUIStyle::getStyleData()
{
	return GetStyleMgr().getStyleData(m_strName);
}

void CUIStyle::draw(const Matrix& mTransform, const CRect<float>& rc, const wchar_t* wcsText)
{
	CRect<float> rcNew;
	rcNew.left =0;// -0.5f*rc.getWidth();
	rcNew.right = rc.getWidth();//0.5f*rc.getWidth();
	rcNew.top =0;// -0.5f*rc.getHeight();
	rcNew.bottom = rc.getHeight();//0.5f*rc.getHeight();
	mWorld = UIGraph::getInstance().setUIMatrix(mTransform,rc,vTranslation,vRotate);
	getStyleData().draw(rcNew, wcsText,m_mapStyleDrawData);
}

void CUIStyle::draw(const Matrix& mTransform, const CRect<float>& rc, const wchar_t* wcsText, CONTROL_STATE state, float fElapsedTime)
{
	Blend(state, fElapsedTime);
	draw(mTransform,rc,wcsText);
}

void CUIStyle::draw(const Matrix& mTransform, const CRect<int>& rc, const wchar_t* wcsText, CONTROL_STATE state, float fElapsedTime)
{
	Blend(state, fElapsedTime);
	draw(mTransform,rc.getRECT(),wcsText);
}

void CUIStyleData::blend(UINT iState, float fElapsedTime, std::map<int,StyleDrawData>& mapStyleDrawData)const
{
	for (size_t i=0; i<m_setStyleElement.size(); ++i)
	{
		mapStyleDrawData[i].blend(*m_setStyleElement[i],iState,fElapsedTime);
	}
}

void CUIStyleData::draw(const CRect<float>& rc, const wchar_t* wcsText, std::map<int,StyleDrawData>& mapStyleDrawData)const
{
// 	for(std::map<int,StyleDrawData>::iterator it=mapStyleDrawData.begin();it!=mapStyleDrawData.end();++it)
// 	{
// 	}
	for (size_t i=0; i<m_setStyleElement.size(); ++i)
	{
		CRect<float> rcReal=mapStyleDrawData[i].updateRect(rc);
		m_setStyleElement[i]->draw(wcsText,rcReal,mapStyleDrawData[i].color.getColor());
	}
}

void CUIStyle::playSound()
{
	UIGraph::getInstance().playSound(getStyleData().m_strSound.c_str());
}

bool CUIStyle::isVisible()
{
	return m_nVisible!=0;
}

// CRect<float>& CUIStyle::getTextRect()
// {
// 	return m_mapStyleDrawData[m_mapStyleDrawData.size()-1].rc;
// }

CUIStyleData::CUIStyleData():m_pFontStyleElement(NULL)
{
	for (size_t i=0;i< CONTROL_STATE_MAX;++i)
	{
		setBlendRate[i]=0.8f;
	}
	for (size_t i=0;i< CONTROL_STATE_MAX;++i)
	{
		setTranslation[i].x=0.0f;
		setTranslation[i].y=0.0f;
		setTranslation[i].z=0.0f;
	}
	for (size_t i=0;i< CONTROL_STATE_MAX;++i)
	{
		setRotate[i].x=0.0f;
		setRotate[i].y=0.0f;
		setRotate[i].z=0.0f;
	}
}

CUIStyleData::~CUIStyleData()
{
	clear();
}

void CUIStyleData::clear()
{
	m_pFontStyleElement = NULL;
	for (size_t i=0; i<m_setStyleElement.size(); ++i)
	{
		delete m_setStyleElement[i];
		m_setStyleElement[i] = NULL;
	}
	m_setStyleElement.clear();
}

void CUIStyleData::Refresh()
{
	//m_SpriteColor.Current = m_TextureColor.States[ CONTROL_STATE_HIDDEN ];
	//m_FontColor.Current = m_FontColor.States[ CONTROL_STATE_HIDDEN ];
}

void CUIStyleData::add(const std::vector<StyleElement*>& setStyleElement)
{
	m_setStyleElement.insert(m_setStyleElement.end(), setStyleElement.begin(), setStyleElement.end()); 
}

void CUIStyleData::XMLParse(const TiXmlElement& xml)
{
	const TiXmlElement* pElement = xml.FirstChildElement();
	while (pElement)
	{
		StyleElement* pNewStyleElement = NULL;
		if (pElement->ValueStr() == "texture")
		{
			pNewStyleElement = new StyleSprite;
		}
		else if (pElement->ValueStr() == "border")
		{
			pNewStyleElement = new StyleBorder;
		}
		else if (pElement->ValueStr() == "square")
		{
			pNewStyleElement = new StyleSquare;
		}
		else if (pElement->ValueStr() == "font"||pElement->ValueStr() == "ubb")
		{
			pNewStyleElement = new StyleText;
			m_pFontStyleElement = pNewStyleElement;
		}
		else
		{
			pNewStyleElement = new StyleBorder;
		}
		pNewStyleElement->XMLParse(*pElement);
		m_setStyleElement.push_back(pNewStyleElement);
		pElement = pElement->NextSiblingElement();
	}
	//
	if (xml.Attribute("sound"))
	{
		m_strSound = GetStyleMgr().getDir()+xml.Attribute("sound");
	}
	//
	{
		const TiXmlElement *pElement = xml.FirstChildElement("blend");
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
	}
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
	{
		const TiXmlElement *pElement = xml.FirstChildElement("translation");
		if (pElement)
		{
			const char* pszText = pElement->GetText();
			if(pszText)
			{
				MY3DGUI_VEC3D v;
				sscanf_s(pszText, "%f,%f,%f", &v.x, &v.y, &v.z);
				for (size_t i=0;i< CONTROL_STATE_MAX;++i)
				{
					setTranslation[i] = v;
				}
			}
			for (size_t i=0;i< CONTROL_STATE_MAX;++i)
			{
				pszText =  pElement->Attribute(szControlState[i]);
				if (pszText)
				{
					sscanf_s(pszText, "%f,%f,%f", &setTranslation[i].x, &setTranslation[i].y, &setTranslation[i].z);
				}
			}
		}
	}
	{
		const TiXmlElement *pElement = xml.FirstChildElement("rotate");
		if (pElement)
		{
			const char* pszText = pElement->GetText();
			if(pszText)
			{
				MY3DGUI_VEC3D v;
				sscanf_s(pszText, "%f,%f,%f", &v.x, &v.y, &v.z);
				v.x*=3.14159f/180.0f;
				v.y*=3.14159f/180.0f;
				v.z*=3.14159f/180.0f;
				for (size_t i=0;i< CONTROL_STATE_MAX;++i)
				{
					setRotate[i] = v;
				}
			}
			for (size_t i=0;i< CONTROL_STATE_MAX;++i)
			{
				pszText =  pElement->Attribute(szControlState[i]);
				if (pszText)
				{
					sscanf_s(pszText, "%f,%f,%f", &setRotate[i].x, &setRotate[i].y, &setRotate[i].z);
					setRotate[i].x*=3.14159f/180.0f;
					setRotate[i].y*=3.14159f/180.0f;
					setRotate[i].z*=3.14159f/180.0f;
				}
			}
		}
	}
}

const StyleElement* CUIStyleData::getFontStyleElement()const
{
	return m_pFontStyleElement;
}

CUIStyleMgr::CUIStyleMgr()
{
}

void StyleDrawData::blend(const StyleElement& se,UINT iState,float fElapsedTime)
{
	if (uState!=iState)
	{
		uState = iState;
		fRate = 0.0f;
	}
	if (fRate<1.0f)
	{
		fRate += se.setBlendRate[iState]*fElapsedTime;
		fRate = min(1.0f,fRate);

		color	= interpolate(fRate, color,	se.setColor[iState]);
		offset	= interpolate(fRate, offset,se.setOffset[iState]);
		scale	= interpolate(fRate, scale,	se.setScale[iState]);
	}
}

void StyleElement::XMLParse(const TiXmlElement& element)
{
	{
		for (size_t i=0;i< CONTROL_STATE_MAX;++i)
		{
			setColor[i].set(0.0f,0.0f,0.0f,0.0f);
		}
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
	}
	//
	{
		for (size_t i=0;i< CONTROL_STATE_MAX;++i)
		{
			setBlendRate[i]=0.8f;
		}
		const TiXmlElement *pElement = element.FirstChildElement("blend");
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
	}
	//
	{
		for (size_t i=0;i< CONTROL_STATE_MAX;++i)
		{
			setOffset[i].set(0.0f,0.0f,0.0f,0.0f);
		}
		const TiXmlElement *pElement = element.FirstChildElement("offset");
		if (pElement)
		{
			const char* pszText = pElement->GetText();
			if(pszText)
			{
				CRect<float> rc;
				rc.setByString(pszText);
				for (size_t i=0;i< CONTROL_STATE_MAX;++i)
				{
					setOffset[i] = rc;
				}
			}
			for (size_t i=0;i< CONTROL_STATE_MAX;++i)
			{
				pszText =  pElement->Attribute(szControlState[i]);
				if (pszText)
				{
					setOffset[i].setByString(pszText);
				}
			}
		}
	}
	// 
	{
		for (size_t i=0;i< CONTROL_STATE_MAX;++i)
		{
			setScale[i].set(0.0f,0.0f,1.0f,1.0f);
		}
		const TiXmlElement *pElement = element.FirstChildElement("scale");
		if (pElement)
		{
			const char* pszText = pElement->GetText();
			if(pszText)
			{
				CRect<float> rc;
				rc.setByString(pszText);
				for (size_t i=0;i< CONTROL_STATE_MAX;++i)
				{
					setScale[i] = rc;
				}
			}
			for (size_t i=0;i< CONTROL_STATE_MAX;++i)
			{
				pszText =  pElement->Attribute(szControlState[i]);
				if (pszText)
				{
					setScale[i].setByString(pszText);
				}
			}
		}
	}
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
		m_rcBorder.setByString(strRect);
		m_rcBorder.right	+= m_rcBorder.left;
		m_rcBorder.bottom	+= m_rcBorder.top;
		if (element.Attribute("center_rect"))
		{
			m_nSpriteLayoutType = SPRITE_LAYOUT_3X3GRID;
			const char* strCenterRect = element.Attribute("center_rect");
			m_rcCenter.setByString(strCenterRect);
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

void StyleText::XMLParse(const TiXmlElement& element)
{
	StyleElement::XMLParse(element);
	uFormat = 0;
	if (element.Attribute("format"))
	{
		uFormat = StrToTextFormat(element.Attribute("format"));
	}
}

void StyleElement::draw(const wchar_t* wcsText,const CRect<float>& rc,const Color32& color)const
{
}

void StyleText::draw(const wchar_t* wcsText,const CRect<float>& rc,const Color32& color)const
{
	if(color.a==0)
	{
		return;
	}
	UIGraph::getInstance().drawText(wcsText,-1,rc.getRECT(),uFormat,color.c);
}

void StyleSprite::draw(const wchar_t* wcsText,const CRect<float>& rc,const Color32& color)const
{
	if(color.a==0)
	{
		return;
	}
	//if (m_bDecolor)
	//{
	//	GetRenderSystem().SetTextureStageStateDecolor();
	//}
	switch(m_nSpriteLayoutType)
	{
	case SPRITE_LAYOUT_WRAP:
		UIGraph::getInstance().DrawSprite(rc,rc,m_pTexture,color);
		break;
	case SPRITE_LAYOUT_SIMPLE:
		UIGraph::getInstance().DrawSprite(m_rcBorder,rc,m_pTexture,color);
		break;
	case SPRITE_LAYOUT_3X3GRID:
		UIGraph::getInstance().DrawSprite3x3Grid(m_rcBorder,m_rcCenter,rc,m_pTexture,color);
		break;
	default:
		break;
	}
	//if (m_bDecolor)
	//{
	//	GetRenderSystem().SetupRenderState();
	//}
}

void StyleBorder::draw(const wchar_t* wcsText,const CRect<float>& rc,const Color32& color)const
{
	if(color.a==0)
	{
		return;
	}
	//UIGraph::getInstance().DrawRect(rc.left,rc.right,rc.top,rc.bottom,color);
	//GetRenderSystem().SetTextureColorOP(0,TBOP_SOURCE2);
	//GetRenderSystem().SetTextureAlphaOP(0,TBOP_SOURCE2);
	//GetGraphics().DrawRect(rc, color);
	//GetRenderSystem().SetTextureColorOP(0,TBOP_MODULATE);
	//GetRenderSystem().SetTextureAlphaOP(0,TBOP_MODULATE);
}

void StyleSquare::draw(const wchar_t* wcsText,const CRect<float>& rc,const Color32& color)const
{
	if(color.a==0)
	{
		return;
	}
	UIGraph::getInstance().FillRect(rc,color);
	//GetRenderSystem().SetTextureColorOP(0,TBOP_SOURCE2);
	//GetRenderSystem().SetTextureAlphaOP(0,TBOP_SOURCE2);
	//GetGraphics().FillRect(rc, color);
	//GetRenderSystem().SetTextureColorOP(0,TBOP_MODULATE);
	//GetRenderSystem().SetTextureAlphaOP(0,TBOP_MODULATE);
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
				CUIStyleData& styleData = m_mapStyleData[pszName];//.add(StyleData);
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
				CUIStyleData& styleData = m_mapStyleData[pszName];//.add(StyleData);
				styleData.XMLParse(*pStyleElement);
				pszName = strtok(pszName+strlen(pszName)+1,",");
			}
		}
		// 查找下一个
		pStyleElement = pStyleElement->NextSiblingElement("element");
	}
	return true;
}

const CUIStyleData& CUIStyleMgr::getStyleData(const std::string& strName)
{
	if (m_mapStyleData.find(strName)!=m_mapStyleData.end())
	{
		return m_mapStyleData[strName];
	}
	static CUIStyleData s_StyleData;
	return s_StyleData;
}