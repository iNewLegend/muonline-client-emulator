// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "windows.h"
#include <vector>
#include <string>

struct TexCharInfo
{
	int nLeft;
	int nTop;
	int nWidth;
	int nHeight;
	int nOffsetX;
	int nOffsetY;
	int nAdvX;
	int nAdvY;
	float fU0,fV0,fU1,fV1;
};

/*
* DrawText() Format Flags
*/
#define DTL_TOP						0x00000001
#define DTL_LEFT					0x00000002
#define DTL_CENTER					0x00000004
#define DTL_RIGHT					0x00000008
#define DTL_VCENTER					0x00000010
#define DTL_BOTTOM					0x00000020
#define DTL_WORDBREAK				0x00000040
#define DTL_SINGLELINE				0x00000080
#define DTL_EXPANDTABS				0x00000100
#define DTL_TABSTOP					0x00000200
#define DTL_NOCLIP					0x00000400
#define DTL_EXTERNALLEADING			0x00000800
#define DTL_CALCRECT				0x00001000 // 计算矩阵
#define DTL_NOPREFIX				0x00002000
#define DTL_INTERNAL				0x00004000

enum UBB_ALIGN_TYPE
{
	ALIGN_TYPE_CENTER	=1,
	ALIGN_TYPE_RIGHT	=1<<1,
	ALIGN_TYPE_VCENTER	=1<<2,
	ALIGN_TYPE_BOTTOM	=1<<3,
};

enum UBB_TAG_TYPE
{
	TAG_TYPE_BOLD = 0,			// [B]文字[/B]：在文字的位置可以任意加入您需要的字符，显示为粗体效果。 
	TAG_TYPE_ITALIC,			// [I]文字[/I]：在文字的位置可以任意加入您需要的字符，显示为斜体效果。
	TAG_TYPE_UNDERLINE,			// [U]文字[/U]：在文字的位置可以任意加入您需要的字符，显示为下划线效果。 
	TAG_TYPE_BR,				// [BR]：换行
	TAG_TYPE_ALIGN,				// [align=center]文字[/align]：在文字的位置可以任意加入您需要的字符，center位置center表示居中，left表示居左，right表示居右。 
	TAG_TYPE_URL,				// [URL]HTTP://WWW.RPGSKY.COM[/URL] 
								// [URL=HTTP://WWW.RPGSKY.COM]RPGSky[/URL]：有两种方法可以加入超级连接，可以连接具体地址或者文字连接。 
	TAG_TYPE_SUP,				// [SUP]上标文字[/SUP]
	TAG_TYPE_SUB,				// [SUB]下标文字[/SUB] 
	TAG_TYPE_FLIPH,				// [FLIPH]左右颠倒文字[/FLIPH]
	TAG_TYPE_FLIPV,				// [FLIPV]上下颠倒文字[/FLIPV]
	TAG_TYPE_IMG,				// [img]img/link/000.gif[/img]：在标签的中间插入图片地址可以实现插图效果。
	TAG_TYPE_FLY,				// [fly]文字[/fly]：在标签的中间插入文字可以实现文字飞翔效果，类似跑马灯。
	TAG_TYPE_MOVE,				// [move]文字[/move]：在标签的中间插入文字可以实现文字移动效果，为来回飘动。
	TAG_TYPE_GLOW,				// [glow=255,red,2]文字[/glow]：在标签的中间插入文字可以实现文字发光特效，glow内属性依次为宽度、颜色和边界大小。
	TAG_TYPE_SHADOW,			// [shadow=255,red,2]文字[/shadow]：在标签的中间插入文字可以实现文字阴影特效，shadow内属性依次为宽度、颜色和边界大小。 
	TAG_TYPE_COLOR,				// [color=颜色代码]文字[/color]：输入您的颜色代码，在标签的中间插入文字可以实现文字颜色改变。
	TAG_TYPE_SIZE,				// [size=数字]文字[/size]：输入您的字体大小，在标签的中间插入文字可以实现文字大小改变。
	TAG_TYPE_FACE,				// [face=字体]隶书[/face]：输入您需要的字体，在标签的中间插入文字可以实现文字字体转换。
	TAG_TYPE_DIR,				// [DIR=500,350]http://[/DIR]：为插入shockwave格式文件，中间的数字为宽度和长度。
	TAG_TYPE_RM,				// [RM=500,350,1]http://[/RM]：为插入realplayer格式的rm文件，数字分别为宽度、长度、播放模式 
	TAG_TYPE_MAX,
};

static const wchar_t* gs_UBB_TAG_TYPE_NAME[] =
{
	L"b",
	L"i",
	L"u",
	L"br",
	L"align",
	L"url",
	L"sup",
	L"sub",
	L"fliph",
	L"flipv",
	L"img",
	L"fly",
	L"move",
	L"glow",
	L"shadow",
	L"color",
	L"size",
	L"face",
	L"dir",
	L"rm",
};
struct VERTEX_TEXT
{
	float x;
	float y;
	float z;
	unsigned long color;
	float u;
	float v;
};

// CUBB 自身是无法解析一段字符串，因为UBB无法知道某个字符的信息（TexCharInfo）。
class CUBB
{
public:
	CUBB(); 
	~CUBB();

	void	Init						(int nShowWidth, int nFontSize, int nAlignType=0); 
	void	ParseTag					(wchar_t* wcsTagt);
	void	AddChar						(const TexCharInfo* charInfo);
	void	updateTextLine				();
	void	setColor(DWORD dwColor)		{m_dwColor = dwColor;}
	int		getMaxWidth					(){return m_nMaxWidth;}
	int		getMaxHeight				(){return m_nMaxHeight;}
public:
	std::vector<VERTEX_TEXT> m_VB;
protected:
	std::vector<int>	m_TagTypeRecords[TAG_TYPE_MAX];
	int		m_nBold;
	int		m_nItalic;
	bool	m_bUnderline;
	int		m_nAlignType;
	bool	m_bSup;
	bool	m_bSub;
	bool	m_bFlipH;
	bool	m_bFlipV;
	DWORD	m_dwColor;
	int		m_nSize;

	int		m_nFontSize;

	int		m_nShowWidth;

	int		m_nMaxWidth;
	int		m_nMaxHeight;

	int		m_nLineWidth;
	int		m_nLineHeight;

	int		m_nLineBegin;
	int		m_nLineEnd;
};