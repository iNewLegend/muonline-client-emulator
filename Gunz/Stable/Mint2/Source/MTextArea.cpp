#include "stdafx.h"
#include "MTextArea.h"
#include "MColorTable.h"
#include "MScrollBar.h"
#include "MEdit.h"	// blinktime����
#include "Mint.h"

#define MTEXTAREA_DEFAULT_WIDTH				100			// Default MTextArea Widget Width, Height�� Font Height�� ���� �ʱ� �����ȴ�.

IMPLEMENT_LOOK(MTextArea, MTextAreaLook)


// nPos��° ���ڰ� ��� ��ġ�� �׷����� �ϴ��� pOut���� �������ش�
// x�� �ȼ��� , y �� ���μ�,bFirst�� ĳ���ϰ�� ��/���÷���
bool MTextArea::GetCaretPosition(MPOINT *pOut,int nSize,const char* szText,int nPos,bool bFirst)
{
	MFont *pFont=GetFont();

	int nLine = 0;
	int nLength = strlen(szText);

	bool bResult = false;

	int nCurPos=0;
	do {	// ���δ� ����
		int nIndentation = (nLine==0 ? 0 : m_nIndentation);
		int nOriginalCharCount = MMGetNextLinePos(pFont,szText+nCurPos,GetClientWidth()-nIndentation,m_bWordWrap,m_bColorSupport);
		if(nOriginalCharCount==-1) return false;

		for(int i=0; i<nSize && nPos+i<=nLength; i++){
			// ���� ���ξȿ� ���� ����(nPos~nPos+nSize)�� ������ ��ġ ��� ��� ����
			if(nPos+i>=nCurPos && nPos+i<=nCurPos+nOriginalCharCount){
				pOut[i].x = MMGetWidth(pFont, szText+nCurPos, nPos+i-nCurPos, m_bColorSupport) + nIndentation;
				pOut[i].y = nLine;
				bResult = true;
			}
		}

		nCurPos+=nOriginalCharCount;
		nLine++;
	} while(nCurPos<nLength);

	return bResult;
}

/*
// nPos��° ���ڰ� ��� ��ġ�� �׷����� �ϴ��� pOut���� �������ش�
// x�� �ȼ��� , y �� ���μ�,bFirst�� ĳ���ϰ�� ��/���÷���
bool MTextArea::GetCaretPosition(MPOINT *pOut,int nSize,const char* szText,int nPos,bool bFirst)
{
	MFont *pFont=GetFont();

	int nLine = 0;
	int nLength = strlen(szText);

	int nCurPos=0;
	do {
		int nIndentation = (nLine==0 ? 0 : m_nIndentation);
		int nOriginalCharCount = MMGetNextLinePos(pFont,szText+nCurPos,GetClientWidth()-nIndentation,m_bWordWrap,m_bColorSupport);
		if(nOriginalCharCount==-1) return false;

		if(nCurPos+nOriginalCharCount==nPos && nPos<nLength) {
			// �������� �ǳ� �Ǵ� �������� �� ��.
			if(bFirst) {
				pOut->x=MMGetWidth(pFont,szText+nCurPos,nPos-nCurPos,m_bColorSupport)+nIndentation;
				pOut->y=nLine;
			}else {
				pOut->x=m_nIndentation;
				pOut->y=nLine+1;
			}
			return true;
		}

		// �̹��࿡ �Ѿ��
		if(nCurPos+nOriginalCharCount>=nPos) {
			pOut->x=MMGetWidth(pFont,szText+nCurPos,nPos-nCurPos,m_bColorSupport)+nIndentation;
			pOut->y=nLine;
			return true;
		}
		nCurPos+=nOriginalCharCount;
		nLine++;
	} while(nCurPos<nLength);

	// �̰��� �Ͼ�� �ȵȴ�
	//	_ASSERT(FALSE);
	return false;
}
*/
// MDrawTextMultiline���� �׸��� nLine �࿡ nX�ȼ� ��ġ�� ���° ���ڰ� ������ �˷��ִ� ���
int MTextArea::GetCharPosition(const char* szText,int nX,int nLine)
{

	MFont *pFont = GetFont();

	int nCur= MMGetLinePos(pFont,szText,GetClientWidth(),m_bWordWrap,m_bColorSupport,nLine,m_nIndentation);
	
	int nRealX =  (nLine>0) ? nX-m_nIndentation : nX;
	int x = MMGetNextLinePos(pFont,szText+nCur,nRealX,m_bWordWrap,m_bColorSupport);
	return nCur+x;
}

// ���� ĳ���� ��ġ�� �� ���ξȿ��� �����ٷ� wrap������ �Ѿ���
// ���� �������̳� ������ ù��°�� ��ǻ� ���� ��ġ�̴�. �̷�������� �˾ƺ���
bool MTextArea::IsDoubleCaretPos()
{
	const char *szText = m_CurrentLine->text.c_str();
	int nPos = m_CaretPos.x;

	int nLine = 0;
	int nLength = strlen(szText);

	if(nPos==nLength) return false;

	int nCurPos=0;
	do {
		int nRealWidth = (nLine==0 ? GetClientWidth() : GetClientWidth()-m_nIndentation);
		int nOriginalCharCount = MMGetNextLinePos(GetFont(),szText+nCurPos,nRealWidth,m_bWordWrap,m_bColorSupport);
		if(nCurPos+nOriginalCharCount==nPos) {
			return true;
		}

		nCurPos+=nOriginalCharCount;
		nLine++;
	} while(nCurPos<nLength);

	return false;
}

