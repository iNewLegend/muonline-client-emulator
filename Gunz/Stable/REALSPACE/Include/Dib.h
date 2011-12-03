//
//	Dib(bmp)�� 24��Ʈ�� �о� ���� Ŭ����
//
//											written by lee jang ho
//
//////////////////////////////////////////////////////////////////////

#ifndef _DIB_H
#define _DIB_H

#include <windows.h>

// Dib(bmp)�� 24��Ʈ�� �о� ���� Ŭ����
class CDib  
{
protected:
	HBITMAP m_hBitmap;			// Bitmap Handle
	LPBYTE	m_pBitmapData;		// Bitmap Memory Data
	BOOL	m_bTopDown;			// Top Down (Origin is upper left)

public:
	CDib();
	virtual ~CDib();

	BOOL Open(HWND hWnd, const char *pFileName, BOOL bOpenFromFile=TRUE);
	void Close(void);

	void BitBlt(HDC hDC, int x, int y, int cx, int cy);

	LONG GetWidth(void);
	LONG GetHeight(void);

	// Get Image Pointer
	LPBYTE GetData(void);

	// Top? or Down?
	BOOL IsTopDown(void);
};

// GDI���� ���ø����̼ǿ� ����ϵ��� ���� DIBŬ����
class CGDIDib
{
protected:
	HBITMAP m_hBitmap;			// Bitmap Handle
	LPBYTE	m_pBitmapData;		// Bitmap Memory Data
	BOOL	m_bTopDown;			// Top Down (Origin is upper left)

public:
	CGDIDib();
	virtual ~CGDIDib();

	BOOL Open(HINSTANCE hInstance,HWND hWnd, const char *pFileName);
	BOOL Open(HWND hWnd, const char *pFileName, BOOL bOpenFromFile=TRUE);
	void Close(void);

	void BitBlt(HDC hDC, int x, int y, int cx, int cy);

	LONG GetWidth(void);
	LONG GetHeight(void);

	// Get Image Pointer
	LPBYTE GetData(void);
	HBITMAP GetBitmap(void){ return m_hBitmap; }

	// Top? or Down?
	BOOL IsTopDown(void){ return m_bTopDown; }
	BOOL IsPalette(void);
};

#endif // _DIB_H
