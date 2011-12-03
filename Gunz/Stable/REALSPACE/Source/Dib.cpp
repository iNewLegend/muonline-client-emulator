//
//	Dib(bmp)�� 24��Ʈ�� �о� ���� Ŭ����
//
//											written by lee jang ho
//
//////////////////////////////////////////////////////////////////////

#include "Dib.h"
#include "CMError.h"

#include <crtdbg.h>

#define WIDTHBYTES(i)	((unsigned)((i+31)&(~31))/8)						// 4����Ʈ ����
#define DibPtr(lpbi)	(LPVOID)(DibColors(lpbi) + (UINT)(lpbi)->biClrUsed)	// Data Pointer
#define DibColors(lpbi)	((LPRGBQUAD)((LPBYTE)(lpbi) + (int)(lpbi)->biSize))	// Palette Pointer

CDib::CDib()
{
	m_hBitmap=NULL;
	m_pBitmapData=NULL;
}

CDib::~CDib()
{
	Close();
}

BOOL CDib::Open(HWND hWnd, const char *pFileName, BOOL bOpenFromFile)
{
	/*
	UINT fuLoad;

	if(bOpenFromFile==TRUE)
		fuLoad = LR_CREATEDIBSECTION|LR_LOADFROMFILE|LR_DEFAULTSIZE;
	else
		fuLoad = (bOpenFromFile?LR_CREATEDIBSECTION:0)|LR_LOADFROMFILE|LR_DEFAULTSIZE;
	*/
	m_hBitmap=(HBITMAP)::LoadImage(
		(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),
		pFileName,
		IMAGE_BITMAP,
		0,0,
		//fuLoad);
		LR_CREATEDIBSECTION|(bOpenFromFile?LR_LOADFROMFILE:0)|LR_DEFAULTSIZE);
	if(m_hBitmap==NULL){
		SetErrors(CMERR_CANT_OPEN_FILE,pFileName);
		return FALSE;
	}

	BITMAP bm;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;	// 24bit�� �ȷ�Ʈ ������ ���� ���̹Ƿ� �ʿ������... Ȯ���� ����

	GetObject(m_hBitmap,sizeof(BITMAP),&bm);

	if(bm.bmHeight>=0)m_bTopDown=FALSE;
	else m_bTopDown=TRUE;

    bi.biSize				= sizeof(BITMAPINFOHEADER);
    bi.biWidth				= bm.bmWidth;
    bi.biHeight				= bm.bmHeight;
    bi.biPlanes				= 1;
    //bi.biBitCount           = bm.bmPlanes * bm.bmBitsPixel;
	bi.biBitCount			= 24;			// 8bit �� 24bit�� �о��. ���� Pal ������ ����.
    bi.biCompression		= BI_RGB;
    bi.biSizeImage			= 0;
    bi.biXPelsPerMeter		= 0;
    bi.biYPelsPerMeter		= 0;
    bi.biClrUsed			= 0;
    bi.biClrImportant		= 0;

// �ȷ�Ʈ ����
#define DibNumColors(lpbi)	((lpbi)->biClrUsed == 0 && (lpbi)->biBitCount <= 8 \
								? (WORD)(1 << (int)(lpbi)->biBitCount) \
								: (WORD)(lpbi)->biClrUsed)

	// BITMAPINFO( BITMAPINFOHEADER+PAL ) �� ũ��
	DWORD nLen  = bi.biSize + DibNumColors(&bi) * sizeof(RGBQUAD);

	lpbi=(LPBITMAPINFOHEADER)new char[nLen];

	*lpbi=bi;

	HDC hDC=GetDC(hWnd);
	GetDIBits(hDC,m_hBitmap,0,bi.biHeight,NULL,(LPBITMAPINFO)lpbi,DIB_RGB_COLORS);
	ReleaseDC(hWnd,hDC);

	bi=*lpbi;

	// ����̹��� biSizeImage�� ä���� �ʴ� ���
	if(bi.biSizeImage==0){
		bi.biSizeImage=(DWORD)WIDTHBYTES(bm.bmWidth*bi.biBitCount)*bm.bmHeight;
		if(bi.biCompression!=BI_RGB)
			bi.biSizeImage=(bi.biSizeImage*3)/2;
	}

	delete[] lpbi;

	nLen=bi.biSize+DibNumColors(&bi)*sizeof(RGBQUAD)+bi.biSizeImage;

	//lpbi=(LPBITMAPINFOHEADER)new char[nLen];
	m_pBitmapData=new BYTE[nLen];

	if(m_pBitmapData==NULL){
		//_RPT0(_CRT_WARN,"Memory Allocation Error");
		SetError(CMERR_OUT_OF_MEMORY);
		return FALSE;
	}

	*(LPBITMAPINFOHEADER)m_pBitmapData=bi;

	hDC=GetDC(hWnd);
	GetDIBits(hDC,m_hBitmap,0,bi.biHeight,DibPtr((LPBITMAPINFOHEADER)m_pBitmapData),(LPBITMAPINFO)m_pBitmapData,DIB_RGB_COLORS);
	ReleaseDC(hWnd,hDC);
	
	return TRUE;
}