void MTextArea::ScrollDown()
{
	const char *szText = GetTextLine(m_nStartLine);
	int nLine = MMGetLineCount(GetFont(),szText,GetClientWidth(),m_bWordWrap,m_bColorSupport,m_nIndentation);
	if(m_nStartLineSkipLine+1 < nLine) {
		m_nStartLineSkipLine++;
		return;
	}

	if(GetLineCount()>m_nStartLine+1)
	{
		m_nStartLine++;
		m_nStartLineSkipLine=0;
	}
}

void MTextArea::ScrollUp()
{
	if(m_nStartLineSkipLine > 0 ) {
		m_nStartLineSkipLine--;
		return;
	}

	if(m_nStartLine>0)
	{
		m_nStartLine--;

		const char *szText = GetTextLine(m_nStartLine);
		int nLine = MMGetLineCount(GetFont(),szText,GetClientWidth(),m_bWordWrap,m_bColorSupport,m_nIndentation);
		m_nStartLineSkipLine=nLine-1;
	}
}

// ctrl+home , end ��������. ���� ������ �ȵǾ���.
void MTextArea::MoveFirst()
{
}

void MTextArea::MoveLast()
{
}

int MTextArea::GetTotalLineCount(int& nStartLine, int& nCurrentLine)
{
	int nTotalLine = 0;
	MLINELISTITERATOR itr = m_Lines.begin();
	for(int i=0;i<GetLineCount();i++)
	{
		MRECT rectScrollBar;
		rectScrollBar = m_pScrollBar->GetRect();
		int nCntWidth = GetClientWidth() - ( (IsScrollBarVisible()) ? rectScrollBar.w : 0);

		const char *szText = itr->text.c_str();
		int nLine = MMGetLineCount(GetFont(),szText,nCntWidth,m_bWordWrap,m_bColorSupport,m_nIndentation);
		if(nLine==-1) 
		{
			int nLine = MMGetLineCount(GetFont(),szText,nCntWidth,m_bWordWrap,m_bColorSupport,m_nIndentation);
			return 0;	// ���� ���Ƽ� �����Ѱ��� �ǹ̰� ����
		}

		if(i==GetStartLine()) {
			nStartLine=nTotalLine+m_nStartLineSkipLine;
		}

		if(itr==m_CurrentLine) {
			MPOINT carpos;
			if(GetCaretPosition(&carpos,1,szText,GetCaretPos().x,m_bCaretFirst))
			{
				nCurrentLine = nTotalLine+carpos.y;
			}
		}

		nTotalLine += nLine;
		itr++;
	}
	return nTotalLine;
}

void MTextArea::UpdateScrollBar(bool bAdjustStart)
{	
	MFont *pFont=GetFont();
	MRECT r=GetClientRect();

	int nStartLine = 0;
	int nCurrentLine = 0;

	int nTotalLine = GetTotalLineCount(nStartLine, nCurrentLine);

	// ��ũ�ѹٸ� �巡�� �ϰ� �������� ��ũ������ �ʴ´�
	if(bAdjustStart && MWidget::m_pCapturedWidget!=m_pScrollBar)
	{
//		int nVisibleCount = (r.h-1) / GetLineHeight( GetFont()->GetHeight()) + 1;
		int nVisibleCount = r.h / GetLineHeight() ;
		
		
		if(nCurrentLine>=nStartLine+nVisibleCount) {
			// ȭ�� �Ʒ��ʿ� ĳ���� �����Ƿ� ĳ���� �� �Ʒ��ٿ� ������ �����Ѵ�
            int nCount = nCurrentLine-(nStartLine+nVisibleCount)+1;
			for(int j=0;j<nCount;j++)
			{
				ScrollDown();
			}
			nStartLine+=nCount;
		}

		if(nCurrentLine<nStartLine) {
			// ȭ�� ���ʿ� ĳ���� �ִ�.
            int nCount = nStartLine-nCurrentLine;
			for(int j=0;j<nCount;j++)
			{
				ScrollUp();
			}
			nStartLine-=nCount;
		}
	}

	int nPosLines = nTotalLine - r.h / GetLineHeight();
	int nMax=max(nPosLines,0);
	m_pScrollBar->SetMinMax(0,nMax);
	
	if(m_nStartLine>nMax)
		m_nStartLine=nMax;

	m_pScrollBar->SetValue(nStartLine);
	bool bShow= m_bScrollBarEnable && 
		(r.h<int(GetLineHeight() *nTotalLine) || m_nStartLine!=0);
	m_pScrollBar->Show(bShow,false);
}

void MTextArea::OnScrollBarChanged(int nPos)
{
	MFont *pFont=GetFont();
	MRECT r=GetClientRect();

	int nStartLine = 0;
	int nCurrentLine = 0;

	int nTotalLine = 0;
	MLINELISTITERATOR itr = m_Lines.begin();
	for(int i=0;i<GetLineCount();i++)
	{
		const char *szText = itr->text.c_str();
		int nLine = MMGetLineCount(GetFont(),szText,GetClientWidth(),m_bWordWrap,m_bColorSupport,m_nIndentation);
		if(nLine==-1) 
			return;	// ���� ���Ƽ� �����Ѱ��� �ǹ̰� ����

		if(nTotalLine<=nPos && nPos<nTotalLine+nLine) {
			m_nStartLine = i;
			m_nStartLineSkipLine = nPos-nTotalLine;
			return;
		}

		nTotalLine += nLine;
		itr++;
	}
}

