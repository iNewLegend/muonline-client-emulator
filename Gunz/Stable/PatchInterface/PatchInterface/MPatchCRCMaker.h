#pragma once
#include "afxwin.h"
#include <string>
using std::string;


// MPatchCRCMaker ��ȭ �����Դϴ�.

class MPatchCRCMaker : public CDialog
{
	DECLARE_DYNAMIC(MPatchCRCMaker)

public:
	MPatchCRCMaker(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~MPatchCRCMaker();
	
	void Clear();

	const DWORD GetServerCRC32() const		{ return m_dwServerCRC32; }
	const DWORD GetAgentCRC32()	const		{ return m_dwAgentCRC32; }

	void SetRootDir( const string& strRootDir )				{ m_strRootDir = strRootDir; }
	void SetListFilePath( const string& strListFilePath )	{ m_strListFilePath = strListFilePath; }

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_PATCH_CRC32MAKER_DIALOG };

protected:
	DWORD MakeFileCRC32( const char* pszFileName );
	void MakeListFile();
    const string GetFileName();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
private:
	DWORD m_dwServerCRC32;
	DWORD m_dwAgentCRC32;

	CEdit m_edtServerName;
	CEdit m_edtAgentName;
	CEdit m_edtGunzClientName;

	string m_strRootDir;
	string m_strListFilePath;

public:
	afx_msg void OnBnClickedFindServerFile();
	afx_msg void OnBnClickedFindAgentFile();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
