#pragma once
#include "afxcmn.h"

#include <vector>
#include <string>

using std::vector;
using std::string;

// MServerInfoDlg ��ȭ �����Դϴ�.

class MServerInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(MServerInfoDlg)

public:
	MServerInfoDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~MServerInfoDlg();

	void Init();
	void Reset();

private :
	void InitColumnName();
		
// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SERVER_INFO_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

private:
	CListCtrl			m_ServerInfo;
	vector< string >	m_vColumnName;
};