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
	StyleElement()
	{
		memset(this,0,sizeof(*this));
	}

	virtual void XMLParse(const TiXmlElement& element);
	virtual void draw(const wchar_t* wcsText,const CRect<float>& rc,const Color32& color)const;

	float			setBlendRate[CONTROL_STATE_MAX];
	Vec4D			setColor[CONTROL_STATE_MAX];
	CRect<float>	setOffset[CONTROL_STATE_MAX];
	CRect<float>	setScale[CONTROL_STATE_MAX];
};

struct StyleDrawData
{
	StyleDrawData()
	{
		memset(this,0,sizeof(*this));
	}

	Vec4D	color;
	CRect<float> scale;
	CRect<float> offset;
	float			fRate;
	unsigned int	uState;
	void blend(const StyleElement& se,UINT iState,float fElapsedTime);
	CRect<float> updateRect(CRect<float> rect);
};

class StyleSprite: public StyleElement
{
public:
	virtual void XMLParse(const TiXmlElement& element);
	void		draw(const wchar_t* wcsText,const CRect<float>& rc,const Color32& color)const;
	void*		m_pTexture;
	bool		m_bDecolor;
	int			m_nSpriteLayoutType;
	CRect<float>m_rcBorder;
	CRect<float>m_rcCenter;
};

class  StyleBorder: public StyleElement
{
public:
	void draw(const wchar_t* wcsText,const CRect<float>& rc,const Color32& color)const;
};

class  StyleSquare: public StyleElement
{
public:
	void draw(const wchar_t* wcsText,const CRect<float>& rc,const Color32& color)const;
};

class StyleText: public StyleElement
{
public:
	virtual void XMLParse(const TiXmlElement& element);
	void draw(const wchar_t* wcsText,const CRect<float>& rc,const Color32& color)const;
	unsigned int uFormat;
};

class CUIStyleData
{
public:
	CUIStyleData();
	~CUIStyleData();
	void clear();
	void Refresh();
	void add(const std::vector<StyleElement*>& setStyleElement);

	virtual void XMLParse(const TiXmlElement& xml);

	void blend(UINT iState, float fElapsedTime, std::map<int,StyleDrawData>& mapStyleDrawData)const;
	void draw(const CRect<float>& rc, const wchar_t* wcsText, std::map<int,StyleDrawData>& mapStyleDrawData)const;
	const StyleElement* getFontStyleElement()const;

	std::vector<StyleElement*>	m_setStyleElement;
	StyleElement*				m_pFontStyleElement;
	std::string		m_strSound;
	float			setBlendRate[CONTROL_STATE_MAX];
	MY3DGUI_VEC3D			setTranslation[CONTROL_STATE_MAX];
	MY3DGUI_VEC3D			setRotate[CONTROL_STATE_MAX];
};

class CUIStyle
{
public:
	CUIStyle();
	~CUIStyle(){};
	void Blend(UINT iState, float fElapsedTime);
	void draw(const Matrix& mTransform, const CRect<float>& rc, const wchar_t* wcsText);
	void SetStyle(const std::string& strName);
	const CUIStyleData& getStyleData();
	void draw(const Matrix& mTransform, const CRect<float>& rc, const wchar_t* wcsText, CONTROL_STATE state, float fElapsedTime);
	void draw(const Matrix& mTransform, const CRect<int>& rc, const wchar_t* wcsText, CONTROL_STATE state, float fElapsedTime);
	void playSound();
	bool isVisible();

	int m_nVisible;
	std::string	m_strName;
	std::map<int,StyleDrawData> m_mapStyleDrawData;
	Matrix	mWorld;
	MY3DGUI_VEC3D	vTranslation;
	MY3DGUI_VEC3D	vRotate;
	float	fRate;
	unsigned int uState;
};

class CUIStyleMgr
{
	std::map<std::string, CUIStyleData> m_mapStyleData;
	std::string m_strFilename;
	std::string m_strDir;
public:
	CUIStyleMgr();
	bool Create(const char* szFilename);
	bool CreateFromMemory(const unsigned char* pBuffer, size_t bufferSize);
	const std::string& getFilename(){return m_strFilename;}
	const std::string& getDir(){return m_strDir;}
	const CUIStyleData& getStyleData(const std::string& strName);
};

CUIStyleMgr& GetStyleMgr();