bool MTextArea::OnCommand(MWidget* pWindow, const char* szMessage)
{
	if(pWindow==m_pScrollBar && strcmp(szMessage, MLIST_VALUE_CHANGED)==0){
		OnScrollBarChanged(m_pScrollBar->GetValue());
		return true;
	}
	return false;
}

bool MTextArea::MoveLeft(bool bBackspace)		// ���������� return true. �齺���̽��� ���� ����
{
	if(IsDoubleCaretPos() && m_bCaretFirst==false && !bBackspace)
	{
		m_bCaretFirst=true;
		return true;		
	}

	if(m_CaretPos.x>1 && IsHangul(*(m_CurrentLine->text.begin()+m_CaretPos.x-1)))
		m_CaretPos.x-=2;
	else
	if(m_CaretPos.x>0)
		m_CaretPos.x--;
	else
	if(m_CaretPos.y>0)
	{
		m_CaretPos.y--;
		m_CurrentLine--;
		m_CaretPos.x=m_CurrentLine->text.size();
		UpdateScrollBar(true);
	}
	else
	{
//		�Ǿ��̴�.
		return false;
	}

	return true;
}

void MTextArea::MoveRight()
{
	if(IsDoubleCaretPos() && m_bCaretFirst)
	{
		m_bCaretFirst=false;
		return;
	}

	if((int)m_CurrentLine->text.size()>m_CaretPos.x+1 && IsHangul(*(m_CurrentLine->text.begin()+m_CaretPos.x)))
		m_CaretPos.x+=2;
	else
	if((int)m_CurrentLine->text.size()>m_CaretPos.x)
//		m_CaretPos.x=min((int)m_CurrentLine->size(),m_CaretPos.x+1);
		m_CaretPos.x++;
	else
	if(m_CaretPos.y+1<(int)m_Lines.size())
	{
		m_CaretPos.y++;
		m_CurrentLine++;
		m_CaretPos.x=0;
		UpdateScrollBar(true);
	}
}

void MTextArea::MoveUp()
{
	// ���� ���ξȿ��� �����̸� �Ǵ°� ���� ����
	if(GetCaretPos().x==m_CurrentLine->text.size()) 
		m_bCaretFirst=true;

	const char *szCurrent = m_CurrentLine->text.c_str();

	MPOINT carpos;
	if(!GetCaretPosition(&carpos,1,szCurrent,GetCaretPos().x,m_bCaretFirst))
		return;

	carpos.x++;
	if(carpos.y>0) {
		// ���� ���ξȿ��� �����̸� �ȴ�

		if(!m_bVerticalMoving)
		{
			m_bVerticalMoving=true;
			m_nVerticalMoveAxis=carpos.x;
		}

		m_CaretPos.x=GetCharPosition(szCurrent,m_nVerticalMoveAxis,carpos.y-1);
		UpdateScrollBar(true);
		return ;
	}

	// �� �������� �ö󰣴�
	if(m_CaretPos.y>0)
	{
		MFont *pFont=GetFont();
		if(!m_bVerticalMoving)
		{
			m_bVerticalMoving=true;
			m_nVerticalMoveAxis=carpos.x;
		}
		m_CurrentLine--;
		m_CaretPos.y--;

		szCurrent = m_CurrentLine->text.c_str();
		int nCurrentLineLineCount = MMGetLineCount(GetFont(),szCurrent,GetClientWidth(),m_bWordWrap,m_bColorSupport,m_nIndentation );
		m_CaretPos.x=GetCharPosition(szCurrent,m_nVerticalMoveAxis,nCurrentLineLineCount-1);
		UpdateScrollBar(true);
	}
}

void MTextArea::MoveDown()
{
	// ���� ���ξȿ��� �����̸� �Ǵ°� ���� ����

	if(GetCaretPos().x==0) 
		m_bCaretFirst=false;

	const char *szCurrent = m_CurrentLine->text.c_str();

	int nCurrentLineLineCount = MMGetLineCount(GetFont(),szCurrent,GetClientWidth(),m_bWordWrap,m_bColorSupport,m_nIndentation );

	MPOINT carpos;

	if(!GetCaretPosition(&carpos, 1, szCurrent, GetCaretPos().x,m_bCaretFirst))
		return;

	carpos.x++;

	if(carpos.y+1<nCurrentLineLineCount) {
		// ���� ���ξȿ��� �����̸� �ȴ�

		if(!m_bVerticalMoving)
		{
			m_bVerticalMoving=true;
			m_nVerticalMoveAxis=carpos.x;
		}

		m_CaretPos.x=GetCharPosition(szCurrent,m_nVerticalMoveAxis,carpos.y+1);
		UpdateScrollBar(true);
		return ;
	}

	// ���� �������� �Ѿ��
	if(m_CaretPos.y+1<(int)m_Lines.size())
	{
		MFont *pFont=GetFont();
		if(!m_bVerticalMoving)
		{
			m_bVerticalMoving=true;
			m_nVerticalMoveAxis=carpos.x;
		}
		m_CurrentLine++;
		m_CaretPos.y++;

		szCurrent = m_CurrentLine->text.c_str();
		m_CaretPos.x=GetCharPosition(szCurrent,m_nVerticalMoveAxis,0);
		UpdateScrollBar(true);
	}
}

