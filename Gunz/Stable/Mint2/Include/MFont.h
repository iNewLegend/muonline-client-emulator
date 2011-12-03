//
//		Font
//
#ifndef MFONT_H
#define MFONT_H

#include "MTypes.h"

#define MFONT_NAME_LENGTH		32

// Font abstract class
class MFont{
public:
#ifdef _DEBUG
	int		m_nTypeID;
#endif

public:
	char	m_szName[MFONT_NAME_LENGTH];

	int				m_nOutlineStyle;
	unsigned long	m_ColorArg1;	// For Custom FontStyle like OutlineStyle
	unsigned long	m_ColorArg2;

public:
	MFont(void);
	virtual ~MFont(void);
	virtual bool Create(const char* szName);
	virtual void Destroy(void);
	virtual int GetHeight(void) = 0;
	virtual int GetWidth(const char* szText, int nSize=-1) = 0;
	virtual int GetWidthWithoutAmpersand(const char* szText, int nSize=-1);
//	virtual int GetLineCount(int nRectWidth, const char* szText);		// dc �� TextMultiLine��ǰ� �������
//	virtual int GetPossibleCharacterCount(int nRectWidth,const char* szText);	// nRectWidth �ȿ� ������ �ִ� ���ڼ�
	virtual bool Resize(float ratio, int nMinimumHeight) { _ASSERT(0); return true; } // �� assert�� �ʿ�� ����. ���� �������� �����߿� ������ �� �ֵ��� ������
};


#endif