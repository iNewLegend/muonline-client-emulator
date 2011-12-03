#pragma once


// CPatch ��ȭ �����Դϴ�.

class CPatch : public CDialog
{
	DECLARE_DYNAMIC(CPatch)

public:
	CPatch(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CPatch();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_PATCH_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDownloadServerPatch();
	afx_msg void OnBnClickedPrepareServerPatch();
	afx_msg void OnBnClickedPatchServer();
	afx_msg void OnBnClickedResetServerPatch();
	afx_msg void OnBnClickedDownloadAgentPatch();
	afx_msg void OnBnClickedPrepareAgentPatch();
	afx_msg void OnBnClickedPatchAgent();
	afx_msg void OnBnClickedResetAgentPatch();
};
