// MUpdate.h : MUpdate ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"       // �� ��ȣ


// CMUpdateApp:
// �� Ŭ������ ������ ���ؼ��� MUpdate.cpp�� �����Ͻʽÿ�.
//

class CMUpdateApp : public CWinApp
{
protected:
	void HeartBeat();

public:
	CMUpdateApp();

// ������
public:
	virtual BOOL InitInstance();

// ����
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	virtual int Run();
};

extern CMUpdateApp theApp;

#define MUPDATE_MUTEXNAME	"MUpdateMutexObject"