void MTextArea::OnHome()
{
	const char *szCurrent = m_CurrentLine->text.c_str();

	MPOINT carpos;

	if(!GetCaretPosition(&carpos, 1, szCurrent, GetCaretPos().x, m_bCaretFirst))
		return;

	m_CaretPos.x=GetCharPosition(szCurrent,0,carpos.y);
	m_bCaretFirst = false;
}

void MTextArea::OnEnd()
{
	const char *szCurrent = m_CurrentLine->text.c_str();

	MPOINT carpos;

	if(!GetCaretPosition(&carpos, 1, szCurrent, GetCaretPos().x, m_bCaretFirst))
		return;

	int nNextLinePos = MMGetLinePos(GetFont(),szCurrent,GetClientWidth(),m_bWordWrap,m_bColorSupport,carpos.y+1,m_nIndentation);
	m_CaretPos.x = nNextLinePos;
	m_bCaretFirst = true;
}

void MTextArea::DeleteCurrent()
{
	if(m_CaretPos.x+1<(int)m_CurrentLine->text.size() && IsHangul(*(m_CurrentLine->text.begin()+m_CaretPos.x)))		// �ѱ� 
	{
		m_CurrentLine->text.erase(m_CurrentLine->text.begin()+m_CaretPos.x,m_CurrentLine->text.begin()+m_CaretPos.x+2); 
		m_nCurrentSize-=2;
	}
	else
	if(m_CaretPos.x<(int)m_CurrentLine->text.size())																// ����
	{
		m_CurrentLine->text.erase(m_CurrentLine->text.begin()+m_CaretPos.x);
		m_nCurrentSize--;
	}
	else
	if(m_CaretPos.y+1<(int)m_Lines.size())																	// �� �� ��
	{
		MLINELISTITERATOR willbedel=m_CurrentLine;
		willbedel++;
		m_CurrentLine->text+=willbedel->text;
		m_Lines.erase(willbedel);
		UpdateScrollBar();
	}
}

bool MTextArea::OnLButtonDown(MPOINT pos)
{
	MRECT r = GetClientRect();

	pos.x-=r.x;
	pos.y-=r.y;
	
	int y = 0;

	m_CurrentLine = GetIterator(GetStartLine());
	for(int i=GetStartLine();i<GetLineCount();i++)
	{
		const char *szText = m_CurrentLine->text.c_str();
		int nLine = MMGetLineCount(GetFont(),szText,GetClientWidth(),m_bWordWrap,m_bColorSupport,m_nIndentation);
		if(pos.y <= y + nLine*GetLineHeight() ||	// �� ���� Ŭ���߰ų�
			i==GetLineCount()-1) {	// ���������̰ų�
			int n = min(nLine-1,(pos.y - y) / GetLineHeight());

			m_CaretPos.x=GetCharPosition(szText,pos.x,n);
			m_CaretPos.y=i;
			return true;
		}

		y+= nLine * GetLineHeight();
		m_CurrentLine++;
	}

	// �̰�찡 ����� �ȵ�
	_ASSERT(FALSE);
	return false;
}

bool MTextArea::OnEvent(MEvent* pEvent, MListener* pListener)
{
	if(MWidget::m_pFocusedWidget!=this) return false;
	MRECT r = GetClientRect();
	switch(pEvent->nMessage){

	case MWM_KEYDOWN:
		if (m_bEditable)
		{
			bool bResult = InputFilterKey(pEvent->nKey,pEvent->bCtrl);
			UpdateScrollBar(true);
			return bResult;
		}
		break;
	case MWM_CHAR:
		
		if(IsFocus() && m_bEditable && GetLength()<GetMaxLen() && InputFilterChar(pEvent->nKey)==false){
			m_CurrentLine->text.insert(m_CurrentLine->text.begin()+m_CaretPos.x,(char)pEvent->nKey);
			m_CaretPos.x++;
			m_nCurrentSize++;
			UpdateScrollBar(true);
			return true;
		}
		break;

	case MWM_IMECOMPOSE:
		strcpy(m_szIMECompositionString, pEvent->szIMECompositionString);
		if(IsFocus() && m_bEditable && GetLength()+1<GetMaxLen() && pEvent->szIMECompositionResultString[0]!=NULL){
			int length=strlen(pEvent->szIMECompositionResultString);
			m_CurrentLine->text.insert(
				m_CurrentLine->text.begin()+m_CaretPos.x,
				pEvent->szIMECompositionResultString,
				pEvent->szIMECompositionResultString+length);
			m_CaretPos.x+=length;
			m_nCurrentSize+=length;
			UpdateScrollBar(true);
			return true;
		}
		break;

	case MWM_MOUSEWHEEL:
		if(r.InPoint(pEvent->Pos)==false) break;

		/*
#define MAX_WHEEL_RANGE	2

		// 
		m_nStartLine=m_nStartLine+min(max(-pEvent->nDelta, -MAX_WHEEL_RANGE), MAX_WHEEL_RANGE);
		m_nStartLine=max(0,min(m_nStartLine,(int)m_Lines.size()-1));
		UpdateScrollBar();
		*/

		if(pEvent->nDelta<0)
		{
			ScrollDown();
			ScrollDown();
		}else
		{
			ScrollUp();
			ScrollUp();
		}
		UpdateScrollBar();


		return false; // �ٸ� �����쵵 ���̾���~

	case MWM_LBUTTONDOWN: 	
		// ���콺 Ŭ������ ĳ�� �����Ŵ�
		{
			MRECT r = GetClientRect();
			if(r.InPoint(pEvent->Pos)==true){
				return OnLButtonDown(pEvent->Pos);
			}
		}
	}

	return false;
}

