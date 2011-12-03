//
//	24Bit Image Class
//
//											written by lee jang ho
//
//////////////////////////////////////////////////////////////////////
#ifndef _IMAGE24_H
#define _IMAGE24_H

#include "Dib.h"

//	24Bit Image Buffer Class
class CImage24
{
private:
	WORD	m_nWidth;			// Width
	WORD	m_nHeight;			// Height
	LPBYTE	m_pBitmapData;		// Buffer Pointer
public:
	CImage24();
	virtual ~CImage24();

	// ������ ũ��� ���۸� �Ҵ�
	BOOL Open(int nWidth,int nHeight);
	// �ε�� �̹����� �� ������Ʈ�� ������
	BOOL Open(CDib *pDib);
	// ����
	void Close();

	// Open �Ǿ��°�?
	BOOL IsOpen(void);

	// ���� ������ ������ ���
	LPBYTE GetData(void);
	// ����Ÿ �� ũ�� ���
	int GetDataSize(void);
	// BitBlt
	void BitBlt(LPBYTE pDst,WORD nDstX,WORD nDstY,WORD nDstPitch,WORD nSrcX,WORD nSrcY,WORD nSrcWidth,WORD nSrcHeight);
	// BitBlt with Pitch width(byte value)
	void CImage24::BitBltwp(LPBYTE pDst,WORD nDstX,WORD nDstY,WORD nPitchByte,WORD nSrcX,WORD nSrcY,WORD nSrcWidth,WORD nSrcHeight);

	// ���� �� ���
	WORD GetWidth(void);
	// ���� �� ���
	WORD GetHeight(void);

	// ���� ��ġ ���(Byte)
	WORD GetPitch(void);
};

#endif
