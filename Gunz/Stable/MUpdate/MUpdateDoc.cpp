// MUpdateDoc.cpp : CMUpdateDoc Ŭ������ ����
//

#include "stdafx.h"
#include "MUpdate.h"

#include "MUpdateDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMUpdateDoc

IMPLEMENT_DYNCREATE(CMUpdateDoc, CDocument)

BEGIN_MESSAGE_MAP(CMUpdateDoc, CDocument)
END_MESSAGE_MAP()


// CMUpdateDoc ����/�Ҹ�

CMUpdateDoc::CMUpdateDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.

}

CMUpdateDoc::~CMUpdateDoc()
{
}

BOOL CMUpdateDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���⿡ �ٽ� �ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	return TRUE;
}




// CMUpdateDoc serialization

void CMUpdateDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}


// CMUpdateDoc ����

#ifdef _DEBUG
void CMUpdateDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMUpdateDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMUpdateDoc ���
