#include "stdafx.h"

#include "ZApplication.h"
#include "ZToolTip.h"
#include "Mint4R2.h"

//#include "MTextArea.h"

#define ZTOOLTIP_WIDTH_GAP 10
#define ZTOOLTIP_HEIGHT_GAP 10

#define ZTOOLTIP_MAX_W 350
#define ZTOOLTIP_LINE_GAP 3

ZToolTip::ZToolTip(const char* szName, MWidget* pParent, MAlignmentMode align)
: MToolTip(szName, pParent)
{
	m_alignMode = align;
//	m_pBitmap1 = NULL;
//	m_pBitmap2 = NULL;

//	m_pBitmap1 = MBitmapManager::Get("arrow butten_left.tga");
	m_pBitmap1 = MBitmapManager::Get("tooltip_edge01.png");
	m_pBitmap2 = MBitmapManager::Get("tooltip_edge02.png");

	SetBounds();

//	m_pTextArea = new MTextArea;
//	m_pTextArea->MTextArea( ZTOOLTIP_MAX_W );
}

ZToolTip::~ZToolTip(void)
{

}

/*
bold ���� ���� �۾���~

- ��ư ������ �����̴� ��� ���������? - ������ ����ó��..( Ȥ�� ���� ���������� ������ )
- �����¿� ������ ( 10,10 ) 
- �������� �ʰ�
*/

bool IsToolTipEnable();

void ZToolTip::OnDraw(MDrawContext* pDC)
{
/*
	if(IsToolTipEnable()==false) {
		return;
	}
*/
	MRECT r = GetClientRect();

//	pDC->SetColor(MCOLOR(DEFCOLOR_MTOOLTIP_PLANE));
//	pDC->FillRectangle(r);
//	pDC->SetColor(MCOLOR(DEFCOLOR_MTOOLTIP_OUTLINE));
//	pDC->Rectangle(r);

	// �ּһ����� 32 x 32 -> 16 2������ ���� �� �ֵ���~
	if(m_pBitmap1&&m_pBitmap2) {

		//�ڽ��� ������ �ѷ��δ� �̹����� �׷��ش�..
		// 9�� ȸ���ؼ� �׸���..
		// 1 2 3 
		// 4 5 6
		// 7 8 9

		m_pBitmap1->SetDrawMode(MBM_Normal);
		m_pBitmap2->SetDrawMode(MBM_Normal);

		pDC->SetBitmap( m_pBitmap1 );
		pDC->Draw(r.x, r.y, 16, 16);

		pDC->SetBitmap( m_pBitmap2 );
		pDC->Draw(r.x+16, r.y, r.w-32,16);

		m_pBitmap1->SetDrawMode(MBM_FlipLR);

		pDC->SetBitmap( m_pBitmap1 );
		pDC->Draw(r.x+r.w-16, r.y, 16, 16);

		//�׷����һ�����ִٸ�~ �߰��ܰ�
		if(r.h > 32) {

			m_pBitmap2->SetDrawMode(MBM_RotL90);
			pDC->SetBitmap( m_pBitmap2 );
			pDC->Draw(r.x, r.y+16, 16, r.h-32);

			//�߰���ä���
			pDC->SetColor(MCOLOR(0xffD9D9D9));//�ӽ�
			pDC->FillRectangle(MRECT(r.x+16,r.y+16,r.w-32,r.h-32));

			m_pBitmap2->SetDrawMode(MBM_RotR90);
			pDC->SetBitmap( m_pBitmap2 );
			pDC->Draw(r.x+r.w-16, r.y+16, 16, r.h-32);
		}

		// �Ʒ��κ�~

		m_pBitmap1->SetDrawMode(MBM_FlipUD);
		pDC->SetBitmap( m_pBitmap1 );
		pDC->Draw(r.x, r.y+r.h-16, 16, 16);

		m_pBitmap2->SetDrawMode(MBM_FlipUD);
		pDC->SetBitmap( m_pBitmap2 );
		pDC->Draw(r.x+16, r.y+r.h-16, r.w-32,16);

		m_pBitmap1->SetDrawMode(MBM_FlipUD|MBM_FlipLR);
		pDC->SetBitmap( m_pBitmap1 );
		pDC->Draw(r.x+r.w-16, r.y+r.h-16, 16, 16);

		m_pBitmap1->SetDrawMode(MBM_Normal);
		m_pBitmap2->SetDrawMode(MBM_Normal);
	}

	char* szName = NULL;

	if(m_bUseParentName==true) 
		szName = GetParent()->m_szName;
	else 
		szName = m_szName;

	MRECT text_r = MRECT(r.x+10,r.y+10,r.w-10,r.h-10);

//	pDC->SetColor(MCOLOR(DEFCOLOR_MTOOLTIP_TEXT));//�ӽ�
	pDC->SetColor(MCOLOR(0xff000000));//�ӽ�
//	pDC->TextWithHighlight(text_r, szName, (MAM_HCENTER|MAM_VCENTER));
	pDC->TextMultiLine(text_r, szName,ZTOOLTIP_LINE_GAP);	
}