void MTextArea::OnSetFocus(void)
{
	Mint::GetInstance()->EnableIME(true);
}

void MTextArea::OnReleaseFocus(void)
{
	Mint::GetInstance()->EnableIME(false);
}

bool MTextArea::InputFilterKey(int nKey,bool bCtrl)
{
	if(nKey!=VK_UP && nKey!=VK_DOWN)
		m_bVerticalMoving=false;

	switch(nKey){
	case VK_LEFT : MoveLeft();return true;
	case VK_RIGHT : MoveRight();return true;
	case VK_UP : if(bCtrl) ScrollUp(); else MoveUp(); return true;
	case VK_DOWN : if(bCtrl) ScrollDown(); else MoveDown(); return true;
	case VK_HOME : OnHome();return true;
	case VK_END	: OnEnd();return true;

	case VK_DELETE : DeleteCurrent();return true;
	case VK_BACK : if(MoveLeft(true)) DeleteCurrent();return true;

	case VK_RETURN :
		if(GetLength()<GetMaxLen()){
			MLineItem newline(m_CurrentLine->color,string(m_CurrentLine->text.begin()+m_CaretPos.x,m_CurrentLine->text.end()));
			m_CurrentLine->text.erase(m_CaretPos.x,m_CurrentLine->text.size());
			m_CurrentLine++;
			m_CurrentLine=m_Lines.insert(m_CurrentLine,newline);
			m_CaretPos.y++;
			m_CaretPos.x=0;
			UpdateScrollBar(true);
			return true;
		}
		break;
	case VK_TAB :
		return true;
	}
	return false;
}

bool MTextArea::InputFilterChar(int nKey)
{
	if(nKey==VK_BACK){
		return true;
	}
	else if(nKey==VK_ESCAPE){
		//ReleaseFocus();
		MListener* pListener = GetListener();
		if(pListener!=NULL) pListener->OnCommand(this, MTEXTAREA_ESC_VALUE);
		return true;
	}
	else if(nKey==22){	// Ctrl+'V'
		return true;
	}
	else if(nKey==3){	// Ctrl+'C'
		return true;
	}

	switch(nKey){
	case VK_TAB:
	case VK_RETURN:
		return true;
	}
	return false;
}

void MTextArea::SetMaxLen(int nMaxLen)
{
//	if(m_pBuffer) delete m_pBuffer;
	m_Lines.erase(m_Lines.begin(),m_Lines.end());

	m_nMaxLen = nMaxLen;
//	m_pBuffer = new char[nMaxLen];
//	m_pBuffer[0] = NULL;

	m_nStartLine=0;
	m_nStartLineSkipLine=0;
	m_nCurrentSize=0;

	m_CaretPos=MPOINT(0,0);

	m_Lines.push_back(MLineItem(MTEXTAREA_DEFAULT_TEXT_COLOR,string()));
	m_CurrentLine=m_Lines.begin();
}

MTextArea::MTextArea(int nMaxLen, const char* szName, MWidget* pParent, MListener* pListener)
 : MWidget(szName, pParent, pListener) , m_TextOffset(0, 0)
{
	LOOK_IN_CONSTRUCTOR()

	m_bScrollBarEnable = true;
	m_bWordWrap = true;
	m_bColorSupport = true;
	m_bMouseOver = false;
	m_bVerticalMoving = false;
	m_nIndentation = 0;

	MFont* pFont = GetFont();
	m_nCustomLineHeight = -1;
	int w = MTEXTAREA_DEFAULT_WIDTH;
	int h = GetLineHeight()+2;
	SetTextOffset(MPOINT(1, 1));

	m_TextColor = MCOLOR(DEFCOLOR_MEDIT_TEXT); 

	m_szIMECompositionString[0] = NULL;
	m_bEditable = true;
	
	m_pScrollBar = new MScrollBar(this, this);
	m_pScrollBar->Show(false,false);

	SetFocusEnable(true);

//	m_pBuffer=NULL;
	SetMaxLen(nMaxLen);
}

void MTextArea::SetTextOffset(MPOINT p)
{
	m_TextOffset = p;
}

void MTextArea::SetTextColor(MCOLOR color)
{
	m_TextColor = color;
}

MCOLOR MTextArea::GetTextColor(void)
{
	return m_TextColor;
}

MTextArea::~MTextArea()
{
/*	if(m_pBuffer){
		delete []m_pBuffer;
		m_pBuffer = NULL;
	}*/
	delete m_pScrollBar;
}

