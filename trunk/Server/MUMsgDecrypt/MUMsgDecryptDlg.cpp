// MUMsgDecryptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MUMsgDecrypt.h"
#include "MUMsgDecryptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMUMsgDecryptDlg dialog

#define SET_NUMBERH(x)		( (BYTE)((DWORD)(x)>>(DWORD)8) )
#define SET_NUMBERL(x)		( (BYTE)((DWORD)(x) & 0xFF) )
#define SET_NUMBERHW(x)		( (WORD)((DWORD)(x)>>(DWORD)16) )
#define SET_NUMBERLW(x)		( (WORD)((DWORD)(x) & 0xFFFF) )
#define MAKE_NUMBERW(x,y)	( (WORD)(((BYTE)((y)&0xFF)) |   ((BYTE)((x)&0xFF)<<8 ))  )
#define MAKE_NUMBERDW(x,y)	( (DWORD)(((WORD)((y)&0xFFFF)) | ((WORD)((x)&0xFFFF)<<16))  )


CMUMsgDecryptDlg::CMUMsgDecryptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMUMsgDecryptDlg::IDD, pParent)
	, m_CString1(_T(""))
	, m_CString2(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMUMsgDecryptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_CString1);
	DDX_Text(pDX, IDC_EDIT2, m_CString2);
}

BEGIN_MESSAGE_MAP(CMUMsgDecryptDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CMUMsgDecryptDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CMUMsgDecryptDlg message handlers

BOOL CMUMsgDecryptDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
 	if(m_SimpleModulusCS.LoadDecryptionKey("Dec2.dat") == FALSE)
 	{
 		MessageBox("Dec1.dat file not found");
	}
	if(m_SimpleModulusSC.LoadEncryptionKey("Enc1.dat") == FALSE)
	{
 		MessageBox("Enc2.dat file not found");
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMUMsgDecryptDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMUMsgDecryptDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void AddDataToString(CString& cstr, const unsigned char* pData, int nSize)
{
	for (size_t i=0;i<nSize;++i)
	{
		cstr.AppendChar((pData[i]>>4)+((pData[i]>>4)<10?'0':('A'-10)));
		cstr.AppendChar((pData[i]&0xF)+((pData[i]&0xF)<10?'0':('A'-10)));
		cstr.AppendChar(' ');
		if (i%16==15)
		{
			cstr.AppendChar(13);
			cstr.AppendChar(10);
		}
	}
	cstr.AppendChar(13);
	cstr.AppendChar(10);
}

void CMUMsgDecryptDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	unsigned char recvbuf[7024]={0};
	int lOfs;
	int size;
	BYTE headcode;
	BYTE xcode;
	int nSentBytes = 0;

	UpdateData(true);
	m_CString2="";
	for (size_t i=0;i<m_CString1.GetLength();++i)
	{
		if (m_CString1[i]>='0'&&m_CString1[i]<='9')
		{
			recvbuf[nSentBytes/2] |= ((m_CString1[i]-'0')<<4)>>(4*(nSentBytes%2));
			nSentBytes++;
		}
		else if (m_CString1[i]>='a'&&m_CString1[i]<='f')
		{
			recvbuf[nSentBytes/2] |= ((m_CString1[i]-'a'+10)<<4)>>(4*(nSentBytes%2));
			nSentBytes++;
		}
		else if (m_CString1[i]>='A'&&m_CString1[i]<='F')
		{
			recvbuf[nSentBytes/2] |= ((m_CString1[i]-'A'+10)<<4)>>(4*(nSentBytes%2));
			nSentBytes++;
		}
	}
	nSentBytes/=2;
	// Check If Recv Data has More thatn 3 BYTES
	//if ( lpIOContext->nSentBytes < 3 )
	{
	//	return TRUE;
	}

	// Initialize Variables
	lOfs=0;
	size=0;
	xcode=0;
	unsigned char byDec[7024];
	// Start Loop
	while ( true )
	{
		// Select packets with
		// C1 or C2 as HEader
		if ( recvbuf[lOfs] == 0xC1 ||
			recvbuf[lOfs] == 0xC3 )
		{
			unsigned char * pBuf;

			// Set Local Var to save values from
			// Cryps
			// th C1 and C3
			pBuf = &recvbuf[lOfs];
			size = pBuf[1];
			headcode = pBuf[2];
			xcode = recvbuf[lOfs];
		}
		else if ( recvbuf[lOfs] == 0xC2 ||
			recvbuf[lOfs] == 0xC4 )
		{
			unsigned char * pBuf;

			// Set Local Var to save values from
			// Cryps
			// th C2 and C4
			pBuf = &recvbuf[lOfs];
			size = pBuf[1] * 256;
			size |= pBuf[2];
			headcode = pBuf[3];
			xcode = recvbuf[lOfs];
		}
		// If HEader Differs
		else
		{
			//MessageBox("error-L1 : Header error (%s %d)lOfs:%d, size:%d",
			//	__FILE__, __LINE__,
			//	lOfs,
			//	lpIOContext->nSentBytes);

			nSentBytes = 0;
			return;// false;
		}

		// Check Size is leess thant 0
		if ( size <= 0 )
		{
			//MessageBox("error-L1 : size %d",size);

			return;// false;
		}

		// Check if Size is On Range
		if ( size <= nSentBytes )
		{
			// Case C3 type
			if ( xcode == 0xC3 )
			{

				// Decrypt Packet
				int ret = m_SimpleModulusCS.Decrypt( &byDec[2],
					&recvbuf[lOfs+2],
					size-2);

				// Case if ret BYTE is Less thant ZERO
				if ( ret < 0 )
				{

				}
				else
				{
					headcode = byDec[3];
					BYTE serial = byDec[2];
					byDec[1] = 0xC3;
					byDec[2] = (ret&0xFF)+2;

					AddDataToString(m_CString2, &byDec[1], ret+2);
				}
			}
			else if ( xcode == 0xC4 )
			{
				int ret = m_SimpleModulusCS.Decrypt(&byDec[3], &recvbuf[lOfs+3], size-3);
				if ( ret < 0 )
				{

				}
				else
				{
					BYTE serial = byDec[3];
					byDec[1] = 0xC4;
					WORD size = (ret &0xFFFF)+3;
					byDec[2] = SET_NUMBERH(size);
					byDec[3] = SET_NUMBERL(size); 
					headcode = byDec[3];
					AddDataToString(m_CString2, &byDec[1], ret+3);
				}
			}
			else
			{
				AddDataToString(m_CString2, &recvbuf[lOfs], size);
			}

			lOfs += size;
			nSentBytes  -= size;

			if ( nSentBytes <= 0 )
			{
				break;
			}
		}
		else if ( lOfs > 0 )
		{
			if ( nSentBytes < 1 )
			{
				MessageBox("error-L1 : recvbuflen 1 %s %d", __FILE__, __LINE__);
				break;
			}

			if ( nSentBytes < 6000 ) 
			{
				memcpy(recvbuf, &recvbuf[lOfs], nSentBytes);
			//	MessageBox("Message copy %d", lpIOContext->nSentBytes);
			//	//break;
			}
			break;

		}
		else
		{
			break;
		}
	}

	UpdateData(false);
}
