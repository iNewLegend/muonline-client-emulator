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
#define DTL_CALCRECT				0x00001000 // �������
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
	TAG_TYPE_BOLD = 0,			// [B]����[/B]�������ֵ�λ�ÿ��������������Ҫ���ַ�����ʾΪ����Ч���� 
	TAG_TYPE_ITALIC,			// [I]����[/I]�������ֵ�λ�ÿ��������������Ҫ���ַ�����ʾΪб��Ч����
	TAG_TYPE_UNDERLINE,			// [U]����[/U]�������ֵ�λ�ÿ��������������Ҫ���ַ�����ʾΪ�»���Ч���� 
	TAG_TYPE_BR,				// [BR]������
	TAG_TYPE_ALIGN,				// [align=center]����[/align]�������ֵ�λ�ÿ��������������Ҫ���ַ���centerλ��center��ʾ���У�left��ʾ����right��ʾ���ҡ� 
	TAG_TYPE_URL,				// [URL]HTTP://WWW.RPGSKY.COM[/URL] 
								// [URL=HTTP://WWW.RPGSKY.COM]RPGSky[/URL]�������ַ������Լ��볬�����ӣ��������Ӿ����ַ�����������ӡ� 
	TAG_TYPE_SUP,				// [SUP]�ϱ�����[/SUP]
	TAG_TYPE_SUB,				// [SUB]�±�����[/SUB] 
	TAG_TYPE_FLIPH,				// [FLIPH]���ҵߵ�����[/FLIPH]
	TAG_TYPE_FLIPV,				// [FLIPV]���µߵ�����[/FLIPV]
	TAG_TYPE_IMG,				// [img]img/link/000.gif[/img]���ڱ�ǩ���м����ͼƬ��ַ����ʵ�ֲ�ͼЧ����
	TAG_TYPE_FLY,				// [fly]����[/fly]���ڱ�ǩ���м�������ֿ���ʵ�����ַ���Ч������������ơ�
	TAG_TYPE_MOVE,				// [move]����[/move]���ڱ�ǩ���м�������ֿ���ʵ�������ƶ�Ч����Ϊ����Ʈ����
	TAG_TYPE_GLOW,				// [glow=255,red,2]����[/glow]���ڱ�ǩ���м�������ֿ���ʵ�����ַ�����Ч��glow����������Ϊ��ȡ���ɫ�ͱ߽��С��
	TAG_TYPE_SHADOW,			// [shadow=255,red,2]����[/shadow]���ڱ�ǩ���м�������ֿ���ʵ��������Ӱ��Ч��shadow����������Ϊ��ȡ���ɫ�ͱ߽��С�� 
	TAG_TYPE_COLOR,				// [color=��ɫ����]����[/color]������������ɫ���룬�ڱ�ǩ���м�������ֿ���ʵ��������ɫ�ı䡣
	TAG_TYPE_SIZE,				// [size=����]����[/size]���������������С���ڱ�ǩ���м�������ֿ���ʵ�����ִ�С�ı䡣
	TAG_TYPE_FACE,				// [face=����]����[/face]����������Ҫ�����壬�ڱ�ǩ���м�������ֿ���ʵ����������ת����
	TAG_TYPE_DIR,				// [DIR=500,350]http://[/DIR]��Ϊ����shockwave��ʽ�ļ����м������Ϊ��Ⱥͳ��ȡ�
	TAG_TYPE_RM,				// [RM=500,350,1]http://[/RM]��Ϊ����realplayer��ʽ��rm�ļ������ֱַ�Ϊ��ȡ����ȡ�����ģʽ 
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

// CUBB �������޷�����һ���ַ�������ΪUBB�޷�֪��ĳ���ַ�����Ϣ��TexCharInfo����
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