bool MTextArea::GetText(char *pBuffer,int nBufferSize)
{
	if(GetLength()>nBufferSize) return false;

	int nSize=0;
	char *temp=pBuffer;
	temp[0]=0;
	MLINELISTITERATOR i;
	for(i=m_Lines.begin();i!=m_Lines.end();i++)
	{
		strcpy(temp,i->text.c_str());temp+=i->text.size();
		strcat(temp,"\n");temp++;
		nSize+=i->text.length()+1;
	}
	temp--;*temp=0;
	_ASSERT(GetLength()==nSize);

	return true;
}

const char* MTextArea::GetTextLine(int nLine)
{
	if(nLine>=(int)m_Lines.size()) return NULL;
	MLINELISTITERATOR i=m_Lines.begin();
	for(int j=0;j<nLine;j++,i++);
	return i->text.c_str();
}

MLINELISTITERATOR MTextArea::GetIterator(int nLine)
{
	if(nLine>=(int)m_Lines.size()) return m_Lines.end();
	MLINELISTITERATOR i=m_Lines.begin();
	for(int j=0;j<nLine;j++,i++);
	return i;
}


void MTextArea::SetText(const char *szText)
{
	m_nCurrentSize=0;

	m_Lines.erase(m_Lines.begin(),m_Lines.end());
	int nLength=strlen(szText);
	string text=string(szText,szText+nLength);
	int nStart=0,nNext;
	while(nStart<nLength)
	{
		nNext=text.find(10,nStart);
		if(nNext==-1) nNext=nLength;
		m_Lines.push_back(MLineItem(m_TextColor,text.substr(nStart,nNext-nStart)));
		m_nCurrentSize+=nNext-nStart;
		nStart=nNext+1;
	}

	m_nStartLine=0;
	m_nStartLineSkipLine=0;
	m_CaretPos=MPOINT(0,0);

	if(!m_Lines.size())
		m_Lines.push_back(MLineItem(m_TextColor,string()));
	m_CurrentLine=m_Lines.begin();

	UpdateScrollBar();
}

void MTextArea::Clear()
{
	SetText("");
}

void MTextArea::OnSize(int w, int h)
{
	// TODO: �ѱ��� ���� �� ���Ϸ� ��������Ǹ� ����ϴ�. �Ϲ��� application ���� �����̰� �Ǿ�����.

	MRECT cr = GetClientRect();
	if(m_pScrollBar->IsVisible()==true)
		m_pScrollBar->SetBounds(MRECT(cr.x+cr.w-m_pScrollBar->GetDefaultBreadth(), cr.y+1, m_pScrollBar->GetDefaultBreadth(), cr.h-1));
	else	// �Ⱥ��̴� ��� Ŭ���̾�Ʈ ������ ��ũ�ѹ� �������� �����Ƿ�, �����ؼ� ���
		m_pScrollBar->SetBounds(MRECT(cr.x+cr.w-m_pScrollBar->GetDefaultBreadth(), cr.y+1, m_pScrollBar->GetDefaultBreadth(), cr.h-1));
	
	if(m_bWordWrap)
		UpdateScrollBar();
}

void MTextAreaLook::OnFrameDraw(MTextArea* pTextArea, MDrawContext* pDC)
{
//	MRECT r = pTextArea->GetInitialClientRect();
//	pDC->SetColor(MCOLOR(DEFCOLOR_MEDIT_PLANE));
//	pDC->FillRectangle(r);
//	pDC->SetColor(MCOLOR(DEFCOLOR_MEDIT_OUTLINE));
//	pDC->Rectangle(r);

	if (m_bgColor.a != 0)
	{
		MRECT r = pTextArea->GetInitialClientRect();
		pDC->SetColor(m_bgColor);
		pDC->FillRectangle(r);
	}
}

const char* MTextArea::GetCompositionString(void)
{
	return m_szIMECompositionString;
}

int MTextArea::GetClientWidth()
{
	return GetClientRect().w;
}


// TODO ����� ���̴�
#pragma optimize( "", off )

