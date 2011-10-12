// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "Color.h"
#include <vector>
#include <map>
#include <Windows.h>
#include "Vec4D.h"
#include "interpolation.h"
#include "Rect.h"
#include "Matrix.h"

class TiXmlElement;

enum CONTROL_STATE
{
	CONTROL_STATE_NORMAL,
	CONTROL_STATE_DISABLED,
	CONTROL_STATE_HIDDEN,
	CONTROL_STATE_FOCUS,
	CONTROL_STATE_MOUSEOVER,
	CONTROL_STATE_PRESSED,
	CONTROL_STATE_MAX,
};
static const char* szControlState[]=
{
	"normal",
	"disabled",
	"hidden",
	"focus",
	"mouseover",
	"pressed",
};
// 纹理布局的三种类型
enum SPRITE_LAYOUT_TYPE
{
	SPRITE_LAYOUT_WRAP,
	SPRITE_LAYOUT_SIMPLE,
	SPRITE_LAYOUT_3X3GRID,
};

#define MY3DGUI_VEC3D Vec3D 
// struct MY3DGUI_VEC3D
// {
// 	float x,y,z;
// };

struct MY3DGUI_VEC4D
{
	float x,y,z;
};

class StyleElement
{
public:
	StyleElement();
	RECT updateRect(RECT rect);
	virtual void XMLParse(const TiXmlElement& element);
	// ----
	Color32			setColor[CONTROL_STATE_MAX];
	RECT			rcOffset;
	RECT			rcScale;
	// ----
	Color32			color;

};

class StyleSprite: public StyleElement
{
public:
	virtual void XMLParse(const TiXmlElement& element);
	void		draw(const RECT& rc);
	void*		m_pTexture;
	bool		m_bDecolor;
	int			m_nLayoutType;
	RECT		m_rcBorder;
	RECT		m_rcCenter;
};

class StyleFont: public StyleElement
{
public:
	virtual void XMLParse(const TiXmlElement& element);
	unsigned int uFormat;
};

class CUIStyle
{
public:
	CUIStyle();
	~CUIStyle();
	void clear();
	void Refresh();
	virtual void XMLParse(const TiXmlElement& xml);
	const StyleFont& getFontStyle()const;
	std::vector<StyleSprite>	m_StyleSprites;
	StyleFont					m_FontStyle;
	std::string					m_strSound;

	float						m_setBlendRate[CONTROL_STATE_MAX];
	float						m_fRate;
	unsigned int				m_uState;

	int							m_nVisible;
	std::string					m_strName;

	void Blend(UINT iState, float fElapsedTime);
	void draw(const RECT& rc, const wchar_t* wcsText);
	void draw(const RECT& rc, const wchar_t* wcsText, CONTROL_STATE state, float fElapsedTime);

	void setStyle(const std::string& strName);
	void playSound();
	bool isVisible();
};

class CUIStyleMgr
{
	std::map<std::string, CUIStyle> m_mapStyleData;
	std::string m_strFilename;
	std::string m_strDir;
public:
	CUIStyleMgr();
	bool Create(const char* szFilename);
	bool CreateFromMemory(const unsigned char* pBuffer, size_t bufferSize);
	const std::string& getFilename(){return m_strFilename;}
	const std::string& getDir(){return m_strDir;}
	const CUIStyle& getStyleData(const std::string& strName);
};

CUIStyleMgr& GetStyleMgr();