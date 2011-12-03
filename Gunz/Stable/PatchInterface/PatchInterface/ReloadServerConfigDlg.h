#pragma once
#include "afxcmn.h"

#include <vector>
#include <string>
#include "afxwin.h"
using namespace std;


#define RELOAD_LIST "reloadlist.txt"


typedef vector< string > ReloadListVec;


// ReloadServerConfigDlg ��ȭ �����Դϴ�.
class ReloadServerConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(ReloadServerConfigDlg)

private :
	ReloadListVec m_ReloadFileList;

private :
	bool InitReport();

public:
	ReloadServerConfigDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~ReloadServerConfigDlg();

	bool LoadReloadList();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ReloadFileListDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
private:
	CListCtrl m_ReloadList;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedReload();
private:
	CEdit m_NewHashValue;
public:
	afx_msg void OnBnClickedAddhashmap();
};