int GetLineCount(char* str,int& max) {

	if(!str || str[0]==NULL)
		return 0;

	int line = 0;

	max = 0;

	int cnt = (int)strlen(str);
	int back = 0;

	for(int i=0;i<cnt;i++) {
		if(str[i]=='\n') {
			line++;

			max = max((i - back),max); // ������..
			back = i;
		}
	}
	return line;
}

void ZToolTip::SetBounds(void)
{
	MFont* pFont = GetFont();

	char szName[MWIDGET_NAME_LENGTH];

	RemoveAnd(szName, m_bUseParentName==true?GetParent()->m_szName:m_szName);

	int nWidth = pFont->GetWidthWithoutAmpersand(szName);
	int nHeight = pFont->GetHeight();
	int x, y;
	
	GetPosAlignedWithParent(x, y, nWidth, nHeight);
	
	// �ּһ����� 16 size bitmap �ϰ��.. 32x32 ����?
	int w = max(nWidth+ZTOOLTIP_WIDTH_GAP*2,32);
	int h = max(nHeight+ZTOOLTIP_HEIGHT_GAP*2,32);

	if(w) {

//		int line = (w / (ZTOOLTIP_MAX_W-20))+1;
		int _max=0;
		int line = GetLineCount( szName , _max );
		int	_max_w = MAX_TOOLTIP_LINE_STRING*(pFont->GetWidth("b"));
		
		if(line) {
			w = _max_w;
			h = h + ((nHeight + ZTOOLTIP_LINE_GAP) * line);
		}
//		else w = min(w,_max_w);
	}

	MWidget::SetBounds(MRECT(x-ZTOOLTIP_WIDTH_GAP, y-ZTOOLTIP_HEIGHT_GAP,w,h));
}

void ZToolTip::GetPosAlignedWithParent(int& x, int& y, int nTextPixelWidth, int nTextPixelHeight)
{
	/// parameter x,y is return value

	MRECT pr = GetParent()->GetClientRect();

	MRECT tr;	//tooltip rect
	tr.w = nTextPixelWidth+ZTOOLTIP_WIDTH_GAP/2;
	tr.h = nTextPixelHeight+ZTOOLTIP_HEIGHT_GAP;

	if (m_alignMode & MAM_LEFT)
		tr.x = pr.x+(ZTOOLTIP_WIDTH_GAP/2+1);
	else if (m_alignMode & MAM_RIGHT)
		tr.x = pr.x+pr.w - tr.w;
	else if (m_alignMode & MAM_HCENTER)
		tr.x = pr.w/2 - tr.w/2;
	else
		tr.x = 0;
	

	if (m_alignMode & MAM_TOP)
		tr.y = pr.y-tr.h;
	else if (m_alignMode & MAM_BOTTOM)
		tr.y = pr.y+pr.h;
	else if (m_alignMode & MAM_VCENTER)
		tr.y = pr.h/2 - tr.h/2;
	else
		tr.y = 0;

	MRECT str = MClientToScreen(GetParent(), tr);
	
	int rightx = str.x+str.w;
	if (rightx > MGetWorkspaceWidth()) {
		int diff = rightx - MGetWorkspaceWidth();
		tr.x -= diff;
	}
	int leftx = str.x;
	if (leftx < 0)
		tr.x = 0;

	int bottomy = str.y+str.h;
	if (bottomy > MGetWorkspaceHeight()) {
		int diff = bottomy - MGetWorkspaceHeight();
		tr.y -= diff;
	}

	int topy = str.y;
	if (topy < 0)
		tr.y = 0;

	x = tr.x;
	y = tr.y;
}
