#ifndef MFRAMEDRAWER_H
#define MFRAMEDRAWER_H

#include "MDrawContext.h"

enum MFDTextStyle{
	MFDTS_NORMAL,
	MFDTS_ACTIVE,
	MFDTS_DISABLE,
};

////////////////////////////////////////////////////
// Frame�� �׸��� ���� Ŭ����
// �̸� ���� ��Ų�� ������ �� �ִ�.
// Default Frame Drawer
class MFrameDrawer{
public:
	virtual void DrawOuterBevel(MDrawContext* pDC, MRECT& r);		// �ٱ��� ���� �׸���
	virtual void DrawInnerBevel(MDrawContext* pDC, MRECT& r);		// ���� ���� �׸���
	virtual void DrawFlatBevel(MDrawContext* pDC, MRECT& r);		// ��� ���� �׸��� ( ���� ��������... )
	virtual void DrawOuterPlane(MDrawContext* pDC, MRECT& r);		// �ٱ��� ��
	virtual void DrawInnerPlane(MDrawContext* pDC, MRECT& r);		// ���� ��
	virtual void DrawFlatPlane(MDrawContext* pDC, MRECT& r);		// ��� ��
	virtual void DrawOuterBorder(MDrawContext* pDC, MRECT& r);		// �ٱ��� ��(Bevel+Plane) �׸���
	virtual void DrawInnerBorder(MDrawContext* pDC, MRECT& r);		// ���� ��(Bevel+Plane) �׸���
	virtual void DrawFlatBorder(MDrawContext* pDC, MRECT& r);		// �Ϲ����� ��� ��(Bevel+Plane) �׸���
	virtual void Text(MDrawContext* pDC, MRECT& r, const char* szText, MAlignmentMode am=MAM_NOTALIGN, MFDTextStyle nTextStyle=MFDTS_NORMAL, bool bHighlight=false);
	virtual void Text(MDrawContext* pDC, MPOINT& p, const char* szText, MFDTextStyle nTextStyle=MFDTS_NORMAL, bool bHighlight=false, MRECT* r=NULL);
	virtual void TextMC(MDrawContext* pDC, MRECT& r, const char* szText, MAlignmentMode am=MAM_NOTALIGN, MFDTextStyle nTextStyle=MFDTS_NORMAL, bool bHighlight=false);
	virtual void TextMC(MDrawContext* pDC, MPOINT& p, const char* szText, MFDTextStyle nTextStyle=MFDTS_NORMAL, bool bHighlight=false, MRECT* r=NULL);
};

#endif