// PatchInterface.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ


// CPatchInterfaceApp:
// �� Ŭ������ ������ ���ؼ��� PatchInterface.cpp�� �����Ͻʽÿ�.
//

class CPatchInterfaceApp : public CWinApp
{
public:
	CPatchInterfaceApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
	virtual int Run();

private :
	bool OnRun();
};

extern CPatchInterfaceApp theApp;
