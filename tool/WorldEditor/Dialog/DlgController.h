#pragma once
#include "DlgModelAnim.h"
#include "DlgModelPlayer.h"

class CDlgController :public CUIDialog
{
public:
	CDlgController();
	~CDlgController();
public:
	void C3Model();
	void MPQModel();
	void initRecentPath();
	virtual void OnControlRegister();
	virtual bool OnInitDialog();
	void OnListBoxFolderItemDblClk();

	void OnTabObjectChanged();
	void OpenFile(const std::wstring& wstrFilename);
	void OpenPath(const std::wstring& wstrPath, const std::wstring& wstrFileType=L"*.*");
	void OnClose();
protected:
private:
	std::wstring	m_wstrPath;
	std::wstring	m_wstrFileType;

	CUIRadioButton m_RadioButtonFile;
	CUIRadioButton m_RadioButtonAvatar;

	CDlgModelPlayer	m_DlgModelAvatar;
	CUIListBox		m_ListBoxFolder;
};