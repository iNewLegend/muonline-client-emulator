// MUpdateDoc.h : CMUpdateDoc Ŭ������ �������̽�
//


#pragma once

class CMUpdateDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CMUpdateDoc();
	DECLARE_DYNCREATE(CMUpdateDoc)

// Ư��
public:

// �۾�
public:

// ������
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ����
public:
	virtual ~CMUpdateDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �޽��� �� �Լ��� �����߽��ϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};


