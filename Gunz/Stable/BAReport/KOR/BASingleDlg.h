#pragma once
#include "afxwin.h"


// BASingleDlg ��ȭ �����Դϴ�.

class BASingleDlg : public CDialog
{
	DECLARE_DYNAMIC(BASingleDlg)

public:
	BASingleDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~BASingleDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_SINGLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	CString *m_pOut;

	virtual BOOL OnInitDialog();
protected:
	virtual void OnCancel();
public:
	CEdit m_Name;
	CEdit m_Symptom;
};
