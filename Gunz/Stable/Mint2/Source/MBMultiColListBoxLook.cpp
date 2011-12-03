#include "stdafx.h"
#include "MBMultiColListBoxLook.h"
#include "MBitmapDrawer.h"

void MBMultiColListBoxLook::OnFrameDraw(MMultiColListBox* pListBox, MDrawContext* pDC)
{
	MRECT r = pListBox->GetInitialClientRect();
	//DrawBitmapFrame9(pDC, r, m_pFrameBitmaps);
	pDC->SetColor(19,19,19,255);
	pDC->FillRectangle(r);
	pDC->SetColor(128,128,128,255);
	pDC->Rectangle(r);
}

MBMultiColListBoxLook::MBMultiColListBoxLook(void)
{
	for(int i=0; i<9; i++){
		m_pFrameBitmaps[i] = NULL;
	}
	m_pFont=NULL;
}

MRECT MBMultiColListBoxLook::GetClientRect(MMultiColListBox* pListBox, MRECT& r)
{
	//int lw = GETWIDTH(m_pFrameBitmaps[3]);
	//int rw = GETWIDTH(m_pFrameBitmaps[5]);
	//int uh = GETHEIGHT(m_pFrameBitmaps[7]);
	//int bh = GETHEIGHT(m_pFrameBitmaps[1]);
	//return MRECT(r.x+lw, r.y+uh, r.w-(lw+rw)-((pListBox->GetScrollBar()->IsVisible()==true)?(pListBox->GetScrollBar()->GetClientRect().w):0), r.h-(uh+bh));
	return MRECT(r.x+3, r.y+3, r.w-(6)-((pListBox->GetScrollBar()->IsVisible()==true)?(pListBox->GetScrollBar()->GetClientRect().w):0), r.h-(6));
}

void MBMultiColListBoxLook::OnDraw(MMultiColListBox* pListBox, MDrawContext* pDC) 
{
	if(m_pFont!=NULL) pDC->SetFont(m_pFont);

	MMultiColListBoxLook::OnDraw(pListBox,pDC);
}
