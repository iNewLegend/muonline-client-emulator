#ifndef _MFILEDIALOG_H
#define _MFILEDIALOG_H

#include "MWidget.h"
#include "MButton.h"
#include "MFileBox.h"
#include "MEdit.h"
#include "MFrame.h"
#include "MMsgBox.h"

/// ���� ���̾�α� Ÿ��
enum MFileDialogType{
	MFDT_OPEN = 0,	///< ���� ���̾�α�
	MFDT_SAVE,		///< ���̺� ���̾�α�
};

/// ���� ���̾�α� �ڽ�
class MFileDialog : public MFrame{
	MButton*		m_pOK;
	MButton*		m_pCancel;
	MFileBox*		m_pFileBox;
	MEdit*			m_pFileName;
	MFileDialogType	m_nType;
	MMsgBox*		m_pMsgBox;
	char			m_szTempPathName[256];
protected:
	//virtual void OnDraw(MDrawContext* pDC);
	virtual bool OnCommand(MWidget* pWindow, const char* szMessage);
	bool IsExistFile(const char* szFileName);

public:
	MFileDialog(const char* szFilter, MWidget* pParent, MListener* pListener=NULL);
	~MFileDialog(void);

	const char* GetFileName(void);
	const char* GetPathName(void);
	void Refresh(const char* szFilter=NULL);

	const char* GetBaseDir(void);

	void SetType(MFileDialogType fdt);

#define MINT_FILEDIALOG	"FileDialog"
	virtual const char* GetClassName(void){ return MINT_FILEDIALOG; }
};

// Listener���� ���޵Ǵ� �޼���
#define MFILEDIALOG_OK		"OK"
#define MFILEDIALOG_CANCEL	"Cancel"

#endif
