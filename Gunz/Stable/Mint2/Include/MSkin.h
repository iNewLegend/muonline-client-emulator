#ifndef MSKIN_H
#define MSKIN_H

#include "MTypes.h"

class MBitmap;
class MDrawContext;

// �� bitmap ���� 9���� �������� ��Ų������ �����͸� ����, �׸��� Ŭ���� 
class MSkin {

public:
	MSkin();

	MBitmap *m_pBitmap;
	MRECT	m_Rect[9];


	void Draw(MDrawContext *pDC, MRECT rt);

};


#endif