void MTextAreaLook::OnTextDraw_WordWrap(MTextArea* pTextArea, MDrawContext* pDC)
{
	bool bColorSupport = true;

	MFont *pFont=pDC->GetFont();

	MRECT r = pTextArea->GetClientRect();

	// ���� 0�ȼ����϶�� �׸��ʿ䰡 ����.
	if(r.w<1 || r.h<1) return;

	pDC->SetClipRect(MClientToScreen(pTextArea,r));

	MRECT textrt=r;
	textrt.x+=2;	// default margin
	textrt.y+=2;
	MRECT rectScrollBar;
	rectScrollBar = pTextArea->m_pScrollBar->GetRect();
	textrt.w-= (pTextArea->IsScrollBarVisible()) ? rectScrollBar.w : 0;

	pDC->SetColor(pTextArea->GetTextColor());

	int nCarY = -1;
	MPOINT pt=pTextArea->GetCaretPos();

	bool bCurrentLine;
	int nStartLine = pTextArea->GetStartLine();
	int nTotalLineCount = pTextArea->GetLineCount();

	int i = nStartLine;

	// ����׿�
	char szText[1024]="";
	int nStartLineSkipLine = pTextArea->m_nStartLineSkipLine;
	int nIndentation = pTextArea->m_nIndentation;
	
	MLINELISTITERATOR itor=pTextArea->GetIterator(pTextArea->GetStartLine());
	
	for(;itor!=pTextArea->m_Lines.end();itor++)
	{
		MCOLOR color = itor->color; 

		pDC->SetColor(color);

		textrt.h=(r.y+r.h)-textrt.y;

		int nLine=0;

		string text;

		bCurrentLine = (i==pTextArea->GetCaretPos().y);

		// ���� �����ϰ��ִ� (�ѱ�) ���ڸ� ĳ���ڿ� �����־� �����ش�
		if(bCurrentLine)
		{
			text=string(pTextArea->GetTextLine(i));

			text.insert(pTextArea->GetCaretPos().x,pTextArea->GetCompositionString());
			strcpy(szText,text.c_str());
		}
		else {
			// Ȥ�� �̷�����ΰ� �׽�Ʈ�غ���.
			if(itor->text.length()>sizeof(szText))
			{
				strncpy(szText,itor->text.c_str(),sizeof(szText)-2);
				szText[sizeof(szText)-1]=0;
			}
			else
				strcpy(szText,itor->text.c_str());
		}

		// ù���̸� skip count ��ŭ �Ѱ��ش�
		int nSkipLine = (i==pTextArea->GetStartLine()) ? pTextArea->m_nStartLineSkipLine : 0;

		int nTextLen = strlen(szText);
		if(nTextLen>0){
			//nLine = pDC->TextMultiLine(textrt, szText, 0, true, nIndentation, nSkipLine);

			// �������ǵǴ� ��ġ�� ������� ��¿ �� ���� TextMultiLine�� ������ ������ ���´�.
			// �ſ� ��ȿ�����̹Ƿ� ���� ���������� �����Ǿ�� �Ѵ�.
			// TextMultiLine�Ӹ� �ƴ϶� TextArea ��ü�� �����ؾ� �Ѵ�. ������ ������ ��ο� Ÿ�ӿ� �������� �ʰ� �̸� �����ϵ���.
			MPOINT* pPositions = NULL;
			if(bCurrentLine==true) pPositions = new MPOINT[nTextLen];
			nLine = pDC->TextMultiLine(textrt, szText, 0, true, nIndentation, nSkipLine, pPositions);

			// �������� �Ӽ�(�����)�� �׸���.
			// TextMulitLine() ȣ���ؾ� ��ġ ������ �� �� �ֱ� ������ ���İ��� ������ �ؽ�Ʈ ������ �׷��ش�. 
			if(pTextArea->IsFocus() && bCurrentLine==true){
				Mint* pMint = Mint::GetInstance();
				const char* szComposition = pTextArea->GetCompositionString();
				int nCompLen = strlen(szComposition);
				for(int i=0; i<nCompLen; i+=(IsHangul(szComposition[i])?2:1)) {
					int pos = pTextArea->GetCaretPos().x+i;
					if (pPositions && pos < nTextLen)
						pMint->DrawCompositionAttribute(pDC, pPositions[pos], pTextArea->GetCompositionString(), i);
				}
			}

			// ��µ� Candidate List ��ġ �����ϱ�
			if(pTextArea->IsFocus()){
				int caretPos = pTextArea->GetCaretPos().x;
				if (pPositions && caretPos < nTextLen)
				{
					Mint* pMint = Mint::GetInstance();
					MPOINT cp = MClientToScreen(pTextArea, pPositions[caretPos]);
					pMint->SetCandidateListPosition(cp, pTextArea->GetLineHeight());
				}
			}

			if(pPositions!=NULL) delete[] pPositions;

		}

		if(pt.y==i) nCarY = textrt.y;
		textrt.y+=nLine*pTextArea->GetLineHeight();
		if(textrt.y>=r.y+r.h) break;
		
		// ĳ���� �׸���.
		// ĳ���� Composition String�ȿ��� ������ �� �����Ƿ�, Composition String�� ���յ� ���ڿ��� �������� ��ġ�� ����.
		MPOINT carpos;
		Mint* pMint = Mint::GetInstance();
		if(bCurrentLine==true && nCarY>=0 && pTextArea->GetCaretPosition(&carpos, 1, szText, pTextArea->GetCaretPos().x+pMint->m_nCompositionCaretPosition, pTextArea->m_bCaretFirst))
		{
			carpos.x+=textrt.x;
			carpos.y=nCarY + carpos.y*pTextArea->GetLineHeight();

			if(pTextArea->IsFocus() && pTextArea->GetEditable() 
				&& timeGetTime()%(MEDIT_BLINK_TIME*2)>MEDIT_BLINK_TIME)
			{
				pDC->Line(carpos.x,carpos.y,carpos.x,carpos.y+pTextArea->GetLineHeight());
			}
		}


		i++;
	}
}

#pragma optimize( "", on )



