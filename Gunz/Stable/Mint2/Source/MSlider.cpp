#include "stdafx.h"
#include "MSlider.h"
#include "MColorTable.h"


void MSliderThumbLook::OnDraw(MSliderThumb* pThumb, MDrawContext* pDC)
{
	pDC->SetColor(MCOLOR(DEFCOLOR_MBUTTON_LIGHTPLANE));
	pDC->FillRectangle(pThumb->GetClientRect());
}

MRECT MSliderThumbLook::GetClientRect(MSliderThumb* pThumb, MRECT& r)
{
	return r;
}

MSIZE MSliderThumbLook::GetDefaultSize(MSliderThumb* pThumb)
{
	return MSIZE(MSCROLLBAR_DEFAULT_WIDTH, MSCROLLBAR_DEFAULT_WIDTH);
}


IMPLEMENT_LOOK(MSliderThumb, MSliderThumbLook)

MSliderThumb::MSliderThumb(const char* szName, MWidget* pParent, MListener* pListener)
: MThumb(szName, pParent, pListener)
{
	LOOK_IN_CONSTRUCTOR()
}

MSIZE MSliderThumb::GetDefaultSize(void)
{
	if(GetLook()!=NULL) return GetLook()->GetDefaultSize(this);
	return MSIZE(MSCROLLBAR_DEFAULT_WIDTH, MSCROLLBAR_DEFAULT_WIDTH);
}


IMPLEMENT_LOOK(MSlider, MScrollBarLook)

int MSlider::GetThumbSize(void)
{
	//MSIZE s = ((MSliderThumb*)m_pThumb)->GetDefaultSize();
	//return s.w;
	int narrowWidth = (m_nScrollBarType==MSBT_VERTICAL) ? GetClientRect().w : GetClientRect().h;
	return narrowWidth;
}

void MSlider::Initialize(void)
{
	delete m_pThumb;
	m_pThumb = new MSliderThumb(NULL, this, this);
	MSIZE s = ((MSliderThumb*)m_pThumb)->GetDefaultSize();
	m_pThumb->SetSize(s.w, s.h);
	m_pThumb->m_nDirection = MSBT_HORIZONTAL;
	LOOK_IN_CONSTRUCTOR()
}

MSlider::MSlider(const char* szName, MWidget* pParent, MListener* pListener)
: MScrollBar(szName, pParent, pListener, MSBT_HORIZONTAL)
{
	Initialize();
}

MSlider::MSlider(MWidget* pParent, MListener* pListener)
: MScrollBar(pParent, pListener, MSBT_HORIZONTAL)
{
	Initialize();
}

MSlider::~MSlider(void)
{
}
