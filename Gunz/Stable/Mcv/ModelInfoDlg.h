#pragma once
#include "afxwin.h"


// CModelInfoDlg ��ȭ �����Դϴ�.

class CModelInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CModelInfoDlg)

public:
	CModelInfoDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CModelInfoDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MODEL_INFO };

	void ClearListBox();

	void Begin();
	void End();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_ListMeshNode;
	CListBox m_ListMtrlNode;
	afx_msg void OnBnClickedButtonMeshnodeColor();
	afx_msg void OnBnClickedButtonMtrlnodeColor();
};