void CDib::BitBlt(HDC hDC, int x, int y, int cx, int cy)
{
	if(m_hBitmap==NULL)return;

	HDC hMemDC=::CreateCompatibleDC(hDC);

	HGDIOBJ hOldObj=::SelectObject(hMemDC,m_hBitmap);

	::BitBlt(hDC,x,y,cx,cy,hMemDC,0,0,SRCCOPY);

	::SelectObject(hMemDC,hOldObj);

	::DeleteDC(hMemDC);
}

void CDib::Close(void)
{
	//::CloseHandle(m_hBitmap);
	if(m_pBitmapData!=NULL){
		delete[] m_pBitmapData;
		m_pBitmapData=NULL;
	}
	if(m_hBitmap!=NULL){
		::DeleteObject(m_hBitmap);
		m_hBitmap=NULL;
	}
}

LONG CDib::GetWidth(void)
{
	_ASSERT(m_pBitmapData!=NULL);
	return ((LPBITMAPINFOHEADER)m_pBitmapData)->biWidth;
}

LONG CDib::GetHeight(void)
{
	_ASSERT(m_pBitmapData!=NULL);
	return abs(((LPBITMAPINFOHEADER)m_pBitmapData)->biHeight);
}

LPBYTE CDib::GetData(void)
{
	_ASSERT(m_pBitmapData!=NULL);
	return (LPBYTE)DibPtr((LPBITMAPINFOHEADER)m_pBitmapData);
}

BOOL CDib::IsTopDown(void)
{
	return m_bTopDown;
}

///////////////////////////////////////////////////////////////////////
// CGDIDib

CGDIDib::CGDIDib()
{
	m_hBitmap=NULL;
	m_pBitmapData=NULL;
}

CGDIDib::~CGDIDib()
{
	Close();
}

