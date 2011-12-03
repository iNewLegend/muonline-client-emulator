#pragma once
#include "afxwin.h"


// CMtrlEditDlg ��ȭ �����Դϴ�.
class RMtrl;
class RMtrlMgr;
class CMtrlEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CMtrlEditDlg)

public:
	CMtrlEditDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMtrlEditDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MTRLEDITDLG };

	RMtrlMgr* GetMtrlMgr();
	RMtrl* GetMtrl(int index);
	void UpdateMtrl(RMtrl* pMtrl);
	void UpdateName();

	void ClearListBox();

	void Begin();
	void End();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_mtrl_list_box;
	CString m_texture_name;
	CString m_alpha_texture_name;
	CButton m_SaveButton;
	afx_msg void OnBnClickedOksave();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnLbnSelchangeMtrllist();
	BYTE m_nAlphaRefValue;
};