void MTextAreaLook::OnTextDraw(MTextArea* pTextArea, MDrawContext* pDC)
{
	MFont *pFont=pDC->GetFont();

	MRECT r = pTextArea->GetClientRect();
	r.w -= pTextArea->IsScrollBarVisible() ? pTextArea->GetScrollBarWidth() : 0;
	pDC->SetClipRect(MClientToScreen(pTextArea,r));

	MRECT textrt=r;
	textrt.h=pTextArea->GetLineHeight();

//	pDC->SetColor(MCOLOR(DEFCOLOR_MEDIT_TEXT));
	pDC->SetColor(pTextArea->GetTextColor());

//////////
//char szTmp[256];
//wsprintf(szTmp, "textrt.y=%d, GetLineHeight( pFont->GetHeight())=%d, r.h=%d \n", textrt.y, GetLineHeight( pFont->GetHeight()), r.h);
//OutputDebugString(szTmp);
//////////

	int toline=min(pTextArea->GetLineCount(),pTextArea->GetStartLine()+r.h/pTextArea->GetLineHeight());
	for(int i=pTextArea->GetStartLine();i<toline;i++)
	{
		if(i==pTextArea->GetCaretPos().y)
		{
			string text=string(pTextArea->GetTextLine(i));

			text.insert(pTextArea->GetCaretPos().x,pTextArea->GetCompositionString());
			pDC->Text(textrt,text.c_str(),MAM_LEFT);

		}
		else {
			pDC->Text(textrt,pTextArea->GetTextLine(i),MAM_LEFT);
		}
		textrt.y+=pTextArea->GetLineHeight();
	}

	// ĳ�� �׸���
	if(pTextArea->IsFocus() && pTextArea->GetEditable() 
		&& timeGetTime()%(MEDIT_BLINK_TIME*2)>MEDIT_BLINK_TIME)
	{
		MPOINT pt=pTextArea->GetCaretPos();
		const char *pCarLine=pTextArea->GetTextLine(pt.y);
		string str;
		if(pCarLine)
			str=string(pCarLine,pCarLine+pt.x);
		textrt.x=r.x+pFont->GetWidth(str.c_str());
		textrt.y=r.y+pTextArea->GetLineHeight()*(pt.y-pTextArea->GetStartLine());
		if(r.y<=textrt.y && textrt.y+pTextArea->GetLineHeight()<=r.y+r.h)
			pDC->Text(textrt,"|",MAM_LEFT);
	}

}

void MTextAreaLook::OnDraw(MTextArea* pTextArea, MDrawContext* pDC)
{
	OnFrameDraw(pTextArea, pDC);
//	OnTextDraw(pTextArea, pDC);
	OnTextDraw_WordWrap(pTextArea, pDC);
}

MRECT MTextAreaLook::GetClientRect(MTextArea* pTextArea, MRECT& r)
{
	return MRECT(r.x, r.y, r.w, r.h);
}


void MTextArea::AddText(const char *szText,MCOLOR color)
{
	// �� ���ٿ� �߰��Ѵ�
	m_CurrentLine = m_Lines.end();
	m_CurrentLine--;

	// �ؽ�Ʈ �ȿ� \n �� ���ԵȰ�츦 ó���ؾ��Ѵ�
	int nLineCount = MMGetLineCount(NULL,szText,0,false,m_bColorSupport);
	
	const char *szCurrent=szText;
	for(int i=0;i<nLineCount;i++)
	{
		int nCharCount=MMGetNextLinePos(NULL,szCurrent,0,false,m_bColorSupport);

		m_CurrentLine->color=color;
		m_CurrentLine->text.append(string(szCurrent,nCharCount));
		
		m_Lines.push_back(MLineItem(color,string()));
		m_CurrentLine = m_Lines.end();
		m_CurrentLine--;
		
		szCurrent+=nCharCount;
	}

	// ĳ�� ��ġ�� �ٽ� �� �ڷ�
	m_CaretPos.x=0;
	m_CaretPos.y=GetLineCount()-1;

	m_CurrentLine = m_Lines.end();
	m_CurrentLine--;

	UpdateScrollBar(true);
}

void MTextArea::AddText(const char *szText)
{
	// �� ���ٻ����� �� ���Ѵ�
	m_CurrentLine = m_Lines.end();
	m_CurrentLine--;

	AddText(szText,m_CurrentLine->color);
}

void MTextArea::DeleteFirstLine()
{
	if(GetLineCount()<2) return;

	if(m_CurrentLine==m_Lines.begin())
	{
		m_CurrentLine++;
		m_CaretPos.x=0;
		m_CaretPos.y++;
	}

	m_Lines.erase(m_Lines.begin());
	if(m_nStartLine>0)
		m_nStartLine--;
	UpdateScrollBar();
}

int MTextArea::GetLineHeight( void)
{
	if (m_nCustomLineHeight != -1)
		return m_nCustomLineHeight;
	return GetFont()->GetHeight();
}

void MTextArea::SetCustomLineHeight( int nHeight)
{
	m_nCustomLineHeight = nHeight;
}

void MTextArea::MultiplySize( float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight )
{
	MWidget::MultiplySize( byIDLWidth, byIDLHeight, byCurrWidth, byCurrHeight );

	// ������ ���� ���γ��̵� ����
	if (m_nCustomLineHeight != -1)
		m_nCustomLineHeight = int(m_nCustomLineHeight * byCurrHeight +0.5f);
}

void MTextArea::AdjustHeightByContent()
{
	// ��ü ������ ���� �� �ִ� ũ��� ���� ���̸� ����
	int nStartLine(0), nCurrline(0), nTotalLine(0);
	nTotalLine = GetTotalLineCount(nStartLine, nCurrline);
	int newHeight = nTotalLine * GetLineHeight();
	MRECT rcTextArea = GetRect();
	SetSize(rcTextArea.w, newHeight);
}