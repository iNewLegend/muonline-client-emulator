// MUpdateView.h : iCMUpdateView Ŭ������ �������̽�
//


#pragma once


class CMUpdateView : public CHtmlView
{
protected: // serialization������ ��������ϴ�.
	CMUpdateView();
	DECLARE_DYNCREATE(CMUpdateView)

// Ư��
public:
	CMUpdateDoc* GetDocument() const;

// �۾�
public:

// ������
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ���� �� ó�� ȣ��Ǿ����ϴ�.

// ����
public:
	virtual ~CMUpdateView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �޽��� �� �Լ��� �����߽��ϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // MUpdateView.cpp�� ����� ����
inline CMUpdateDoc* CMUpdateView::GetDocument() const
   { return reinterpret_cast<CMUpdateDoc*>(m_pDocument); }
#endif

