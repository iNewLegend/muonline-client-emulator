// MUpdateView.cpp : CMUpdateView Ŭ������ ����
//

#include "stdafx.h"
#include "MUpdate.h"

#include "MUpdateDoc.h"
#include "MUpdateView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMUpdateView

IMPLEMENT_DYNCREATE(CMUpdateView, CHtmlView)

BEGIN_MESSAGE_MAP(CMUpdateView, CHtmlView)
END_MESSAGE_MAP()

// CMUpdateView ����/�Ҹ�

CMUpdateView::CMUpdateView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CMUpdateView::~CMUpdateView()
{
}

BOOL CMUpdateView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	// Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CHtmlView::PreCreateWindow(cs);
}

void CMUpdateView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();
	Navigate2(_T("http://www.gunz.co.kr/update"),NULL,NULL);
}


// CMUpdateView ����

#ifdef _DEBUG
void CMUpdateView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CMUpdateView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

CMUpdateDoc* CMUpdateView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMUpdateDoc)));
	return (CMUpdateDoc*)m_pDocument;
}
#endif //_DEBUG


// CMUpdateView �޽��� ó����
