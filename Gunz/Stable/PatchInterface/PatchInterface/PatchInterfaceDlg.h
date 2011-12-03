// PatchInterfaceDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "MPatchCRCMaker.h"
#include "MScheduleMaker.h"
#include "Patch.h"
#include "MServerInfoDlg.h"
#include "ReloadServerConfigDlg.h"

#define TIMER	1000
#define TIMERID 501

static const char* CONFIG_FILE = ".\\config.ini";

class CReportCtrl;


// CPatchInterfaceDlg ��ȭ ����
class CPatchInterfaceDlg : public CDialog
{
// ����
public:
	CPatchInterfaceDlg(CWnd* pParent = NULL);	// ǥ�� ������

	bool CreateReportCtrl();
	bool LoadConfig();
	bool OnRun();

// ��ȭ ���� ������
	enum { IDD = IDD_PATCHINTERFACE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����

private :
	// CReportCtrl*	m_pReport;
	UINT_PTR				m_nTimerIDCheckProcess;
	MPatchCRCMaker			m_PatchCRCMaker;
	MScheduleMaker			m_ScheduleMaker;
	CPatch					m_Patch;
	MServerInfoDlg			m_ServerInfo;
	ReloadServerConfigDlg	m_ReloadServerConfigDlg;

	string			m_strPatchRootDir;
	string			m_strListFilePath;

// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedSend();
	CEdit m_AnnounceEdit;
	CButton m_AnnounceSendBtn;
	afx_msg void OnConnectKeeper();
	afx_msg void OnDisconnectKeeper();
	afx_msg void OnConnectServer();
	afx_msg void OnDisconnectServer();
	afx_msg void OnAgentStop();
	afx_msg void OnAgentStart();
	afx_msg void OnServerStop();
	afx_msg void OnServerStart();
	afx_msg void OnServerStopWithAnncounce();
	afx_msg void OnRefreshList();
	afx_msg void OnReboot();
	afx_msg void OnPatch();
	afx_msg void OnMakeCrc();
	afx_msg void OnSchedule();
	afx_msg void OnReloadServerConfig();
protected:
	virtual void OnOK();
public:
	afx_msg void OnEnSetfocusAdvertisement();
	afx_msg void OnServerInfo();
	afx_msg void OnLoadserverlist();
};