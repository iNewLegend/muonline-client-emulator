/**
	RSBmFont.h
	----------

	Bitmap Font for RealSpace

	Programming by Joongpil Cho, Na Ja Young
	All copyright (c) MAIET entertainment
*/
#include "RSFont.h"
#include <memory.h>

#ifndef __RSBMFONT_HEADER__
#define __RSBMFONT_HEADER__


// RsBmFont class
// version 2
// ALPHA/KOREAN support

class RSBmFont {
private:
	RECT		m_rcClip;		// Ŭ���� ����
	RealSpace*	m_pRS;
	RSFont*		m_pKorFnt;		// ���� ��Ʈ
	RSFont*		m_pEngFnt;		// ���� ��Ʈ

	int			GetKsIndex(BYTE fstByte, BYTE sndByte){
		return (((int)fstByte  - 0xb0) * 94 + (int)sndByte - 0xa1);
	}
public:
	RSBmFont(RealSpace *pRS);
	virtual	~RSBmFont();

	// ���� szKorFontName�� NULL�� ������ ���� ��� ��������...
	BOOL		Open(const char *szEngFontName, const char *szKorFontName = NULL);
	void		Close();
	
	// ���� 0�̸� ������Ʈ�� ���̸�... 1�̸� �ѱ���Ʈ�� ���̸� �����Ѵ�.
	int			GetHeight(int nIndex = 0){
		if(nIndex==0) return m_pEngFnt->GetHeight(); else return m_pKorFnt->GetHeight();
	}
	
	int			GetMaxHeight() { return max(m_pEngFnt?m_pEngFnt->GetHeight():0,m_pKorFnt?m_pKorFnt->GetHeight():0); }
	
	// return value : width of the drawn character.
	void		Puts( LPBYTE pBuffer, long lPitch, int nX, int nY, DWORD nColor, char *szMessage );
	// return value : ��µ� ĳ���� ��(byte��...)
	int			PutsWithClip( LPBYTE pBuffer, long lPitch, int nX, int nY, DWORD nColor, char *szMessage );

/*
	//'\n'�� ���ԵǾ� �ִ� �ؽ�Ʈ�� ����Ѵ�.
	// ��ȯ���� ���ݱ��� ��µ� �ؽ�Ʈ�� �ε���
	int			TextWithClip(LPBYTE pBuffer, long lPitch, int nX, int nY, DWORD nColor, char *szMessage);
*/

	// If it fails, returns -1
	int			GetStringWidth( char *szMessage );
	int			GetPossibleCharacterCount( int nWidth, const char *string );

	void		SetClipRect(int nLeft, int nTop, int nRight, int nBottom){
		RECT rc = {nLeft, nTop, nRight, nBottom};
		SetClipRect(rc);
	}

	//���� �׷��� �ִ� �ؽ�Ʈ���� �ƹ��� ȿ���� ����.
	void		SetClipRect(RECT rc){ memcpy(&m_rcClip,&rc,sizeof(RECT)); }
	void		GetClipRect(RECT* rc){ memcpy(rc, &m_rcClip, sizeof(RECT)); }
};


#endif // __RSFONT_HEADER__

/////////////////////////////////////////////////////////////////////////