BOOL CGDIDib::Open(HWND hWnd, const char *pFileName, BOOL bOpenFromFile)
{
	m_hBitmap=(HBITMAP)::LoadImage(
		(HINSTANCE)::GetWindowLong(hWnd,GWL_HINSTANCE),
		pFileName,
		IMAGE_BITMAP,
		0,0,
		//fuLoad);
		LR_CREATEDIBSECTION|(bOpenFromFile?LR_LOADFROMFILE:0)|LR_DEFAULTSIZE);
	if(m_hBitmap==NULL){
		SetErrors(CMERR_CANT_OPEN_FILE, pFileName);
		return FALSE;
	}

	BITMAP bm;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;	// 24bit�� �ȷ�Ʈ ������ ���� ���̹Ƿ� �ʿ������... Ȯ���� ����

	GetObject(m_hBitmap,sizeof(BITMAP),&bm);

	if(bm.bmHeight>=0)m_bTopDown=FALSE;
	else m_bTopDown=TRUE;

    bi.biSize				= sizeof(BITMAPINFOHEADER);
    bi.biWidth				= bm.bmWidth;
    bi.biHeight				= bm.bmHeight;
    bi.biPlanes				= 1;
    //bi.biBitCount           = bm.bmPlanes * bm.bmBitsPixel;
	bi.biBitCount			= 24;			// 8bit �� 24bit�� �о��. ���� Pal ������ ����.
    bi.biCompression		= BI_RGB;
    bi.biSizeImage			= 0;
    bi.biXPelsPerMeter		= 0;
    bi.biYPelsPerMeter		= 0;
    bi.biClrUsed			= 0;
    bi.biClrImportant		= 0;

// �ȷ�Ʈ ����
#define DibNumColors(lpbi)	((lpbi)->biClrUsed == 0 && (lpbi)->biBitCount <= 8 \
								? (WORD)(1 << (int)(lpbi)->biBitCount) \
								: (WORD)(lpbi)->biClrUsed)

	// BITMAPINFO( BITMAPINFOHEADER+PAL ) �� ũ��
	DWORD nLen  = bi.biSize + DibNumColors(&bi) * sizeof(RGBQUAD);

	lpbi=(LPBITMAPINFOHEADER)new char[nLen];

	*lpbi=bi;

	HDC hDC=GetDC(hWnd);
	GetDIBits(hDC,m_hBitmap,0,bi.biHeight,NULL,(LPBITMAPINFO)lpbi,DIB_RGB_COLORS);
	ReleaseDC(hWnd,hDC);

	bi=*lpbi;

	// ����̹��� biSizeImage�� ä���� �ʴ� ���
	if(bi.biSizeImage==0){
		bi.biSizeImage=(DWORD)WIDTHBYTES(bm.bmWidth*bi.biBitCount)*bm.bmHeight;
		if(bi.biCompression!=BI_RGB)
			bi.biSizeImage=(bi.biSizeImage*3)/2;
	}

	delete[] lpbi;

	nLen=bi.biSize+DibNumColors(&bi)*sizeof(RGBQUAD)+bi.biSizeImage;

	//lpbi=(LPBITMAPINFOHEADER)new char[nLen];
	m_pBitmapData=new BYTE[nLen];

	if(m_pBitmapData==NULL){
		//_RPT0(_CRT_WARN,"Memory Allocation Error");
		SetError(CMERR_OUT_OF_MEMORY);
		return FALSE;
	}

	*(LPBITMAPINFOHEADER)m_pBitmapData=bi;

	hDC=GetDC(hWnd);
	GetDIBits(hDC,m_hBitmap,0,bi.biHeight,DibPtr((LPBITMAPINFOHEADER)m_pBitmapData),(LPBITMAPINFO)m_pBitmapData,DIB_RGB_COLORS);
	ReleaseDC(hWnd,hDC);
	
	return TRUE;
}

