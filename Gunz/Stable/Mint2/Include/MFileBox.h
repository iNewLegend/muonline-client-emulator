#ifndef MFILEBOX_H
#define MFILEBOX_H

#include "MListBox.h"

#ifdef WIN32
	#pragma comment(lib, "Shlwapi.lib")
#endif

/// ���� ����Ʈ �ڽ�
class MFileBox : public MListBox{
	char	m_szFilter[256];
	char	m_szBaseDir[256];
public:
	/// @param	szFilter	���� ���� ( ��: *.exe )
	MFileBox(const char* szFilter, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MFileBox(void);

	/// ���Ͱ����� ��� ��������
	/// @param	szFilter	���� ���� ( ��: *.exe )
	void Refresh(const char* szFilter=NULL);

	/// ���ϸ������� ���̽� ���丮
	const char* GetBaseDir(void);

#define MINT_FILEBOX	"FileBox"
	virtual const char* GetClassName(void){ return MINT_FILEBOX; }
};

#endif