BOOL CGDIDib::Open(HINSTANCE hInstance,HWND hWnd, const char *pFileName)
{
	m_hBitmap=(HBITMAP)::LoadImage(
		hInstance,
		pFileName,
		IMAGE_BITMAP,
		0,0,
		LR_CREATEDIBSECTION|LR_DEFAULTSIZE);

	if(m_hBitmap==NULL){
		SetErrors(CMERR_CANT_OPEN_FILE, pFileName);
		return FALSE;
	}

	BITMAP bm;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;	// 24bit�� �ȷ�Ʈ ������ ���� ���̹Ƿ� �ʿ������... Ȯ���� ����

	GetObject(m_hBitmap,sizeof(BITMAP),&bm);

	if(bm.bmHeight>=0)m_bTopDown=FALSE;
	else m_bTopDown=TRUE;

    bi.biSize				= sizeof(BITMAPINFOHEADER);
    bi.biWidth				= bm.bmWidth;
    bi.biHeight				= bm.bmHeight;
    bi.biPlanes				= 1;
    //bi.biBitCount           = bm.bmPlanes * bm.bmBitsPixel;
	bi.biBitCount			= 24;			// 8bit �� 24bit�� �о��. ���� Pal ������ ����.
    bi.biCompression		= BI_RGB;
    bi.biSizeImage			= 0;
    bi.biXPelsPerMeter		= 0;
    bi.biYPelsPerMeter		= 0;
    bi.biClrUsed			= 0;
    bi.biClrImportant		= 0;

// �ȷ�Ʈ ����
#define DibNumColors(lpbi)	((lpbi)->biClrUsed == 0 && (lpbi)->biBitCount <= 8 \
								? (WORD)(1 << (int)(lpbi)->biBitCount) \
								: (WORD)(lpbi)->biClrUsed)

	// BITMAPINFO( BITMAPINFOHEADER+PAL ) �� ũ��
	DWORD nLen  = bi.biSize + DibNumColors(&bi) * sizeof(RGBQUAD);

	lpbi=(LPBITMAPINFOHEADER)new char[nLen];

	*lpbi=bi;

	HDC hDC=GetDC(hWnd);
	GetDIBits(hDC,m_hBitmap,0,bi.biHeight,NULL,(LPBITMAPINFO)lpbi,DIB_RGB_COLORS);
	ReleaseDC(hWnd,hDC);

	bi=*lpbi;

	// ����̹��� biSizeImage�� ä���� �ʴ� ���
	if(bi.biSizeImage==0){
		bi.biSizeImage=(DWORD)WIDTHBYTES(bm.bmWidth*bi.biBitCount)*bm.bmHeight;
		if(bi.biCompression!=BI_RGB)
			bi.biSizeImage=(bi.biSizeImage*3)/2;
	}

	delete[] lpbi;

	nLen=bi.biSize+DibNumColors(&bi)*sizeof(RGBQUAD)+bi.biSizeImage;

	//lpbi=(LPBITMAPINFOHEADER)new char[nLen];
	m_pBitmapData=new BYTE[nLen];

	if(m_pBitmapData==NULL){
		//_RPT0(_CRT_WARN,"Memory Allocation Error");
		SetError(CMERR_OUT_OF_MEMORY);
		return FALSE;
	}

	*(LPBITMAPINFOHEADER)m_pBitmapData=bi;

	hDC=GetDC(hWnd);
	GetDIBits(hDC,m_hBitmap,0,bi.biHeight,DibPtr((LPBITMAPINFOHEADER)m_pBitmapData),(LPBITMAPINFO)m_pBitmapData,DIB_RGB_COLORS);
	ReleaseDC(hWnd,hDC);
	
	return TRUE;
}

void CGDIDib::BitBlt(HDC hDC, int x, int y, int cx, int cy)
{
	if(m_hBitmap==NULL)return;

	HDC hMemDC=::CreateCompatibleDC(hDC);

	HGDIOBJ hOldObj=::SelectObject(hMemDC,m_hBitmap);
	::BitBlt(hDC,x,y,cx,cy,hMemDC,0,0,SRCCOPY);
	::SelectObject(hMemDC,hOldObj);
	::DeleteDC(hMemDC);
}

void CGDIDib::Close(void)
{
	//::CloseHandle(m_hBitmap);
	if(m_pBitmapData!=NULL){
		delete[] m_pBitmapData;
		m_pBitmapData=NULL;
	}
	if(m_hBitmap!=NULL){
		::DeleteObject(m_hBitmap);
		m_hBitmap=NULL;
	}
}

LONG CGDIDib::GetWidth(void)
{
	_ASSERT(m_pBitmapData!=NULL);
	return ((LPBITMAPINFOHEADER)m_pBitmapData)->biWidth;
}

LONG CGDIDib::GetHeight(void)
{
	_ASSERT(m_pBitmapData!=NULL);
	return abs(((LPBITMAPINFOHEADER)m_pBitmapData)->biHeight);
}

LPBYTE CGDIDib::GetData(void)
{
	_ASSERT(m_pBitmapData!=NULL);
	return (LPBYTE)DibPtr((LPBITMAPINFOHEADER)m_pBitmapData);
}
